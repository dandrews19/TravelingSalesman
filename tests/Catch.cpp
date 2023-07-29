#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "catch_reporter_github.hpp"
#include "md5.h"
#include <fstream>
#include <string>
#include <cstdlib>

bool CheckFileMD5(const std::string& fileName, const std::string& expected)
{
	MD5 md5;
	std::string hash = md5.digestFile(fileName.c_str());
	return hash == expected;
}

bool CheckTextFilesSame(const std::string& fileNameA, 
	const std::string& fileNameB)
{
	// This is super unfortunate that I'm using std::system, but...
	std::string command;
#ifdef _MSC_VER
	command = "FC /W /LB5 " + fileNameA + " " + fileNameB;
#elif __APPLE__
	command = "diff -sBbu " + fileNameA + " " + fileNameB + " > diff.txt";
#else
	command = "diff -sBbu " + fileNameA + " " + fileNameB + " > diff.txt";
#endif
	int retVal = std::system(command.c_str());
#if !defined(_MSC_VER)
   // Add up to 50 lines of diff output, so we know the difference
	int result = std::system("head -50 diff.txt");
	if (result != 0)
	{
		FAIL("CheckTextFilesSame: error with diff command");
	}
#endif
	return retVal == 0;
}
