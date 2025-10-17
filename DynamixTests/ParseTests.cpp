#include <catch.hpp>
#include <Parser.h>
#include <Value.h>
#include <RuntimeObject.h>
#include <Tokenizer.h>

using namespace Dynamix;

TEST_CASE("Parse members", "[parser]") {
	Tokenizer t;
	Parser parser(t);
	auto node = parser.Parse("a.b.c.d(4);", true);
	REQUIRE(node);
	node = parser.Parse("A::B::C::D(4);", true);
	REQUIRE(node);
}

