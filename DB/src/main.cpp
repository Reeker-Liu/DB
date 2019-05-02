#include "lexer.h"
#include "parse.h"

constexpr std::size_t MAXSIZE = 256;
char buffer[MAXSIZE];

void readFile()
{
	std::string filename = "G:\\Git\\DB\\DB\\src\\SQL.txt";

	std::ifstream inputFile( filename, std::ios::in );
	if (!inputFile.is_open())
	{
		std::cout << "failed to open: " << std::quoted(filename) << std::endl;
		return;
	}

	inputFile.getline(buffer, MAXSIZE - 1);
	inputFile.close();
}

SQLValue sql_parse(const std::string &sql)
{
	try
	{
		DB::lexer::Lexer lexer;
		std::cout << "\n--Start Tokenize---------------------------------------\n" << std::endl;
		lexer.tokenize(sql.c_str(), sql.size());
		lexer.print(std::cout);
		std::cout << "\n--End Tokenize---------------------------------------\n" << std::endl;
		std::cout << "\n--Start Parse---------------------------------------\n" << std::endl;
		DB::query::SQLValue value = analyze(lexer.getTokens()).sqlValue;
		std::cout << "\n--End Parse---------------------------------------\n" << std::endl;
		return value;
	}
	catch (const DB::DB_Base_Exception& e)
	{
		e.printException(); std::cout << std::endl;
		return ErrorMsg(e.str());	//change to details
	}
	catch (const std::exception& e) { std::cout << e.what() << std::endl << std::endl; }
	catch (...) { std::cout << "Unexpected Exception" << std::endl << std::endl; }
	return ErrorMsg("unexpected exception");
}

int main()
{
	readFile();
	sql_parse(std::string(buffer));

	return 0;
}