#include <iostream>
#include <string>
#include <exception>

#include "utility.hpp"
#include "club.hpp"

int main (int argc, char* argv[])
{

    if (argc != 2)
    {
        std::cerr<<"Usage:\n./<program_name> <file_name.txt>\n";
        return 1;
    }

    int tables{};
    int rate {};
    my::Time start, end {};

    try
    {
        my::if_handler file {argv[1]};
        //my::if_handler file {"test.txt"};


        //парсинг шапки
        
        //кол-во столов
        file.new_line();
        //если не смог прочитать инт или столов меньше 1
        if(file.read(tables) || tables < 1)
            throw std::runtime_error{file.get_line().data()};

        //создание клуба
        my::Club club{tables};

        //время открытия и закрытия
        file.new_line();
        if(!file.read(start))
            throw std::runtime_error{file.get_line().data()};
        if(file.read(end))
            throw std::runtime_error{file.get_line().data()};


        //если не смог задать время (открытие позже закрытия - логическая ошибка) - вызов исключения
        if(!club.set_time(start, end))
            throw std::runtime_error{file.get_line().data()};

        //стоимость часа
        file.new_line();
        //если не смог прочитать инт или стоимость меньше 1
        if(file.read(rate) || rate < 1)
            throw std::runtime_error{file.get_line().data()};

        //задание ставки
        club.set_rate(rate);

        
        std::cout<<club.get_start()<<"\n";

        //парсинг тела файла

        //создание объекта типа событие, который служит в роли посредника (в него парсим строку, затем передаем его клубу)
        my::Event event{};

        //читаем линиию
        while(file.new_line())
        {
            //печатаем линию
            std::cout<<file.get_line()<<"\n";
            
            //извлекаем из линии событие, если линия не соответствует требованиям - исключение
            file.read(event);

            //принимаем событие (записываем в клуб), если событие некорректное логически - исключение
            club.log_event(event);
        }

        club.close_and_print();
        
        std::cout<<club.get_end()<<"\n";

        club.print_profit();
    
    }
    catch(const std::exception& e)
    {
        std::cout<<e.what()<<"\n";
        return 2;
    }
    
    return 0;
}