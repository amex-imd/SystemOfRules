#include <cctype>
#include <cmath>
#include <string>
#include <stdexcept>
#include <format>
#include <sstream>
#include <iostream>
#include <fstream>
#include <unordered_set>

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
}