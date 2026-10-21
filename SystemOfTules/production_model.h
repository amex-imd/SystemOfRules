#ifndef __production_model_IMD_
#define __production_model_IMD_

#include <string>
#include <cstddef>
#include <vector>
#include <unordered_map>

#include "constants.h"

namespace IMD {
	constexpr double MIN_CONFIDENCE_COEFFICIENT_VALUE = -1.0; // Минимальное значение коэффициента уверенности
	constexpr double MAX_CONFIDENCE_COEFFICIENT_VALUE = 1.0; // Максимальное значение коэффициента уверенности

	// Класс, описывающий факт
	class fact {
	public:
		// Создание факта
		static fact create(const std::string& id,
			const std::string& description,
			double confidence_coefficient);

		const std::string& get_id() const noexcept; // Получение ID факта
		const std::string& get_description() const noexcept; // Получение описание факта
		double get_confidence_coefficient() const noexcept; // Получение коэффициента уверенности в факте
	private:
		std::string __id; // ID факта
		std::string __description; // Описание факта
		double __confidence_coefficient; // Коэффициент уверенности в факте

		// Конструктор факта
		fact(const std::string& id,
			const std::string& description,
			double confidence_coefficient);
	};

	// Класс, описывающий правило
	class rule {
	public:
		// Создание правила
		static rule create(const std::string& id,
			const std::vector<std::string>& input,
			const std::string& output,
			double confidence_coefficient,
			const std::string& description = "");

		const std::string& get_id() const noexcept; // Получение ID правила
		const std::vector<std::string>& get_input_facts() const noexcept; // Получение входных фактов (посылок) правила
		const std::string& get_output_fact() const noexcept; // Получение выходного факта правила
		double get_confidence_coefficient() const noexcept; // Получение коэффициента уверенности
		const std::string& get_description() const noexcept; // Получение описание правила

	private:
		std::string __id; // ID правила
		std::vector<std::string> __input_facts; // Входные факты (посылки) правила
		std::string __output_fact; // Выходной факт правила
		double __confidence_coefficient; // Коэффициент уверенности правила
		std::string __description; // Описание правила

		// Конструктор правила
		rule(const std::string& id,
			const std::vector<std::string>& input_facts,
			const std::string& output_fact,
			double confidence_coefficient,
			const std::string& description);
	};

	// Класс, описывающий продукционную модель
	class production_model {
	public:
		void add_subject_fact(const fact& fact); // Добавление факта в предметную область
		void add_axiom(const fact& axiom); // Добавление аксиомы в начальные условий
		void add_rule(const rule& rule); // Добавление правила
		void add_working_fact(const fact& fact); // Добавление факта в рабочую память

		fact get_subject_fact(const std::string& id) const; // Получение факта из предметной области
		fact get_working_fact(const std::string& id) const; // Получение факта из рабочей памяти
		fact get_axiom(const std::string& id) const; // Получение аксиомы
		rule get_rule(const std::string& id) const; // Получение правила

		bool contains_working_fact(const std::string& id) const; // Проверка наличия факта в рабочей памяти
		bool contains_subject_fact(const std::string& id) const; // Проверка наличия факта в предметной области
		bool contains_axiom(const std::string& id) const; // Проверка наличия аксиомы

		const std::unordered_map<std::string, rule>& get_rules() const noexcept; // Получение всех правил
		const std::unordered_map<std::string, fact>& get_subject_facts() const noexcept; // Получение всех возможных фактов из предметной области
		const std::unordered_map<std::string, fact>& get_axioms() const noexcept; // Получение всех аксиом
		const std::unordered_map<std::string, fact>& get_working_memory() const noexcept; // Получение рабочей памяти

		production_model() = default; // Конструктор по умолчанию

	private:
		std::unordered_map<std::string, fact> __subject_domain; // Предметная область
		std::unordered_map<std::string, fact> __axioms; // Аксиомы (начальные факты пользователя)
		std::unordered_map<std::string, rule> __rules; // Набор правил
		std::unordered_map<std::string, fact> __working_memory; // Рабочая память (активные факты)
	};

	// Класс, описывающий парсер
	class parser {
	public:
		static void parse_subject_domain(const std::string& file_path, production_model& kb); // Разбор файла фактов модели
		static void parse_axioms(const std::string& file_path, production_model& kb); // Разбор файла аксиом пользователя
		static void parse_rules(const std::string& file_path, production_model& kb); // Разбор файла правил
	private:
		static std::string trim(const std::string& line); // Обрезание пробельных символов по краям
		static bool has_inner_space(const std::string& line); // Проверка на наличие пробельных символов внутри
		static void validate_id(const std::string& id); // Проверка ID
		static void validate_cf(double confidence_coefficient); // Проверка коэффициента уверенности
		static std::vector<std::string> split(const std::string& line, char delimeter); // Разбиение строки по разделителю
	};

	bool forward_chaining(production_model& pm, const std::string& target_fact_id);
	bool backward_chaining(production_model& pm, const std::string& target_fact_id);
}

#endif // !__production_model_IMD_