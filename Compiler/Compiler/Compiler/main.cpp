#include "CompilerDriver.h"
#include "../utils/file_utils.h"

#include <iostream>
#include <memory>

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		std::cerr << "two arguments expected: compiler.exe <txt file with program>" << std::endl;
		return 1;
	}
	try
	{
		CompilerDriver driver(std::cout);
		driver.Compile(file_utils::GetFileContent(argv[1]));
	}
	catch (const std::exception& ex)
	{
		std::cerr << "Error: " << ex.what() << std::endl;
		return 1;
	}
}