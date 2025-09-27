#include <catch.hpp>
#include <Parser.h>
#include <Tokenizer.h>
#include <Interpreter.h>
#include <Value.h>
#include <ArrayType.h>

using namespace Dynamix;

TEST_CASE("Memory") {
    Tokenizer tokenizer;
    Parser parser(tokenizer);
    Runtime rt(parser);
    Interpreter interpreter(parser, rt);

    SECTION("Lots of objects") {
        auto code = parser.Parse(R"(
        class Foo {
            var x = 1;
        }
        repeat 10000 {
            new Foo();
        }
        var x = new Foo();
        typeof(Foo).ObjectCount()
    )", true);

        REQUIRE(code != nullptr);
        CHECK(interpreter.Eval(code.get()).ToInteger() == 1);
    }

    SECTION("Simple Cycle") {
        auto code = parser.Parse(R"(
        class A {
            var b;
        }
        class B {
            var a;
        }
        fn Test() {
            var a = new A();
            var b = new B();
            a.b = b;
            b.a = a;
        }
        Test();
        [ typeof(A).ObjectCount(), typeof(B).ObjectCount() ]
    )", true);

        REQUIRE(code != nullptr);
        auto arr = interpreter.Eval(code.get()).ToObject();
        REQUIRE(arr->Type() == ArrayType::Get());
        auto result = reinterpret_cast<ArrayObject*>(arr);
        REQUIRE(result->Count() == 2);
        CHECK(result->Items()[0].ToInteger() == 1);
        CHECK(result->Items()[1].ToInteger() == 1);
    }
}
