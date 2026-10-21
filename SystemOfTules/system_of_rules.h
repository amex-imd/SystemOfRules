#ifndef __SYSTEM_OF_RULES_IMD_
#define __SYSTEM_OF_RULES_IMD_

#include <string>
#include <cstddef>
#include <vector>
#include <unordered_map>

#include "constants.h"

namespace IMD {
	constexpr double MIN_CONFIDENCE_COEFFICIENT_VALUE = -1.0;
	constexpr double MAX_CONFIDENCE_COEFFICIENT_VALUE = 1.0;

	// Класс, описывающий факт
	class fact {
	public:
		static fact create(const std::string& id, const std::string& description, double confidence_coefficient); // Создание факта

		const std::string& get_id() const noexcept;
		const std::string& get_description() const noexcept;
		double get_confidence_coefficient() const noexcept;
	private:
		std::string __id; // ID факта
		std::string __description; // Описание (содержимое) факта
		double __confidence_coefficient; // Коэффициент уверенности в факте

		fact(const std::string& id, const std::string& description, double confidence_coefficient);
	};

	class rule {
	public:
		static rule create(const std::string& id, const std::vector<std::string>& input, const std::string& output);


		const std::string& get_id() const noexcept;
		const std::vector<std::string>& get_input_facts() const noexcept;
		const std::string& get_output_fact() const noexcept;
	private:
		std::string __id;
		std::vector<std::string> __input_facts;
		std::string __output_fact;

		rule(const std::string& id, const std::vector<std::string>& input_facts, const std::string& output_fact);

	};

	class knowledge_base {
	public:
		void add_initial_fact(const fact& fact);
		void add_fact(const fact& fact);
		void add_rule(const rule& rule);

		fact get_initial_fact(const std::string& id);
		fact get_fact(const std::string& id);
		rule get_rule(const std::string& id);
		
		knowledge_base() = default;

	private:
		std::unordered_map<std::string, fact> __initial_facts; // Множество начальных факто
		std::unordered_map<std::string, rule> __rules; // Набор всех правил
		std::unordered_map<std::string, fact> __working_memory; // Рабочая память - множество всех выведенных фактов (является подмножеством __initial_facts)
	};

	class parser {
	public:
		static void parse_facts(const std::string& file_path, knowledge_base& kb);
		static void parse_rules(const std::string& file_path, knowledge_base& kb);

	private:
		static std::string trim(const std::string& line);
		static bool has_inner_space(const std::string& line);
		static void validate_id(const std::string& id);
		static void validate_cf(double confidence_coefficient);
		static std::vector<std::string> split(const std::string& line, char delimeter);
	};
}



#endif // !__SYSTEM_OF_RULES_CYAN_
