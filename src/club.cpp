#include "club.hpp"

namespace my
{
    bool Table::take(const Time& take_time)
    {
        if (!empty())
        {
            return false;
        }

        m_taken = true;
        m_take_time = take_time;

        return true;
    }

    bool Table::free(Time free_time)
    {
        if (empty())
        {
            return false;
        }

        m_taken = false;

        //подсчет суммарного времени занятости стола
        m_total_time = m_total_time + (free_time - m_take_time);

        //посдчет суммарных часов для расчета платы
        m_hours = m_hours + (free_time - m_take_time).get_hours_rup();

        return true;
    }

    bool Client::sit(Table& table, const Time& time)
    {
        if (!table.empty())
        {
            return false;
        }

        if (m_table)
        {
            m_table->free(time);
        }

        m_table = &table;

        m_table->take(time);

        return true;
    }

    bool Client::leave(const Time& time)
    {
        if (!m_table)
        { 
            return false;
        }

        m_table->free(time);
        m_table = nullptr;
        return true;
    }

    std::ostream& operator<< (std::ostream& out, Club::error_e error)
    {
        using enum Club::error_e;
        switch (error)
        {
        case already_in:
            std::cout<<"YouShallNotPass";
            break;

        case not_open:
            std::cout<<"NotOpenYet";
            break;

        case busy:
            std::cout<<"PlaceIsBusy";
            break;

        case unknown_client:
            std::cout<<"ClientUnknown";
            break;

        case free_table:
            std::cout<<"ICanWaitNoLonger!";
            break;
        
        default:
        std::cout<<"???";
            break;
        }

        return out;
    }

    int Club::free_table() const
    {
        for (std::size_t i {}; i < m_tables.size(); ++i)
        {
            if (m_tables[i].empty())
                return static_cast<int>(i);
        }
        return -1;
    }

    int Club::client_waits() const
    {
        for (std::size_t i {}; i < m_clients.size(); ++i)
        {
            if (m_clients[i].waits())
                return static_cast<int>(i);
        }
        return -1;
    }

    int Club::find_client(const std::string_view name) const
    {
        for (std::size_t i {}; i < m_clients.size(); ++i)
        {
            if (m_clients[i].get_name() == name)
                return static_cast<int>(i);
        }
        return -1;
    }

    bool Club::client_sit(const Time& time, const std::string_view name,const int table)
    {
        int index {find_client(name)};
        
        if (index < 0)
            return false;

        return m_clients[static_cast<std::size_t>(index)].sit(m_tables[static_cast<std::size_t>(table)], time);
    }

    bool Club::add_client(std::string_view name)
    {
        if (in_list(name))
            return false;

        m_clients.push_back(Client{name});
        return true;
    }

    bool Club::client_out(const Time& time, std::string_view name)
    {
        int index {find_client(name)};
        if (index < 0)
            return false;

        m_clients[static_cast<std::size_t>(index)].leave(time);
        m_clients.erase(m_clients.begin() + index);
        return true;
    }

    bool Club::check_event(const Event& event) const
    {
            //имя не соответсвует ограничениям
        if (!correct_name(event.name) ||
            //время раньше, чем прошлое событие
            event.time < m_last_event ||
            //некорректное событие
            event.event >= Event::client_max_event ||
             //номер стола больше, чем должен
            (event.event == Event::client_sit && event.table > m_tables.size()) ||
            //валидный стол, а событие не подходит
            (event.event != Event::client_sit && event.table != 0) ||
            //событие не "клиент пришел", а время раньше открытия
            (event.event != Event::client_in && event.time < m_start)
            )
            return false;

        return true;
    }

    void Club::log_event(const Event& event)
    {   
        //несколько валидаций события
        if (!check_event(event))
            throw std::runtime_error{""};

        m_last_event = event.time;

        //дополнительные штуки (исключения, рассадка) для событий
        switch (event.event)
        {
        case Event::client_in:
            if (event.time < m_start)
            {
                print_error(event.time, error_e::not_open);
                break;
            }
            if (!add_client(event.name))
            {
                print_error(event.time, error_e::already_in);
            }
            break;

        case Event::client_sit:
            if(!in_list(event.name))
            {
                print_error(event.time, error_e::unknown_client);
                break;
            }
            if(!client_sit(event.time, event.name, event.table - 1))
            {
                print_error(event.time, error_e::busy);
            }
            break;

        case Event::client_wait:
            if(!in_list(event.name))
            {
                print_error(event.time, error_e::unknown_client);
                break;
            }
            if(free_table() != - 1)
            {
                print_error(event.time, error_e::free_table);
            }
            else
            {
                ++queue;
            }

            //если ожидает и очередь больше кол-ва столов, клиента надо выпроводить
            if (queue > m_tables.size())
            {
                client_out(event.time, event.name);
                Event{event.time, Event::club_out, event.name}.print();
                --queue;
            }

            
            break;

        case Event::client_out:
            if(!client_out(event.time, event.name))
            {
                print_error(event.time, error_e::unknown_client);
                break;
            }
            //если есть очередь, стол надо занять
            if (queue > 0)
            {
                int table {free_table()};
                int client {client_waits()};
                client_sit(event.time, client, table);
                Event{event.time, Event::club_sit, m_clients[static_cast<std::size_t>(client)].get_name().data(), table+1}.print();
                --queue;

            }
            break;
        }
    }

    void Club::close_and_print()
    {
        std::sort(m_clients.begin(), m_clients.end(), 
            [](const auto& c1, const auto& c2)
            {
                return c1.get_name() > c2.get_name();
            }
        );
        for(Client& client : m_clients)
        {
            client.leave(m_end);
            Event{m_end, Event::club_out, client.get_name().data(), 0}.print();
        }
    }

    void Club::print_profit()
    {
        int table_num {1};
        for (const Table& table : m_tables)
        {
            std::cout<<"Table " << table_num++<<": "
            <<"revenue "<<table.get_hours() * m_rate
            <<", total time "<<table.get_time()<<"\n";
        }
    }
}