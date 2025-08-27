// DynamixTests.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <catch.hpp>
#include <Parser.h>
#include <Tokenizer.h>
#include <AstNode.h>
#include <ParseError.h>
#include <SymbolTable.h>

using namespace Dynamix;

TEST_CASE("Parser and Tokenizer integration with AST validation") {
    Tokenizer tokenizer;
    Parser parser(tokenizer, true);
    REQUIRE(parser.Init());

    SECTION("Parse simple var statement and check AST") {
        auto stmts = parser.Parse("var x = 1;", 1);
        REQUIRE(stmts != nullptr);
        auto& stmtsVec = stmts->Get();
        REQUIRE(stmtsVec.size() == 1);
        auto varStmt = dynamic_cast<VarValStatement*>(stmtsVec[0].get());
        REQUIRE(varStmt != nullptr);
        REQUIRE(varStmt->Name() == "x");
        REQUIRE_FALSE(varStmt->IsConst());
        auto literal = dynamic_cast<const LiteralExpression*>(varStmt->Init());
        REQUIRE(literal != nullptr);
        REQUIRE(literal->Literal().Type == TokenType::Integer);
        REQUIRE(literal->Literal().Lexeme == "1");
    }

    SECTION("Parse const val statement and check AST") {
        auto stmts = parser.Parse("val y = 42;", 1);
        REQUIRE(stmts != nullptr);
        auto& stmtsVec = stmts->Get();
        REQUIRE(stmtsVec.size() == 1);
        auto valStmt = dynamic_cast<VarValStatement*>(stmtsVec[0].get());
        REQUIRE(valStmt != nullptr);
        REQUIRE(valStmt->Name() == "y");
        REQUIRE(valStmt->IsConst());
        auto* literal = dynamic_cast<const LiteralExpression*>(valStmt->Init());
        REQUIRE(literal != nullptr);
        REQUIRE(literal->Literal().Type == TokenType::Integer);
        REQUIRE(literal->Literal().Lexeme == "42");
    }

    SECTION("Parse function declaration and check AST") {
        auto stmts = parser.Parse("fn foo(a, b) { return a; }", 1);
        REQUIRE(stmts != nullptr);
        auto& stmtsVec = stmts->Get();
        REQUIRE(stmtsVec.size() == 1);
        auto* fnDecl = dynamic_cast<FunctionDeclaration*>(stmtsVec[0].get());
        REQUIRE(fnDecl != nullptr);
        REQUIRE(fnDecl->Name() == "foo");
        REQUIRE(fnDecl->Parameters().size() == 2);
        REQUIRE(fnDecl->Parameters()[0].Name == "a");
        REQUIRE(fnDecl->Parameters()[1].Name == "b");
        auto* body = fnDecl->Body();
        REQUIRE(body != nullptr);
        auto* bodyStmts = dynamic_cast<const Statements*>(body);
        REQUIRE(bodyStmts != nullptr);
        REQUIRE(bodyStmts->Get().size() == 1);
        auto* retStmt = dynamic_cast<const ReturnStatement*>(bodyStmts->Get()[0].get());
        REQUIRE(retStmt != nullptr);
        auto* nameExpr = dynamic_cast<const NameExpression*>(retStmt->ReturnValue());
        REQUIRE(nameExpr != nullptr);
        REQUIRE(nameExpr->Name() == "a");
    }

    SECTION("Parse while statement and check AST") {
        auto stmts = parser.Parse("while (true) { var z = 5; }", 1);
        REQUIRE(stmts != nullptr);
        auto& stmtsVec = stmts->Get();
        REQUIRE(stmtsVec.size() == 1);
        auto* whileStmt = dynamic_cast<WhileStatement*>(stmtsVec[0].get());
        REQUIRE(whileStmt != nullptr);
        auto* cond = dynamic_cast<const LiteralExpression*>(whileStmt->Condition());
        REQUIRE(cond != nullptr);
        REQUIRE(cond->Literal().Type == TokenType::True);
        auto* body = whileStmt->Body();
        REQUIRE(body != nullptr);
        REQUIRE(body->Get().size() == 1);
        auto* varStmt = dynamic_cast<VarValStatement*>(body->Get()[0].get());
        REQUIRE(varStmt != nullptr);
        REQUIRE(varStmt->Name() == "z");
    }

    SECTION("Parse enum declaration and check AST") {
        auto stmts = parser.Parse("enum Color { Red, Green = 2, Blue }", 1);
        REQUIRE(stmts != nullptr);
        auto& stmtsVec = stmts->Get();
        REQUIRE(stmtsVec.size() == 1);
        auto enumDecl = dynamic_cast<EnumDeclaration*>(stmtsVec[0].get());
        REQUIRE(enumDecl != nullptr);
        REQUIRE(enumDecl->Name() == "Color");
        auto values = enumDecl->Values();
        REQUIRE(values.size() == 3);
        REQUIRE(values.at("Red") == 0);
        REQUIRE(values.at("Green") == 2);
        REQUIRE(values.at("Blue") == 3);
    }

    SECTION("Parse return statement and check AST") {
        auto stmts = parser.Parse("return 123;", 1);
        REQUIRE(stmts != nullptr);
        auto& stmtsVec = stmts->Get();
        REQUIRE(stmtsVec.size() == 1);
        auto* retStmt = dynamic_cast<ReturnStatement*>(stmtsVec[0].get());
        REQUIRE(retStmt != nullptr);
        auto* literal = dynamic_cast<const LiteralExpression*>(retStmt->ReturnValue());
        REQUIRE(literal != nullptr);
        REQUIRE(literal->Literal().Lexeme == "123");
    }

    SECTION("Parse binary expression and check AST") {
        auto stmts = parser.Parse("var sum = 1 + 2;", 1);
        REQUIRE(stmts != nullptr);
        auto& stmtsVec = stmts->Get();
        REQUIRE(stmtsVec.size() == 1);
        auto* varStmt = dynamic_cast<VarValStatement*>(stmtsVec[0].get());
        REQUIRE(varStmt != nullptr);
        auto* binExpr = dynamic_cast<const BinaryExpression*>(varStmt->Init());
        REQUIRE(binExpr != nullptr);
        auto* left = dynamic_cast<const LiteralExpression*>(binExpr->Left());
        auto* right = dynamic_cast<const LiteralExpression*>(binExpr->Right());
        REQUIRE(left != nullptr);
        REQUIRE(right != nullptr);
        REQUIRE(left->Literal().Lexeme == "1");
        REQUIRE(right->Literal().Lexeme == "2");
        REQUIRE(binExpr->Operator().Type == TokenType::Plus);
    }
}

int main(int argc, char* argv[]) {
	int result = Catch::Session().run(argc, argv);

	return result;
}
