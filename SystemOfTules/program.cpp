#include <iostream>

#include "system_of_rules.h"

int main(int argc, char* argv[]) {
	try {
		const char* facts_file_path = "C:\\Users\\Amethyst\\Desktop\\SystemOfTules\\SystemOfTules\\facts.txt";
		const char* rules_file_path = "C:\\Users\\Amethyst\\Desktop\\SystemOfTules\\SystemOfTules\\rules.txt";
		IMD::knowledge_base kb;

		IMD::parser::parse_facts(facts_file_path, kb);
	}
	catch (const std::exception& ex) {
		std::cout << ex.what() << "\n";
	}
	return 0;
}