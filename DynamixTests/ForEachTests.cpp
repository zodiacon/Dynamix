#include <catch.hpp>
#include <Parser.h>
#include <Tokenizer.h>
#include <AstNode.h>
#include <Interpreter.h>
#include <Value.h>
#include <ArrayType.h>

using namespace Dynamix;

TEST_CASE("Parser parses foreach statement", "[foreach]") {
    Tokenizer tokenizer;
    Parser parser(tokenizer);

    auto stmts = parser.Parse("foreach item in [1, 2, 3] { }", true);
    REQUIRE(stmts != nullptr);
    auto& stmtsVec = stmts->Get();
    REQUIRE(stmtsVec.size() == 1);

    auto* foreachStmt = dynamic_cast<ForEachStatement*>(stmtsVec[0].get());
    REQUIRE(foreachStmt != nullptr);
    REQUIRE(foreachStmt->Name() == "item");
    REQUIRE(foreachStmt->Collection() != nullptr);
    REQUIRE(foreachStmt->Body() != nullptr);
}

TEST_CASE("Interpreter executes foreach over array", "[foreach]") {
    Tokenizer tokenizer;
    Parser parser(tokenizer);
    Runtime rt;
    Interpreter interpreter(rt);

    // Sum all items in array using foreach
    const char* code = R"(
        var sum = 0;
        foreach (item in [1, 2, 3, 4]) {
            sum = sum + item;
        }
        sum;
    )";
    auto stmts = parser.Parse(code, true);
    REQUIRE(stmts != nullptr);
    auto& stmtsVec = stmts->Get();
    REQUIRE(stmtsVec.size() == 3);

    // Evaluate all statements
    for (auto& stmt : stmtsVec) {
        stmt->Accept(&interpreter);
    }
    // The result of the last statement should be 10
    auto* exprStmt = dynamic_cast<ExpressionStatement*>(stmtsVec[2].get());
    REQUIRE(exprStmt != nullptr);
    auto val = exprStmt->Expr()->Accept(&interpreter);
    REQUIRE(val.IsInteger());
    REQUIRE(val.ToInteger() == 10);
}

TEST_CASE("Interpreter throws on foreach with non-array", "[foreach]") {
    Tokenizer tokenizer;
    Parser parser(tokenizer);
    Runtime rt;
    Interpreter interpreter(rt);

    // Try to foreach over an integer (should throw)
    const char* code = R"(
        foreach (x in 42) {
            // Should not execute
        }
    )";
    auto stmts = parser.Parse(code, true);
    REQUIRE(stmts != nullptr);
    auto& stmtsVec = stmts->Get();
    REQUIRE(stmtsVec.size() == 1);

    auto* foreachStmt = dynamic_cast<ForEachStatement*>(stmtsVec[0].get());
    REQUIRE(foreachStmt != nullptr);

    REQUIRE_THROWS_AS(foreachStmt->Accept(&interpreter), RuntimeError);
}