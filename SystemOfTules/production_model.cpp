#include <cctype>
#include <cmath>
#include <string>
#include <stdexcept>
#include <format>
#include <sstream>
#include <iostream>
#include <fstream>
#include <unordered_set>
#include <algorithm>
#include <queue>
#include <memory>
#include <limits>

#include "production_model.h"

namespace IMD {
	// -------- FACT --------

	// Конструктор факта
	fact::fact(const std::string& id, const std::string& description, double confidence_coefficient)
		: __id(id), __description(description), __confidence_coefficient(confidence_coefficient) {
	}

	// Создание факта
	fact fact::create(const std::string& id, const std::string& description, double confidence_coefficient) {
		if (id.empty())
			throw std::invalid_argument(std::format("The argument 'id' ('{}') can't be empty", id)); // ID не может быть пустым
		if (!std::isfinite(confidence_coefficient))
			throw std::invalid_argument(std::format("The argument 'confidence_coefficient' ({}) must be finite", confidence_coefficient)); // Коэффициент уверенности должен быть конечным
		if (confidence_coefficient < MIN_CONFIDENCE_COEFFICIENT_VALUE ||
			confidence_coefficient > MAX_CONFIDENCE_COEFFICIENT_VALUE)
			throw std::invalid_argument(std::format("The argument 'confidence_coefficient' ({}) is out of range [-1; +1]", confidence_coefficient)); // Коэффициент уверенности вне диапазона
		return fact(id, description, confidence_coefficient);
	}

	const std::string& fact::get_id() const noexcept { return this->__id; } // Получение ID факта
	const std::string& fact::get_description() const noexcept { return this->__description; } // Получение описание факта
	double fact::get_confidence_coefficient() const noexcept { return this->__confidence_coefficient; } // Получение коэффициента уверенности в факте

	// -------- RULE --------

	// Конструктор правила
	rule::rule(const std::string& id,
		const std::vector<std::string>& input_facts,
		const std::string& output_fact,
		double confidence_coefficient,
		const std::string& description)
		: __id(id), __input_facts(input_facts), __output_fact(output_fact),
		__confidence_coefficient(confidence_coefficient), __description(description) {
	}

	// Создание правила
	rule rule::create(const std::string& id,
		const std::vector<std::string>& input_facts,
		const std::string& output_fact,
		double confidence_coefficient,
		const std::string& description)
	{
		if (id.empty())
			throw std::invalid_argument(std::format("The argument 'id' ('{}') can't be empty", id)); // ID не может быть пустым
		if (input_facts.empty())
			throw std::invalid_argument(std::format("The argument 'input_facts' of rule '{}' can't be empty", id)); // Входные факты не могут быть пустыми
		if (!std::isfinite(confidence_coefficient))
			throw std::invalid_argument(std::format("The argument 'confidence_coefficient' ({}) of rule '{}' must be finite", confidence_coefficient, id)); // Коэффициент уверенности должен быть конечным
		if (confidence_coefficient < MIN_CONFIDENCE_COEFFICIENT_VALUE ||
			confidence_coefficient > MAX_CONFIDENCE_COEFFICIENT_VALUE)
			throw std::invalid_argument(std::format("The argument 'confidence_coefficient' ({}) of rule '{}' is out of range [-1; +1]", confidence_coefficient, id)); // Коэффициент уверенности вне диапазона
		return rule(id, input_facts, output_fact, confidence_coefficient, description);
	}

	const std::string& rule::get_id() const noexcept { return this->__id; } // Получение ID правила
	const std::vector<std::string>& rule::get_input_facts() const noexcept { return this->__input_facts; } // Получение входных фактов (посылок) правила
	const std::string& rule::get_output_fact() const noexcept { return this->__output_fact; } // Получение выходного факта правила
	double rule::get_confidence_coefficient() const noexcept { return this->__confidence_coefficient; } // Получение коэффициента уверенности
	const std::string& rule::get_description() const noexcept { return this->__description; } // Получение описание правила

	// -------- SYSTEM OF RULES --------

	// Добавление факта в каталог
	void production_model::add_subject_fact(const fact& fact) {
		auto [it, inserted] = this->__subject_domain.emplace(fact.get_id(), fact);
		if (!inserted)
			throw std::invalid_argument(std::format("The fact '{}' already exists in the model", fact.get_id())); // Факт уже существует
	}

	// Добавление аксиомы
	void production_model::add_axiom(const fact& axiom) {
		auto [it, inserted] = this->__axioms.emplace(axiom.get_id(), axiom);
		if (!inserted)
			throw std::invalid_argument(std::format("The axiom '{}' already exists", axiom.get_id())); // Аксиома уже существует
		this->__working_memory.emplace(axiom.get_id(), axiom); // Аксиома сразу активна
	}

	// Добавление правила
	void production_model::add_rule(const rule& rule) {
		auto [it, inserted] = this->__rules.emplace(rule.get_id(), rule);
		if (!inserted)
			throw std::invalid_argument(std::format("The rule '{}' already exists", rule.get_id())); // Правило уже существует
	}

	// Добавление факта в рабочую память
	void production_model::add_working_fact(const fact& fact) {
		// Никаких проверок
		this->__working_memory.emplace(fact.get_id(), fact);
	}

	// Получение факта из предметной области
	fact production_model::get_subject_fact(const std::string& id) const {
		auto it = this->__subject_domain.find(id);
		if (it == this->__subject_domain.end())
			throw std::invalid_argument(std::format("The fact '{}' was not found in the model", id)); // Факт не найден
		return it->second;
	}

	// Получение факта из рабочей памяти
	fact production_model::get_working_fact(const std::string& id) const {
		auto it = this->__working_memory.find(id);
		if (it == this->__working_memory.end())
			throw std::invalid_argument(std::format("The fact '{}' was not found in the model", id)); // Факт не найден
		return it->second;
	}

	// Получение аксиомы
	fact production_model::get_axiom(const std::string& id) const {
		auto it = this->__axioms.find(id);
		if (it == this->__axioms.end())
			throw std::invalid_argument(std::format("The axiom '{}' was not found", id)); // Аксиома не найдена
		return it->second;
	}

	// Получение правила
	rule production_model::get_rule(const std::string& id) const {
		auto it = this->__rules.find(id);
		if (it == this->__rules.end())
			throw std::invalid_argument(std::format("The rule '{}' was not found", id)); // Правило не найдено
		return it->second;
	}

	// Проверка наличия факта в рабочей памяти
	bool production_model::contains_working_fact(const std::string& id) const {
		return this->__working_memory.find(id) != this->__working_memory.end();
	}

	// Проверка наличия факта в каталоге
	bool production_model::contains_subject_fact(const std::string& id) const {
		return this->__subject_domain.find(id) != this->__subject_domain.end();
	}

	// Проверка наличия аксиомы
	bool production_model::contains_axiom(const std::string& id) const {
		return this->__axioms.find(id) != this->__axioms.end();
	}

	// Получение всех правил
	const std::unordered_map<std::string, rule>& production_model::get_rules() const noexcept {
		return this->__rules;
	}

	// Получение всех фактов каталога
	const std::unordered_map<std::string, fact>& production_model::get_subject_facts() const noexcept {
		return this->__subject_domain;
	}

	// Получение всех аксиом
	const std::unordered_map<std::string, fact>& production_model::get_axioms() const noexcept {
		return this->__axioms;
	}

	// Получение рабочей памяти
	const std::unordered_map<std::string, fact>& production_model::get_working_memory() const noexcept {
		return this->__working_memory;
	}

	// -------- PARSER --------

	// Разбор файла фактов модели
	void parser::parse_subject_domain(const std::string& file_path, production_model& kb) {
		std::ifstream file(file_path);
		if (!file.is_open())
			throw std::runtime_error(std::format("The file '{}' can't be opened", file_path)); // Файл не открылся

		std::size_t idx(0); // Индекс текущей строки
		std::string line; // Текущая строка
		while (std::getline(file, line)) {
			++idx;
			line = trim(line);

			if (line.empty() || line[0] == '#') continue; // Пустая строка или комментарий

			auto tokens = split(line, ';'); // Разбиение на токены по разделителю
			if (tokens.size() != 3)
				throw std::runtime_error(std::format("facts line {}: expected 3 fields, got {}", idx, tokens.size())); // Неверное число полей

			const std::string& id = tokens[0];
			const std::string& description = tokens[1];
			double cf = 0.0;
			try { cf = std::stod(tokens[2]); }
			catch (...) { throw std::runtime_error(std::format("facts line {}: invalid cf '{}'", idx, tokens[2])); } // Некорректный коэффициент уверенности

			validate_id(id); // Проверка ID
			validate_cf(cf); // Проверка коэффициента уверенности
			kb.add_subject_fact(fact::create(id, description, cf)); // Расширение предметной области
		}
	}

	// Разбор файла аксиом пользователя
	void parser::parse_axioms(const std::string& file_path, production_model& kb) {
		std::ifstream file(file_path);
		if (!file.is_open())
			throw std::runtime_error(std::format("The file '{}' can't be opened", file_path)); // Файл не открылся

		std::size_t idx = 0; // Индекс текущей строки
		std::string line; // Текущая строка
		while (std::getline(file, line)) {
			++idx;
			line = trim(line);

			if (line.empty() || line[0] == '#') continue; // Пустая строка или комментарий

			auto tokens = split(line, ';'); // Разбиение на токены по разделителю
			if (tokens.size() != 3)
				throw std::runtime_error(std::format("axioms line {}: expected 3 fields, got {}", idx, tokens.size())); // Неверное число полей

			const std::string& id = tokens[0];
			const std::string& description = tokens[1];
			double cf = 0.0;
			try { cf = std::stod(tokens[2]); }
			catch (...) { throw std::runtime_error(std::format("axioms line {}: invalid cf '{}'", idx, tokens[2])); } // Некорректный коэффициент уверенности

			validate_id(id); // Проверка ID
			validate_cf(cf); // Проверка коэффициента уверенности
			kb.add_axiom(fact::create(id, description, cf));
		}
	}

	// Разбор файла правил
	void parser::parse_rules(const std::string& file_path, production_model& kb) {
		std::ifstream file(file_path);
		if (!file.is_open())
			throw std::runtime_error(std::format("The file '{}' can't be opened", file_path)); // Файл не открылся

		std::size_t idx = 0; // Индекс текущей строки
		std::string line; // Текущая строка
		while (std::getline(file, line)) {
			++idx;
			line = trim(line);

			if (line.empty() || line[0] == '#') continue; // Пустая строка или комментарий

			auto tokens = split(line, ';'); // Разбиение на токены по разделителю
			if (tokens.size() != 3 && tokens.size() != 4)
				throw std::runtime_error(std::format("rules line {}: expected 3 or 4 fields, got {}", idx, tokens.size())); // Неверное число полей

			const std::string& id = tokens[0];
			const std::string& body = tokens[1];
			double cf = 0.0;
			try { cf = std::stod(tokens[2]); }
			catch (...) { throw std::runtime_error(std::format("rules line {}: invalid cf '{}'", idx, tokens[2])); } // Некорректный коэффициент уверенности

			std::string description = (tokens.size() == 4) ? tokens[3] : ""; // Описание правила (опционально)

			auto arrow = body.find("->"); // Поиск разделителя входов и выхода
			if (arrow == std::string::npos)
				throw std::runtime_error(std::format("rules line {}: no '->' delimiter", idx)); // Разделитель не найден

			std::string input = trim(body.substr(0, arrow)); // Входные факты
			std::string output = trim(body.substr(arrow + 2)); // Выходной факт
			if (input.empty()) throw std::runtime_error(std::format("rules line {}: empty input", idx)); // Пустые входные факты
			if (output.empty()) throw std::runtime_error(std::format("rules line {}: empty output", idx)); // Пустой выходной факт

			auto packages = split(input, ','); // Разбиение входных фактов по запятой
			for (const auto& p : packages) validate_id(p); // Проверка ID каждого входного факта
			validate_id(id); // Проверка ID правила
			validate_id(output); // Проверка ID выходного факта
			validate_cf(cf); // Проверка коэффициента уверенности

			kb.add_rule(rule::create(id, packages, output, cf, description));
		}
	}

	// Обрезание пробельных символов по краям
	std::string parser::trim(const std::string& line) {
		constexpr const char* ws = " \t\n\r\f\v";
		auto begin = line.find_first_not_of(ws);
		if (begin == std::string::npos) return "";
		auto end = line.find_last_not_of(ws);
		return line.substr(begin, end - begin + 1);
	}

	// Проверка на наличие пробельных символов внутри
	bool parser::has_inner_space(const std::string& line) {
		for (unsigned char symbol : line)
			if (std::isspace(symbol)) return true;
		return false;
	}

	// Проверка ID
	void parser::validate_id(const std::string& id) {
		if (id.empty())
			throw std::invalid_argument(std::format("The argument 'id' ('{}') can't be empty", id)); // ID не может быть пустым
		if (has_inner_space(id))
			throw std::invalid_argument(std::format("The argument 'id' ('{}') can't have an inner space symbol", id)); // ID не может содержать пробелы внутри
	}

	// Проверка коэффициента уверенности
	void parser::validate_cf(double confidence_coefficient) {
		if (!std::isfinite(confidence_coefficient))
			throw std::invalid_argument(std::format("The argument 'confidence_coefficient' ({}) must be finite", confidence_coefficient)); // Коэффициент уверенности должен быть конечным
		if (confidence_coefficient < MIN_CONFIDENCE_COEFFICIENT_VALUE ||
			confidence_coefficient > MAX_CONFIDENCE_COEFFICIENT_VALUE)
			throw std::invalid_argument(std::format("The argument 'confidence_coefficient' ({}) is out of range [-1; +1]", confidence_coefficient)); // Коэффициент уверенности вне диапазона
	}

	// Разбиение строки по разделителю
	std::vector<std::string> parser::split(const std::string& line, char delimeter) {
		std::vector<std::string> result;
		std::stringstream ss(line);
		std::string item;
		while (std::getline(ss, item, delimeter))
			result.push_back(trim(item)); // Разбиение и обрезание каждого токена

		return result;
	}

	bool forward_chaining(production_model& pm, const std::string& target_fact_id) {
		size_t step(0);
		bool changed(1);
		std::unordered_set<std::string> used_rules;

		while (changed) {
			changed = false;
			++step;

			for (const auto& [rule_id, rule] : pm.get_rules()) {
				if (used_rules.count(rule_id)) continue; // Правило уже было использовано ранее - нет смысла его снова применять

				auto input_facts = rule.get_input_facts();

				// Проверка, что все посылки уже доступны
				bool enough(1);
				for (const std::string& fact_id : input_facts) {
					if (!pm.contains_working_fact(fact_id)) {
						enough = false;
						break;
					}
				}
				if (!enough) continue;

				// Применение правила
				used_rules.insert(rule_id);

				auto output_fact = pm.get_subject_fact(rule.get_output_fact());
				pm.add_working_fact(output_fact);
				if (output_fact.get_id() == target_fact_id) return true;
			}
		}

		return false;
	}

	class node {
	public:
		enum class status_type { FALSE, UNKNOWN, TRUE }; // Статус узла: опровергнут / не определён / доказан
		enum class kind_type { OR, AND }; // Тип узла: OR — факт (ИЛИ), AND — правило (И)

	private:
		kind_type __kind; // Тип узла
		status_type __status; // Статус узла
		std::string __id; // ID узла (Fact для OR, Rule для AND)

		long long __heuristic; // Эвристическая оценка
		std::unordered_set<std::string> __path; // ID фактов на пути от корня (защита от циклов)

		node* __parent = NULL; // Родитель узла
		std::vector<std::shared_ptr<node>> __children; // Дети узла
	public:

		// Конструктор узла
		node(kind_type type, const std::string& id, const std::unordered_set<std::string>& path)
			: __kind(type), __id(id), __path(path), __heuristic(std::numeric_limits<long long>::max()), __status(status_type::UNKNOWN) {
		}

		// Проверка, является ли узел листом
		bool leaf() const {
			return this->__children.empty();
		}

		// Получение типа узла
		kind_type get_kind() const noexcept {
			return this->__kind;
		}
		// Получение статуса узла
		status_type get_status() const noexcept {
			return this->__status;
		}
		// Получение ID узла
		const std::string& get_id() const noexcept {
			return this->__id;
		}
		// Получение эвристической оценки узла
		long long get_heuristic() const noexcept {
			return this->__heuristic;
		}
		// Получение пути от корня до текущего узла
		const std::unordered_set<std::string>& get_path() const noexcept {
			return this->__path;
		}
		// Получение родителя узла
		node* get_parent() const noexcept {
			return this->__parent;
		}
		// Получение детей узла
		const std::vector<std::shared_ptr<node>>& get_children() const noexcept {
			return this->__children;
		}
		// Установка статуса узла
		void set_status(status_type status) noexcept {
			this->__status = status;
		}
		// Добавление ребёнка
		void add_child(std::shared_ptr<node> child) {
			child->__parent = this;
			this->__children.push_back(std::move(child));
		}
		// Установка эвристической оценки
		void set_heuristic(long long heuristic) noexcept {
			this->__heuristic = heuristic;
		}
	};

	// Подсчёт эвристической оценки AND-узла (число недоказанных посылок)
	long long compute_and_heuristic(const production_model& pm,
		const std::unordered_map<std::string, node::status_type>& working_fact_status,
		const std::shared_ptr<node>& and_node) {
		auto rule_it = pm.get_rules().find(and_node->get_id()); // Итератор на узел правила
		if (rule_it == pm.get_rules().end()) return std::numeric_limits<long long>::max();

		long long result(0); // Число недоказанных посылок
		for (const auto& input_fact_id : rule_it->second.get_input_facts()) {
			auto it = working_fact_status.find(input_fact_id);
			if (it == working_fact_status.end() || it->second != node::status_type::TRUE) ++result;
		}
		return result; // Чем меньше недоказанных посылок, тем ближе правило к срабатыванию
	}

	// Эвристика эвристической оценки OR-узла (минимум по детям-правилам)
	long long compute_or_heuristic(const std::shared_ptr<node>& or_node) {
		if (or_node->get_status() == node::status_type::TRUE) return 0; // Уже доказан
		if (or_node->get_children().empty()) return std::numeric_limits<long long>::max(); // Нечем доказать

		long long result = std::numeric_limits<long long>::max();
		for (const auto& c : or_node->get_children()) result = std::min(result, c->get_heuristic()); // Минимальная стоимость среди детей

		return result;
	}

	// Создание OR-узла (факта)
	static std::shared_ptr<node> create_fact_node(const std::string& fact_id,
		const std::unordered_set<std::string>& path,
		const std::unordered_map<std::string, node::status_type>& working_fact_status) {
		auto n = std::make_shared<node>(node::kind_type::OR, fact_id, path);
		auto it = working_fact_status.find(fact_id);

		if (it != working_fact_status.end() && it->second == node::status_type::TRUE)
			n->set_status(node::status_type::TRUE); // Аксиома сразу TRUE

		return n;
	}

	// Раскрытие OR-узла: факт -> правила -> факты
	std::vector<std::shared_ptr<node>> expand(const production_model& pm,
		const std::unordered_map<std::string, std::vector<std::string>>& rules_by_result,
		const std::unordered_map<std::string, node::status_type>& working_fact_status,
		const std::shared_ptr<node>& n) {

		std::vector<std::shared_ptr<node>> result; // Созданные AND-узлы (правила)
		if (n->get_path().count(n->get_id())) return result; // // Факт уже на пути от корня — цикл, не раскрываем

		auto it = rules_by_result.find(n->get_id()); // Ищем правила, выводящие этот факт
		if (it == rules_by_result.end()) return result; // Правил нет — факт нечем доказать

		std::unordered_set<std::string> new_path = n->get_path(); // Копия пути от корня
		new_path.insert(n->get_id()); // Добавляем текущий факт в путь

		for (const auto& rule_id : it->second) { // Все правила, выводящие факт
			const auto& rule = pm.get_rule(rule_id);
			auto and_node = std::make_shared<node>(node::kind_type::AND, rule_id, new_path); // Создаём AND-узел (правило)

			for (const auto& input_fact_id : rule.get_input_facts()) // Дети AND-узла — посылки правила
				and_node->add_child(create_fact_node(input_fact_id, new_path, working_fact_status)); // // Создаём OR-узел для каждой посылки

			and_node->set_heuristic(compute_and_heuristic(pm, working_fact_status, and_node));
			result.push_back(std::move(and_node));
		}

		return result;
	}

	// Пересчёт эвристических оценок и статуса вверх по родителям
	void recompute_upwards(const production_model& pm,
		std::unordered_map<std::string, node::status_type>& working_fact_status,
		std::unordered_map<std::string, node::status_type>& rule_status,
		node* start) {
		node* current_node = start;
		while (current_node) { // Идём вверх по родителям до корня
			// Пересчёт эвристики
			if (current_node->get_kind() == node::kind_type::AND) { // AND: число недоказанных посылок
				long long undone(0);
				auto rule_it = pm.get_rules().find(current_node->get_id());
				if (rule_it != pm.get_rules().end())
					for (const auto& premise_id : rule_it->second.get_input_facts()) {
						auto it = working_fact_status.find(premise_id);
						if (it == working_fact_status.end() || it->second != node::status_type::TRUE) ++undone;
					}
				current_node->set_heuristic(undone);
			}
			else if (!current_node->get_children().empty()) { // OR: min по детям
				long long best = std::numeric_limits<long long>::max();
				for (const auto& c : current_node->get_children()) best = std::min(best, c->get_heuristic());
				current_node->set_heuristic(best);
			}

			// Пересчёт статуса
			if (!current_node->get_children().empty()) {
				int count_true(0), count_false(0), count_unknown(0); // Счётчики по статусам детей
				for (const auto& c : current_node->get_children()) { // Обход всех детей
					switch (c->get_status()) {
					case node::status_type::TRUE: ++count_true; break;
					case node::status_type::FALSE: ++count_false; break;
					case node::status_type::UNKNOWN: ++count_unknown; break;
					}
				}
				int total = (int)current_node->get_children().size(); // Общее число детей
				node::status_type next = current_node->get_status(); // Новый статус - по умолчанию, не меняется
				if (current_node->get_kind() == node::kind_type::OR) {
					if (count_true > 0) next = node::status_type::TRUE; // Хотя бы один TRUE
					else if (count_false == total) next = node::status_type::FALSE; // Все FALSE
				}
				else {
					if (count_true == total) next = node::status_type::TRUE; // Все TRUE
					else if (count_false > 0) next = node::status_type::FALSE; // Хотя бы один FALSE
				}
				current_node->set_status(next);
				if (current_node->get_kind() == node::kind_type::OR) working_fact_status[current_node->get_id()] = next;
				else rule_status[current_node->get_id()] = next;
			}
			current_node = current_node->get_parent();
		}
	}
	bool backward_chaining(production_model& pm,
		const std::string& target_fact_id) {
		std::unordered_map<std::string, std::vector<std::string>> rules_by_result;
		for (const auto& [rule_id, rule] : pm.get_rules())
			rules_by_result[rule.get_output_fact()].push_back(rule_id);

		std::unordered_map<std::string, node::status_type> working_fact_status; // Статусы фактов (глобальные)
		std::unordered_map<std::string, node::status_type> rule_status; // Статусы правил (глобальные)
		for (const auto& [id, f] : pm.get_subject_facts()) working_fact_status[id] = node::status_type::UNKNOWN; // Все факты — UNKNOWN
		for (const auto& [id, r] : pm.get_rules()) rule_status[id] = node::status_type::UNKNOWN; // Все правила — UNKNOWN
		for (const auto& [id, f] : pm.get_axioms()) working_fact_status[id] = node::status_type::TRUE; // Аксиомы — TRUE

		// Компаратор очереди с приоритетом: OR выше AND, AND — дорогие первыми, OR — дешёвые первыми
		auto cmp = [](const std::shared_ptr<node>& a, const std::shared_ptr<node>& b) {
			// OR-узлы (факты) всегда выше AND-узлов (правил) — сначала раскрываем факты
			if (a->get_kind() != b->get_kind()) {
				return a->get_kind() == node::kind_type::AND; // AND ниже OR
			}
			if (a->get_kind() == node::kind_type::AND) {
				return a->get_heuristic() < b->get_heuristic(); // AND: большая эвристика — выше приоритет (max-heap)
			}
			return a->get_heuristic() > b->get_heuristic(); // OR: меньшая эвристика — выше приоритет (min-heap)
			};

		auto root = create_fact_node(target_fact_id, {}, working_fact_status); // Корень — OR-узел цели
		std::priority_queue<std::shared_ptr<node>, std::vector<std::shared_ptr<node>>, decltype(cmp)> pq(cmp); // Очередь с приоритетом по эвристике
		pq.push(root);

		while (!pq.empty()) {
			auto n = pq.top(); pq.pop();

			if (n->get_kind() == node::kind_type::OR) { // Текущий узел описывает факт

				// Узел уже доказан — пересчёт вверх
				if (n->get_status() == node::status_type::TRUE && n->leaf()) {
					recompute_upwards(pm, working_fact_status, rule_status, n.get());
					if (root->get_status() == node::status_type::TRUE) break; // Цель доказана
					continue;
				}

				// Раскрытие узла
				auto children = expand(pm, rules_by_result, working_fact_status, n);
				if (children.empty()) { // Нет детей — провал
					n->set_status(node::status_type::FALSE);
					working_fact_status[n->get_id()] = node::status_type::FALSE;

					recompute_upwards(pm, working_fact_status, rule_status, n.get());

					if (root->get_status() == node::status_type::FALSE) break; // Цель недоказуема
					continue;
				}

				// Добавление детей (сначала все, потом пересчёт)
				for (const auto& child : children) {
					n->add_child(child);
					pq.push(child); // AND-узел
					for (const auto& premise : child->get_children())
						pq.push(premise); // OR-посылки
				}

				for (const auto& child : children)
					recompute_upwards(pm, working_fact_status, rule_status, child.get());
			}
			else // Текущий узел описывает правило
				recompute_upwards(pm, working_fact_status, rule_status, n.get()); // Дети уже созданы и положены в очередь - просто пересчитываем вверх
		}

		return root->get_status() == node::status_type::TRUE;
	}
}