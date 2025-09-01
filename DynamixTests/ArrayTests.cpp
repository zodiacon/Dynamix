#include <catch.hpp>
#include <ArrayType.h>
#include <Value.h>
#include <RuntimeObject.h>
#include <Runtime.h>

using namespace Dynamix;

TEST_CASE("ArrayObject basic construction and ToString", "[array]") {
    std::vector<Value> init = { Value(1), Value(2), Value(3) };
    ArrayObject arr(init);

    REQUIRE(arr.Items().size() == 3);
    REQUIRE(arr.ToString() == "[ 1, 2, 3 ]");
}

TEST_CASE("ArrayObject indexer access", "[array]") {
    std::vector<Value> init = { Value(10), Value(20), Value(30) };
    ArrayObject arr(init);

    SECTION("Valid index") {
        REQUIRE(arr.InvokeIndexer(Value(0)).ToInteger() == 10);
        REQUIRE(arr.InvokeIndexer(Value(2)).ToInteger() == 30);
    }

    SECTION("Invalid index throws") {
        REQUIRE_THROWS_AS(arr.InvokeIndexer(Value(-1)), RuntimeError);
        REQUIRE_THROWS_AS(arr.InvokeIndexer(Value(3)), RuntimeError);
    }
}

TEST_CASE("ArrayObject index assignment", "[array]") {
    std::vector<Value> init = { Value(5), Value(6), Value(7) };
    ArrayObject arr(init);

    arr.AssignIndexer(Value(1), Value(42), TokenType::Assign);
    REQUIRE(arr.Items()[1].ToInteger() == 42);

    REQUIRE_THROWS_AS(arr.AssignIndexer(Value(5), Value(99), TokenType::Assign), RuntimeError);
}

TEST_CASE("ArrayType::CreateArray", "[array]") {
    std::vector<Value> init = { Value(100), Value(200) };
    ArrayType& type = ArrayType::Get();
    ArrayObject* arr = type.CreateArray(init);

    REQUIRE(arr != nullptr);
    REQUIRE(arr->Items().size() == 2);
    REQUIRE(arr->Items()[0].ToInteger() == 100);
    REQUIRE(arr->Items()[1].ToInteger() == 200);

    arr->Release();
}