

#include "chaining_methods.h"

// Метод прямого вывода через рекурсию
bool forward_chain_with_cycle(IMD::system_of_rules& sr,
    const std::string& target,
    std::vector<std::string>& log) {
    bool changed(true); // Флаг, что за текущий проход появились новые факты
    int step(0); // Номер шага вывода

    while (changed) {
        changed = false;

        for (const auto& [rule_id, r] : sr.get_rules()) { // Обход всех правил
            bool applicable(true); // Флаг, что все посылки правила есть в рабочей памяти
            for (const auto& in_id : r.get_input_facts()) {
                if (!sr.contains_fact(in_id)) { // Посылки нет в рабочей памяти — правило неприменимо
                    applicable = false;
                    break;
                }
            }
            if (!applicable) continue; // Правило неприменимо
            if (sr.contains_fact(r.get_output_fact())) continue; // Выходной факт уже есть в рабочей памяти

            IMD::fact out = sr.get_or_create_fact(r.get_output_fact()); // Получение выходного факта из каталога или создание нового
            sr.add_working_fact(out); // Добавление выходного факта в рабочую память
            changed = true;

            std::string desc = r.get_description().empty() ? "" : " (" + r.get_description() + ")"; // Описание правила (если есть)

            std::string wm; // Снимок рабочей памяти
            for (const auto& [id, f] : sr.get_working_memory()) { // Обход рабочей памяти
                wm += '\t' + id + '\n';
            }
            if (wm.empty()) wm = "(пусто)";

            log.push_back("Шаг " + std::to_string(++step) + ": " + rule_id + desc + " -> " + r.get_output_fact() +
                "\nРабочая память:\n" + wm); // Логирование шага + рабочая память
        }
    }

    return target.empty() || sr.contains_fact(target);
}

// Метод обратного вывода - основан на DFS AND-OR дерева
bool backward_dfs(IMD::system_of_rules& sr, const std::string& target, std::vector<std::string>& log) {
    if (target.empty()) return false;
}

