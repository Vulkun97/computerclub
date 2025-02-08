#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <iostream>
#include <string>
#include <exception>
#include <fstream>
#include <format> 

namespace my
{
class Time
{
    int m_hours{};
    int m_minutes{};
public:
    Time(int hours, int minutes) : m_hours{hours} , m_minutes{minutes}
    {
        if (0 > hours || hours > 23 || 0 > minutes || minutes > 59)
        {
            throw std::runtime_error{"Exception creating Time type"};
        }
    }

    Time(std::string_view str)
    {
        if (str.size() != 5)
            throw std::runtime_error{"Exception creating Time type"};

        m_hours = std::stoi(str.data());
        str.remove_prefix(3);
        m_minutes = std::stoi(str.data());
    }

    Time() : Time{0,0} {}

    Time& operator=(const Time& time)
    {
        if (this == &time)
            return *this;

        m_hours = time.m_hours;
        m_minutes = time.m_minutes;
        return *this;
    }

    Time(const Time& time)
    {
        *this = time;
    }

    void set_time(int hours, int minutes)
    {
        if (0 > hours || hours > 23 || 0 > minutes || minutes > 59)
        {
            throw std::runtime_error{"Exception setting Time type"};
        }

        m_hours = hours;
        m_minutes = minutes;
    }

    void set_time(const Time& time)
    {
        *this = time;
    }

    friend std::ostream& operator<< (std::ostream& out,const Time& time)
    {
        out<<std::format("{:02}:{:02}", time.m_hours, time.m_minutes);
        return out;
    }

    friend bool operator==(const Time& time1,const Time& time2)
    {
        return time1.m_hours == time2.m_hours && time1.m_minutes == time2.m_minutes;
    }

    friend bool operator!=(const Time& time1,const Time& time2)
    {
        return !(time1 == time2);
    }

    friend bool operator>(const Time& time1,const Time& time2)
    {
        if(time2.m_hours > time1.m_hours)
            return false;

        if(time2.m_hours < time1.m_hours)
            return true;

        if(time2.m_minutes < time1.m_minutes)
            return true;

        return false;
    }

    friend bool operator<(const Time& time1,const Time& time2)
    {
        return time2>time1;
    }

    friend bool operator>=(const Time& time1,const Time& time2)
    {
        return !(time1<time2);
    }

    friend bool operator<=(const Time& time1,const Time& time2)
    {
        return !(time1>time2);
    }

    friend Time operator+(const Time& time1,const Time& time2)
    {
        int hours {};
        int minutes {time1.m_minutes + time2.m_minutes};
        if (minutes > 59)
        {
            minutes = minutes%60;
            ++hours;
        }

        hours = (hours + time1.m_hours + time2.m_hours) % 24;

        return Time{hours, minutes};
    }

    friend Time operator-(const Time& time1,const Time& time2)
    {
        int hours{}; 
        int minutes{time1.m_minutes - time2.m_minutes};
        if (minutes < 0)
        {
            --hours;
            minutes = 60 + minutes;
        }

        hours = hours + time1.m_hours - time2.m_hours;

        if (hours < 0)
        {
            hours = 24 + hours;
        }

        return Time{hours, minutes};
    }

    int get_hours_rup()
    {
        return (m_minutes == 0 ? m_hours : m_hours + 1);
    }
};

struct Event
{
    enum event_e
    {
        client_in = 1,
        client_sit,
        client_wait,
        client_out,
        client_max_event,
        club_min_event = 10,
        club_out,
        club_sit,
        club_error,
        club_max_event,
    };

    friend std::ostream& operator<< (std::ostream& out, event_e event)
    {
        out<<static_cast<int>(event);
        return out;   
    }

    Time time;
    event_e event; 
    std::string name;
    int table;

    void print()
    {
        std::cout<<time<<" "<<event<<" "<<name;
        if (event == Event::client_sit || event == Event::club_sit)
        {
            std::cout<<" "<<table;
        }
        std::cout<<std::endl;
    }
};

class if_handler
{
    std::ifstream m_file{};
    int m_lines{};
    std::string_view m_line_view{};
    std::string m_line{};

public:
    if_handler(std::string name)
    {
        m_file.open(name);
        if (!m_file.is_open())
            throw std::runtime_error{"Error opening file"};
    }

    ~if_handler() {m_file.close();}

    if_handler(const if_handler&) = delete;

    if_handler operator=(const if_handler&) = delete;

    //чтение следующей строки в файле
    bool new_line()
    {
        bool eof {std::getline(m_file, m_line)};
        m_line_view = m_line;

        if (m_file.bad())
            throw std::runtime_error{"Error reading line"};     
        
        return eof;
    }

    //чтение следующего слова в строке, если строка закончилась возвращает фолс
    bool read(std::string& word)
    {
        auto begin {m_line_view.begin()};
        while(begin != m_line_view.end() && *begin == ' ')
            ++begin;

        if (begin == m_line_view.end())
        {
            word.clear();
            return false;
        }
            
        auto end {begin};
        while(end != m_line_view.end() && *end != ' ')
            ++end;

        word = m_line_view.substr(begin - m_line_view.begin(), end - begin);

        m_line_view.remove_prefix(end - m_line_view.begin());

        return end != m_line_view.end();
    }

    //чтение инт из строки, если строка закончилась возвращает фолс, если слово не инт - бросает исключение
    bool read(int& number)
    {
        std::string word{};
        bool eol {read(word)};

        if (word.empty())
        {
            number = 0;
            return false;
        }
            
        number = std::stoi(word);

        return eol;
    }

    //возвращает целую прочитанную строку
    const std::string_view get_line() {return m_line;}

    //чтение времени из строки, если строка закончилась возвращает фолс, если слово не время - бросает исключение
    bool read(Time& time)
    {
        std::string word{};
        bool eof {read(word)};

        time = Time{word};
        
        return eof;
    }

    //чтение события из строки, если строка закончилась возвращает фолс, если строка не событие - бросает исключение
    void read(Event& event)
    {
        if(!read(event.time))
            throw std::runtime_error{""};

        int number{};

        if(!read(number))
            throw std::runtime_error{""};


        event.event = static_cast<Event::event_e>(number);

        if (!read(event.name))
        {
            event.table = 0;
            return;
        }

        if(read(event.table))
        {
            throw std::runtime_error{""};
        }
    }
};
}






#endif