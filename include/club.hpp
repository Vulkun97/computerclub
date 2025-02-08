#ifndef CLUB_HPP
#define CLUB_HPP

#include <iostream>
#include <string>
#include <exception>

#include <fstream>
#include <vector>
#include <format>
#include <algorithm>

#include "utility.hpp"

namespace my
{
class Table
{
    bool m_taken {};
    int m_hours{};
    Time m_take_time{};
    Time m_total_time{};

    //сидит ли кто нибудь за столом
    bool empty() const { return !m_taken; }

    //прикрепить стол к клиенту
    bool take(const Time& take_time);

    //открепить стол от клиента
    bool free(Time free_time);

    int get_hours() const { return m_hours;}

    const Time& get_time() const { return m_total_time;}

    friend class Client;
    friend class Club;
};

class Client
{
    std::string m_name {};
    Table* m_table {};

    Client() = delete;
    Client(std::string_view name) : m_name{name} {};

    //прикрепить стол к клиенту
    bool sit(Table& table, const Time& time);

    //открепить стол от клиента
    bool leave(const Time& time);

    //закреплен ли за крълиентом стол
    bool waits() const { return m_table == nullptr; }

    const std::string_view get_name() const { return m_name; }

    friend class Club;
};

class Club
{
    std::vector<Table> m_tables {};
    std::vector<Client> m_clients {};
    int m_rate {};
    int queue {};
    Time m_start{};
    Time m_end {};
    Time m_last_event{};

    enum class error_e
    {
        already_in,
        not_open,
        busy,
        unknown_client,
        free_table,
        max_error,
    };

    friend std::ostream& operator<< (std::ostream& out, error_e error);

    static void print_error(const Time& time, const error_e error)
    {
        std::cout<<time<<" 13 "<<error<<std::endl;
    }

    //возвращает индеус свободного стола, если свободног остола нет возвращает -1
    int free_table() const;

    //возвращает индекс ждущего клиента, если ждущего клиента нет возвращает -1
    int client_waits() const;

    //поиск клиента по имени, если клиента нет возвращает - 1
    int find_client(const std::string_view name) const;

    //закрепляет за клиентом стол, если не удалось возвращает фолс
    bool client_sit(const Time& time, const int client, const int table)
    {
        return m_clients[static_cast<std::size_t>(client)].sit(m_tables[static_cast<std::size_t>(table)], time);
    }

    //закрепляет за клиентом стол, если не удалось возвращает фолс
    bool client_sit(const Time& time, const std::string_view name,const int table);

    //проверяет есть ли клиент в клубе
    bool in_list(std::string_view name) const
    {
        return find_client(name) != -1;
    }

    //добавить клиента в клуб
    bool add_client(std::string_view name);

    //исключить клиента из клуба
    bool client_out(const Time& time, std::string_view name);

    static bool correct_letter(const unsigned char c)
    {   
        return (std::islower(c) || std::isdigit(c) || '_' == c || '-' == c);
    }

    //проверка имени клиента
    static bool correct_name(const std::string_view name)
    {
        if (name.empty())
            return false;

        for (const auto& e : name)
        {
            if (!correct_letter(e))
                return false;
        }

        return true;
    }

    //проверка события на валидность
    bool check_event(const Event& event) const;

public:
    Club () = delete;
    Club (int tables) : m_tables{static_cast<std::size_t>(tables)} {}

    Club (const Club&) = delete;
    Club operator=(const Club&) = delete;

    bool set_rate (int rate)
    {
        if (rate <= 0)
            return false;

        m_rate = rate;

        return true;
    }

    bool set_time (const Time& start, const Time& end)
    {
        if (end < start)
            return false;

        m_start = start;
        m_end = end;

        return true;
    }

    //запись события 
    void log_event(const Event& event);

    const Time& get_start() { return m_start; }

    const Time& get_end() { return m_end; }

    //закрыть клуб и напечатать всех клиентов, которых пришлось выпроводить
    void close_and_print();

    //печать прибыли по всем столам за день
    void print_profit();
};
}


#endif