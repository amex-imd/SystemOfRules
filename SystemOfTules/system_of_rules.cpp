#include <cctype>
#include <string>
#include <stdexcept>
#include <format>
#include <sstream>
#include <fstream>

#include "system_of_rules.h"


namespace IMD {
	// -------- FACT IMPLEMENTATION --------

	fact::fact(const std::string& id, const std::string& description, double confidence_coefficient) :
	__id(id),
	__description(description),
	__confidence_coefficient(confidence_coefficient) {
	}

	const std::string& fact::get_id() const noexcept {
		return this->__id;
	}
	const std::string& fact::get_description() const noexcept {
		return this->__description;
	}
	double fact::get_confidence_coefficient() const noexcept {
		return this->__confidence_coefficient;
	}

	fact fact::create(const std::string& id, const std::string& description, double confidence_coefficient) {
		return fact(id, description, confidence_coefficient);
	}

	// -------- RULE IMPLEMENTATION --------

	rule::rule(const std::string& id, const std::vector<std::string>& input_facts, const std::string& output_fact) :
		__id(id),
		__input_facts(input_facts),
		__output_fact(output_fact) {
	}

	rule rule::create(const std::string& id, const std::vector<std::string>& input_facts, const std::string& output_facts) {
		return rule(id, input_facts, output_facts);
	}


	const std::string& rule::get_id() const noexcept {
		return this->__id;
	}
	const std::vector<std::string>& rule::get_input_facts() const noexcept {
		return this->__input_facts;
	}
	const std::string& rule::get_output_fact() const noexcept {
		return this->__output_fact;
	}

	// -------- KNOWLEDGE BASE IMPLEMENTATION --------

	void knowledge_base::add_initial_fact(const fact& fact) {
		auto [it, inserted] = this->__initial_facts.emplace(fact.get_id(), fact);
		if (!inserted) throw std::invalid_argument("TODO");
	}
	void knowledge_base::add_fact(const fact& fact) {
		auto [it, inserted] = this->__working_memory.emplace(fact.get_id(), fact);
		if (!inserted) throw std::invalid_argument("TODO");
	}
	void knowledge_base::add_rule(const rule& rule) {
		auto [it, inserted] = this->__rules.emplace(rule.get_id(), rule);
		if (!inserted) throw std::invalid_argument("TODO");
	}

	fact knowledge_base::get_initial_fact(const std::string& id) {
		auto it = this->__initial_facts.find(id);
		if (it == this->__initial_facts.end()) throw std::invalid_argument("TODO");
		return it->second;
	}

	fact knowledge_base::get_fact(const std::string& id) {
		auto it = this->__working_memory.find(id);
		if (it == this->__working_memory.end()) throw std::invalid_argument("TODO");
		return it->second;
	}
	rule knowledge_base::get_rule(const std::string& id) {
		auto it = this->__rules.find(id);
		if (it == this->__rules.end()) throw std::invalid_argument("TODO");
		return it->second;
	}

	// -------- PARSER IMPLEMENTATION --------

	void parser::parse_facts(const std::string& file_path, knowledge_base& kb) {
		std::ifstream file(file_path);

		if (!file.is_open())
			throw std::runtime_error(std::format("The file \'{}\' can't be opened", file_path));

		size_t idx(0); // Индекс текущей строки
		std::string line; // Текущая строка

		while (std::getline(file, line)) {
			++idx;
			line = trim(line);

			if (line.empty()) continue; // Натолкнулись на пустую строку

			std::vector<std::string> tokens = split(line, ';'); // Разбиение на токены по delimeter
			if (tokens.empty()) continue;
			if (tokens.size() != 3)
				throw std::runtime_error("Wrong interface"); // TODO

			const std::string& id = tokens[0];
			const std::string& description = tokens[1];
			double confidence_coefficient = std::stod(tokens[2]);
			fact f = fact::create(id, description, confidence_coefficient);
			kb.add_initial_fact(f);
			kb.add_fact(f);
		}
	}

	void parser::parse_rules(const std::string& file_path, knowledge_base& kb) {
		std::ifstream file(file_path);

		if (!file.is_open())
			throw std::runtime_error(std::format("The file \'{}\' can't be opened", file_path));

		size_t idx(0); // Индекс текущей строки
		std::string line; // Текущая строка

		while (std::getline(file, line)) {
			++idx;
			line = trim(line);

			if (line.empty()) continue; // Натолкнулись на пустую строку

			std::vector<std::string> tokens = split(line, ';'); // Разбиение на токены по delimeter
			if (tokens.empty()) continue;
			if (tokens.size() != 3)
				throw std::runtime_error("Wrong interface"); // TODO

			const std::string& id = tokens[0];
			const std::string& body = tokens[1];
			double confidence_coefficient = std::stod(tokens[2]);

			auto arrow_position = body.find("->");
			if (arrow_position == std::string::npos)
				throw std::runtime_error(std::format("The rule string with the index \'{}\' must have the delimeter '->'", idx));

			std::string input = trim(body.substr(0, arrow_position));
			std::string output = trim(body.substr(arrow_position + 2)); // Check out
			if (input.empty()) throw std::runtime_error("TODO");
			if (output.empty()) throw std::runtime_error("TODO");

			auto packages = split(input, ',');
			if (packages.empty()) throw std::runtime_error("TODO");
			for (const std::string& package_id : packages) validate_id(package_id);
			validate_id(output);

			rule rule = rule::create(id, packages, output);
			kb.add_rule(rule);

		}
	}
	std::string parser::trim(const std::string& line) {
		constexpr const char* ws = " \t\n\r\f\v";

		auto begin = line.find_first_not_of(ws);
		if (begin == std::string::npos) return "";

		auto end = line.find_last_not_of(ws);
		return line.substr(begin, end - begin + 1);

	}
	bool parser::has_inner_space(const std::string& line) {
		for (char symbol : line)
			if (std::isspace(symbol))
				return true;
		return false;
	}
	void parser::validate_id(const std::string& id) {
		if (id.empty())
			throw std::invalid_argument(std::format("The argument \'id\' (%d) can't be empty", id));
		if (has_inner_space(id))
			throw std::invalid_argument(std::format("The argument \'id\' (%d) can't have an inner space symbol", id));
	}
	void parser::validate_cf(double confidence_coefficient) {
		if (!std::isfinite(confidence_coefficient))
			throw std::invalid_argument(std::format("The argument \'confidence_coefficient\' (%d) must be finite (not NaN, Inf)", confidence_coefficient));

		if (confidence_coefficient < MIN_CONFIDENCE_COEFFICIENT_VALUE - EPSILON ||
			confidence_coefficient > MAX_CONFIDENCE_COEFFICIENT_VALUE + EPSILON)
			throw std::invalid_argument(std::format("The argument \'confidence_coefficient\' (%d) is out of range [-1; +1]", confidence_coefficient));
	}
	std::vector<std::string> parser::split(const std::string& input, char delimeter) {
		std::vector<std::string> result;
		std::stringstream ss(input);
		std::string item;

		while (std::getline(ss, item, delimeter)) result.push_back(trim(item));

		return result;
	}
}