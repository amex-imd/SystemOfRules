#include <iostream>
#include <string>
#include <vector>
#include <windows.h>
#include <clocale>
#include <iostream>
#include <fstream>
#include <format>
#include "production_model.h"

// Чтение одной цели из файла
static std::string read_target(const std::string& file_path) {
    std::ifstream file(file_path);
    if (!file.is_open())
        throw std::runtime_error(std::format("The file '{}' can't be opened", file_path)); // Файл не открылся

    std::string line;
    while (std::getline(file, line)) {
        // Обрезаем пробельные символы по краям
        auto begin = line.find_first_not_of(" \t\r\n\f\v");
        if (begin == std::string::npos) continue; // Пустая строка
        auto end = line.find_last_not_of(" \t\r\n\f\v");
        line = line.substr(begin, end - begin + 1);

        if (line.empty() || line[0] == '#') continue; // Комментарий
        return line; // Первая непустая строка — цель
    }
    return ""; // Цель не найдена
}


int main(int argc, char* argv[]) {
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    setlocale(LC_ALL, ".UTF-8");

    try {
        std::string subject_domain_path = "subject_domain.txt";
        std::string axioms_path = "axioms.txt";
        std::string rules_path = "rules.txt";
        std::string targets_path = "targets.txt";

        IMD::production_model pm;

        IMD::parser::parse_subject_domain(subject_domain_path, pm);
        IMD::parser::parse_axioms(axioms_path, pm);
        IMD::parser::parse_rules(rules_path, pm);
        std::string target = read_target(targets_path);

        std::cout << "Выберите режим вывода (1 - прямой, 2 - обратный)" << std::endl;
        std::cout << "Предметная область: фактов " << pm.get_subject_facts().size()
            << ", аксиом " << pm.get_axioms().size()
            << ", правил " << pm.get_rules().size() << "\n";
        std::cout << "Цель: " << target << "\n\n";

        std::cout << "Режим: 1 - прямой, 2 - обратный\n> ";
        int mode = 0;
        std::cin >> mode;

        if (mode == 1) {
            bool reachness = IMD::forward_chaining(pm, target); // Прямой вывод

            std::cout << "\n-------- ПРЯМОЙ ВЫВОД --------\n";
            std::cout << (reachness ? "ЦЕЛЬ ДОСТИГНУТА\n" : "ЦЕЛЬ НЕ ДОСТИГНУТА\n");
        }
        else if (mode == 2) {
            std::cout << "\nОбратный вывод пока не реализован\n";
        }
        else {
            std::cout << "Неверный режим\n";
            return 1;
        }

        return 0;


    }
    catch (const std::exception& ex) {
        std::cerr << "Ошибка: " << ex.what() << "\n";
        return 1;
    }
    return 0;
}