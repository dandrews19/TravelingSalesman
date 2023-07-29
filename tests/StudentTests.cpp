#include "catch.hpp"
#include "SrcMain.h"
#include "TSP.h"
#include <string>

// Don't change this
#ifdef _MSC_VER
const std::string SamplePath("sampleoutput/win/");
#elif __APPLE__
const std::string SamplePath("sampleoutput/mac/");
#else
const std::string SamplePath("sampleoutput/linux/");
#endif
// Helper function declarations (don't change these)
extern bool CheckFileMD5(const std::string& fileName, const std::string& expected);
extern bool CheckTextFilesSame(const std::string& fileNameA, 
	const std::string& fileNameB);

// Provided Complete file test cases
// (Once you pass the first section, comment out the other)
TEST_CASE("Full file test cases", "[student]")
{
	SECTION("input/locations.txt 8 5 10 1337")
	{
		const char* argv[] = {
			"tests/tests",
			"input/locations.txt",
			"8",
			"5",
			"10",
			"1337"
		};
		ProcessCommandArgs(6, argv);
		bool result = CheckTextFilesSame("log.txt", SamplePath + "log_8_5_10_1337.txt");
		REQUIRE(result);
	}
	SECTION("input/locations.txt 32 20 5 5741328")
	{
		const char* argv[] = {
			"tests/tests",
			"input/locations.txt",
			"32",
			"20",
			"5",
			"5741328"
		};
		ProcessCommandArgs(6, argv);
		bool result = CheckTextFilesSame("log.txt", SamplePath + "log_32_20_5_5741328.txt");
		REQUIRE(result);
	}
	SECTION("input/locations.txt 64 500 30 12165465")
	{
		const char* argv[] = {
			"tests/tests",
			"input/locations.txt",
			"64",
			"500",
			"30",
			"12165465"
		};
		ProcessCommandArgs(6, argv);
		bool result = CheckTextFilesSame("log.txt", SamplePath + "log_64_500_30_12165465.txt");
		REQUIRE(result);
	}
	SECTION("input/locations.txt 128 200 10 331988960")
	{
		const char* argv[] = {
			"tests/tests",
			"input/locations.txt",
			"128",
			"200",
			"10",
			"331988960"
		};
		ProcessCommandArgs(6, argv);
		bool result = CheckTextFilesSame("log.txt", SamplePath + "log_128_200_10_331988960.txt");
		REQUIRE(result);
	}
	SECTION("input/locations.txt 256 200 20 7410785")
	{
		const char* argv[] = {
			"tests/tests",
			"input/locations.txt",
			"256",
			"200",
			"20",
			"7410785"
		};
		ProcessCommandArgs(6, argv);
		bool result = CheckTextFilesSame("log.txt", SamplePath + "log_256_200_20_7410785.txt");
		REQUIRE(result);
	}
	SECTION("input/locations2.txt 64 200 25 49328573")
	{
		const char* argv[] = {
			"tests/tests",
			"input/locations2.txt",
			"64",
			"200",
			"25",
			"49328573"
		};
		ProcessCommandArgs(6, argv);
		bool result = CheckTextFilesSame("log.txt", SamplePath + "log_64_200_25_49328573.txt");
		REQUIRE(result);
	}
}
