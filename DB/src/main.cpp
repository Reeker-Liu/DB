#include "lexer.h"

int main()
{
	std::string filename = "G:\\Git\\DB\\DB\\src\\SQL.txt";
	bool ok = false;
	DB::lexer::Lexer lexer;
	try { lexer.tokenize(filename); ok = true; }
	catch (const DB::DB_Base_Exception& e) { e.printException(); std::cout << std::endl; }
	catch (const std::exception& e) { std::cout << e.what() << std::endl << std::endl; }
	catch (...) { std::cout << "Unexpected Exception" << std::endl << std::endl; }
	if (!ok) return 0;

	lexer.print(std::cout);
	std::cout << "\n-----------------------------------------\n" << std::endl;

	return 0;
}