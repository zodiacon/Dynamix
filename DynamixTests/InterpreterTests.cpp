#include <catch.hpp>
#include <Parser.h>
#include <Tokenizer.h>
#include <AstNode.h>
#include <Interpreter.h>
#include <Value.h>

using namespace Dynamix;

TEST_CASE("Interpreter evaluates literals and arithmetic expressions") {
    Tokenizer tokenizer;
    Parser parser(tokenizer, true);
    Interpreter interpreter(parser);

    SECTION("Integer literal") {
        auto stmts = parser.Parse("1;", 1);
        REQUIRE(stmts != nullptr);
        auto& stmtsVec = stmts->Get();
        REQUIRE(stmtsVec.size() == 1);
        auto* exprStmt = dynamic_cast<ExpressionStatement*>(stmtsVec[0].get());
        REQUIRE(exprStmt != nullptr);
        auto val = exprStmt->Expr()->Accept(&interpreter);
        REQUIRE(val.IsInteger());
        REQUIRE(val.ToInteger() == 1);
    }

    SECTION("Simple addition") {
        auto stmts = parser.Parse("2 + 3;", 1);
        REQUIRE(stmts != nullptr);
        auto& stmtsVec = stmts->Get();
        REQUIRE(stmtsVec.size() == 1);
        auto* exprStmt = dynamic_cast<ExpressionStatement*>(stmtsVec[0].get());
        REQUIRE(exprStmt != nullptr);
        auto val = exprStmt->Expr()->Accept(&interpreter);
        REQUIRE(val.IsInteger());
        REQUIRE(val.ToInteger() == 5);
    }

    SECTION("Parenthesized expression") {
        auto stmts = parser.Parse("(2 + 3) * 4;", 1);
        REQUIRE(stmts != nullptr);
        auto& stmtsVec = stmts->Get();
        REQUIRE(stmtsVec.size() == 1);
        auto* exprStmt = dynamic_cast<ExpressionStatement*>(stmtsVec[0].get());
        REQUIRE(exprStmt != nullptr);
        auto val = exprStmt->Expr()->Accept(&interpreter);
        REQUIRE(val.IsInteger());
        REQUIRE(val.ToInteger() == 20);
    }
}

TEST_CASE("Interpreter evaluates variable declaration and assignment") {
    Tokenizer tokenizer;
    Parser parser(tokenizer, true);
    Interpreter interpreter(parser);

    SECTION("Variable declaration and use") {
        auto stmts = parser.Parse("var x = 10; x + 5;", 1);
        REQUIRE(stmts != nullptr);
        auto& stmtsVec = stmts->Get();
        REQUIRE(stmtsVec.size() == 2);
        // Evaluate both statements
        for (auto& stmt : stmtsVec) {
            stmt->Accept(&interpreter);
        }
        // The result of the last statement should be 15
        auto* exprStmt = dynamic_cast<ExpressionStatement*>(stmtsVec[1].get());
        REQUIRE(exprStmt != nullptr);
        auto val = exprStmt->Expr()->Accept(&interpreter);
        REQUIRE(val.IsInteger());
        REQUIRE(val.ToInteger() == 15);
    }
}

TEST_CASE("Interpreter evaluates a recursive function (factorial)") {
    Tokenizer tokenizer;
    Parser parser(tokenizer, true);
    Interpreter interpreter(parser);

    const char* code = R"(
        fn fact(n) {
            if (n == 0) {
                return 1;
            }
            return n * fact(n - 1);
        }
        fact(5);
    )";

    auto stmts = parser.Parse(code, 1);
    REQUIRE(stmts != nullptr);
    auto& stmtsVec = stmts->Get();
    REQUIRE(stmtsVec.size() == 2);

    // Register the function in the interpreter (simulate global scope)
    stmtsVec[0]->Accept(&interpreter);
    // Evaluate the function call
    auto* exprStmt = dynamic_cast<ExpressionStatement*>(stmtsVec[1].get());
    REQUIRE(exprStmt != nullptr);
    auto val = exprStmt->Expr()->Accept(&interpreter);
    REQUIRE(val.IsInteger());
    REQUIRE(val.ToInteger() == 120);
}