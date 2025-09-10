#include <catch.hpp>
#include <Parser.h>
#include <Tokenizer.h>
#include <AstNode.h>
#include <Interpreter.h>
#include <Value.h>
#include <Runtime.h>
#include <ObjectType.h>
#include <Runtime.h>

using namespace Dynamix;

TEST_CASE("Parser parses class with fields and methods", "[class]") {
    Tokenizer tokenizer;
    Parser parser(tokenizer);
    Runtime rt(parser);
    Interpreter interpreter(parser, rt);

    auto stmts = parser.Parse(R"(
        class Foo {
            var x = 1;
            fn getX() { return x; }
        }
    )", true);
    REQUIRE(stmts != nullptr);
    auto& stmtsVec = stmts->Get();
    REQUIRE(stmtsVec.size() == 1);

    auto* classDecl = dynamic_cast<ClassDeclaration*>(stmtsVec[0].get());
    REQUIRE(classDecl != nullptr);
    REQUIRE(classDecl->Name() == "Foo");
    // Check for field and method presence
    bool hasField = false, hasMethod = false;
    for (auto& member : classDecl->Fields()) {
        if (auto varStmt = dynamic_cast<VarValStatement*>(member.get()))
            if (varStmt->Name() == "x") hasField = true;
    }
    for(auto& member : classDecl->Methods()) {
        if (auto fnDecl = dynamic_cast<FunctionDeclaration*>(member.get()))
            if (fnDecl->Name() == "getX") hasMethod = true;
    }
    REQUIRE(hasField);
    REQUIRE(hasMethod);
}

TEST_CASE("Interpreter instantiates class and calls method", "[class]") {
    Tokenizer tokenizer;
    Parser parser(tokenizer);
    Runtime rt(parser);
    Interpreter interpreter(parser, rt);

    const char* code = R"(
        class Bar {
            var y = 7;
            fn getY() { return this.y; }
        }
        var obj = new Bar();
        obj.getY()
    )";
    auto stmts = parser.Parse(code, true);
    REQUIRE(stmts != nullptr);
    auto& stmtsVec = stmts->Get();
    REQUIRE(stmtsVec.size() == 3);

    auto val = interpreter.Eval(stmts.get());

    REQUIRE(val.IsInteger());
    REQUIRE(val.ToInteger() == 7);
}

TEST_CASE("Interpreter supports constructor and field initialization", "[class]") {
    Tokenizer tokenizer;
    Parser parser(tokenizer);
    Runtime rt(parser);
    Interpreter interpreter(parser, rt);

    const char* code = R"(
        class Baz {
            var z;
            new(v) { this.z = v; }
        }
        var obj = new Baz(99);
        obj.z;
    )";
    auto stmts = parser.Parse(code, true);
    REQUIRE(stmts != nullptr);
    auto& stmtsVec = stmts->Get();
    REQUIRE(stmtsVec.size() == 3);

    interpreter.Eval(stmtsVec[0].get());
    interpreter.Eval(stmtsVec[1].get());

    auto* exprStmt = dynamic_cast<ExpressionStatement*>(stmtsVec[2].get());
    REQUIRE(exprStmt != nullptr);
    auto val = interpreter.Eval(exprStmt->Expr());
    REQUIRE(val.IsInteger());
    REQUIRE(val.ToInteger() == 99);
}

TEST_CASE("Interpreter supports inheritance", "[class][inheritance]") {
    Tokenizer tokenizer;
    Parser parser(tokenizer);
    Runtime rt(parser);
    Interpreter interpreter(parser, rt);

    const char* code = R"(
        class Base {
            fn foo() { return 123; }
        }
        class Derived : Base {
        }
        var obj = new Derived();
        obj.foo()
    )";
    auto stmts = parser.Parse(code, true);
    REQUIRE(stmts != nullptr);
    auto& stmtsVec = stmts->Get();
    REQUIRE(stmtsVec.size() == 4);

    interpreter.Eval(stmtsVec[0].get()); // Base
    interpreter.Eval(stmtsVec[1].get()); // Derived
    interpreter.Eval(stmtsVec[2].get()); // obj

    auto* exprStmt = dynamic_cast<ExpressionStatement*>(stmtsVec[3].get());
    REQUIRE(exprStmt != nullptr);
    auto val = exprStmt->Expr()->Accept(&interpreter);
    REQUIRE(val.IsInteger());
    REQUIRE(val.ToInteger() == 123);
}