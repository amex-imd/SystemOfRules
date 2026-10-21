#ifndef __CHAINING_METHODS_IMD__
#define __CHAINING_METHODS_IMD__

#include <string>
#include <vector>
#include <unordered_set>
#include <memory>

#include "system_of_rules.h"

namespace IMD {
    enum kind_type {OR, AND};
    enum status_type {FALSE, UNKNOWN, TRUE};

	class node {
	public:
		enum class kind_type { AND, OR };

		// Конструктор
		node(kind_type type, const std::string& id, const std::unordered_set<std::string>& path)
			: __type(type), __id(id), __path(path), __cost(std::numeric_limits<int>::max()), __status(status_type::UNKNOWN) {
		}

		kind_type get_type() const noexcept { return this->__type; } // Получение типа узла
		const std::string& get_id() const noexcept { return this->__id; } // Получение ID узла
		int get_cost() const noexcept { return this->__cost; } // Получение эвристики
		status_type get_status() const noexcept { return this->__status; } // Получение статуса
		const std::unordered_set<std::string>& get_path() const noexcept { return this->__path; } // Путь от корня
		node* get_parent() const noexcept { return this->__parent; } // Родитель
		const std::vector<std::shared_ptr<node>>& get_children() const noexcept { return this->__children; } // Дети

		void set_cost(int new_cost) noexcept { this->__cost = new_cost; } // Установка эвристики
		void set_status(status_type new_status) noexcept { this->__status = new_status; } // Установка статуса

		void add_child(std::shared_ptr<node> child) { // Добавление ребёнка
			child->__parent = this;
			this->__children.push_back(std::move(child));
		}
		bool leaf() const {
			return this->__children.empty();
		} // Проверка, является ли узел листом

	private:
		kind_type __type; // Тип узла
		std::string __id; // FactId для OR, RuleId для AND
		int __cost; // Эвристика
		status_type __status;
		std::unordered_set<std::string> __path; // ID фактов на пути от корня
		node* __parent; // Родитель (владение через __children)
		std::vector<std::shared_ptr<node>> __children; // Дети
	};



    // Метод прямого вывода
    bool forward_chain(IMD::system_of_rules& sr,
        const std::string& target,
        std::vector<std::string>& log);

}
#endif // !__CHAINING_METHODS_IMD__
