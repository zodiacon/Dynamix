// DynamixTests.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <catch.hpp>

int main(int argc, char* argv[]) {
	int result = Catch::Session().run(argc, argv);

	return result;
}

TEST_CASE("Hello") {
	REQUIRE(true);
}
