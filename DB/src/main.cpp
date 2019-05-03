#include <fstream>
#include <string>
#include <iostream>
#include "include/query.h"

constexpr std::size_t MAXSIZE = 256;
char buffer[MAXSIZE];

void readFile()
{
	std::string filename = "G:\\Git\\DB\\DB\\src\\SQL.txt";

	std::ifstream inputFile( filename, std::ios::in );
	if (!inputFile.is_open())
	{
		std::cout << "failed to open: " << filename << std::endl;
		return;
	}

	inputFile.getline(buffer, MAXSIZE - 1);
	inputFile.close();
}



int main()
{
	readFile();
	DB::query::SQLValue res = DB::query::sql_parse(std::string(buffer));
	DB::query::print(res);

	return 0;
}