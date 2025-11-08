#include <catch.hpp>
#include <Parser.h>
#include <Tokenizer.h>
#include <AstNode.h>

using namespace Dynamix;

TEST_CASE("Parser parses a recursive function and checks AST") {
    Tokenizer tokenizer;
    Parser parser(tokenizer);

    // Simple recursive factorial function in the target language
    const char* code = R"(
        fn fact(n) {
            if (n == 0) {
                return 1;
            }
            return n * fact(n - 1);
        }
    )";

    auto stmts = parser.Parse(code, true);
    REQUIRE(stmts != nullptr);
    auto& stmtsVec = stmts->Get();
    REQUIRE(stmtsVec.size() == 1);

    auto* fnDecl = reinterpret_cast<FunctionDeclaration*>(stmtsVec[0].get());
    REQUIRE(fnDecl->NodeType() == AstNodeType::FunctionDeclaration);
    REQUIRE(fnDecl->Name() == "fact");
    REQUIRE(fnDecl->Parameters().size() == 1);
    REQUIRE(fnDecl->Parameters()[0].Name == "n");

    // Check function body is a block with two statements (if and return)
    auto body = fnDecl->Body();
    REQUIRE(body != nullptr);
    auto bodyStmts = reinterpret_cast<const Statements*>(body);
    REQUIRE(bodyStmts->NodeType() == AstNodeType::Statements);
    REQUIRE(bodyStmts->Count() == 2);

    // First statement: if (n == 0) { return 1; }
    {
        auto stmt = reinterpret_cast<const ExpressionStatement*>(bodyStmts->GetAt(0));
        auto ifExpr = reinterpret_cast<const IfThenElseExpression*>(stmt->Expr());
        REQUIRE(ifExpr->NodeType() == AstNodeType::IfThenElse);

        // Condition: n == 0
        auto cond = reinterpret_cast<const BinaryExpression*>(ifExpr->Condition());
        REQUIRE(cond != nullptr);
        auto left = reinterpret_cast<const NameExpression*>(cond->Left());
        auto right = reinterpret_cast<const LiteralExpression*>(cond->Right());
        REQUIRE(left != nullptr);
        REQUIRE(right != nullptr);
        REQUIRE(left->Name() == "n");
        REQUIRE(right->Literal().ToInteger() == 0);

        // Then branch: return 1;
        auto thenBlock = reinterpret_cast<const Statement*>(ifExpr->Then());
        REQUIRE(thenBlock != nullptr);
        auto thenReturn = reinterpret_cast<const ReturnStatement*>(thenBlock);
        REQUIRE(thenReturn != nullptr);
        auto thenLiteral = reinterpret_cast<const LiteralExpression*>(thenReturn->ReturnValue());
        REQUIRE(thenLiteral != nullptr);
        REQUIRE(thenLiteral->Literal().ToInteger() == 1);
    }

    // Second statement: return n * fact(n - 1);
    {
        auto retStmt = reinterpret_cast<const ReturnStatement*>(bodyStmts->Get()[1].get());
        REQUIRE(retStmt != nullptr);

        auto* binExpr = reinterpret_cast<const BinaryExpression*>(retStmt->ReturnValue());
        REQUIRE(binExpr != nullptr);
        REQUIRE(binExpr->Operator() == TokenType::Mul);

        // Left: n
        auto* left = reinterpret_cast<const NameExpression*>(binExpr->Left());
        REQUIRE(left != nullptr);
        REQUIRE(left->Name() == "n");

        // Right: fact(n - 1)
        auto* callExpr = reinterpret_cast<const InvokeFunctionExpression*>(binExpr->Right());
        REQUIRE(callExpr != nullptr);
        REQUIRE(callExpr->Arguments().size() == 1);

        auto* argExpr = reinterpret_cast<const BinaryExpression*>(callExpr->Arguments()[0].get());
        REQUIRE(argExpr != nullptr);
        REQUIRE(argExpr->Operator() == TokenType::Minus);

        auto* argLeft = reinterpret_cast<const NameExpression*>(argExpr->Left());
        auto* argRight = reinterpret_cast<const LiteralExpression*>(argExpr->Right());
        REQUIRE(argLeft != nullptr);
        REQUIRE(argRight != nullptr);
        REQUIRE(argLeft->Name() == "n");
        REQUIRE(argRight->Literal().ToInteger() == 1);
    }
}