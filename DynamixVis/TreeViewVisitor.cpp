#include "pch.h"
#include "TreeViewVisitor.h"
#include "Helpers.h"
#include <Value.h>
#include <AstNode.h>
#include <format>

using namespace Dynamix;

TreeViewVisitor::TreeViewVisitor(HWND hTreeView) : m_Tree(hTreeView) {
}

void TreeViewVisitor::Visit(AstNode* node, HTREEITEM hRoot) {
    m_hCurrent = hRoot;
    node->Accept(this);
}

Value TreeViewVisitor::VisitLiteral(LiteralExpression const* expr) {
    m_Tree.InsertItem(Helpers::AstNodeTypeToString(expr->NodeType()), m_hCurrent, TVI_LAST);
    return Value();
}

Value TreeViewVisitor::VisitBinary(BinaryExpression const* expr) {
    auto h = m_hCurrent;
    m_hCurrent = m_Tree.InsertItem(Helpers::AstNodeTypeToString(expr->NodeType()), m_hCurrent, TVI_LAST);
    expr->Left()->Accept(this);
    expr->Right()->Accept(this);
    m_hCurrent = h;

    return Value();
}

Value TreeViewVisitor::VisitUnary(UnaryExpression const* expr) {
    auto h = m_hCurrent;
    m_hCurrent = m_Tree.InsertItem(Helpers::AstNodeTypeToString(expr->NodeType()), m_hCurrent, TVI_LAST);
    expr->Arg()->Accept(this);
    m_hCurrent = h;
    return Value();
}

Value TreeViewVisitor::VisitName(NameExpression const* expr) {
    m_Tree.InsertItem(Helpers::AstNodeTypeToString(expr->NodeType()), m_hCurrent, TVI_LAST);
    return Value();
}

Value TreeViewVisitor::VisitVar(VarValStatement const* expr) {
    auto h = m_hCurrent;
    m_hCurrent = m_Tree.InsertItem(Helpers::AstNodeTypeToString(expr->NodeType()), m_hCurrent, TVI_LAST);
    m_Tree.InsertItem(CString(expr->Name().c_str()), m_hCurrent, TVI_LAST);
    if(expr->Init())
        expr->Init()->Accept(this);
    m_hCurrent = h;
    return Value();
}

Value TreeViewVisitor::VisitAssign(AssignExpression const* expr) {
    return Value();
}

Value TreeViewVisitor::VisitInvokeFunction(InvokeFunctionExpression const* expr) {
    return Value();
}

Value TreeViewVisitor::VisitRepeat(RepeatStatement const* expr) {
    return Value();
}

Value TreeViewVisitor::VisitWhile(WhileStatement const* stmt) {
    return Value();
}

Value TreeViewVisitor::VisitIfThenElse(IfThenElseExpression const* expr) {
    return Value();
}

Value TreeViewVisitor::VisitFunctionDeclaration(FunctionDeclaration const* decl) {
    auto h = m_hCurrent;
    m_hCurrent = m_Tree.InsertItem(Helpers::AstNodeTypeToString(decl->NodeType()), m_hCurrent, TVI_LAST);
    m_Tree.InsertItem(CString(decl->Name().c_str()), m_hCurrent, TVI_LAST);
    decl->Body()->Accept(this);
    m_hCurrent = h;
    return Value();
}

Value TreeViewVisitor::VisitReturn(ReturnStatement const* decl) {
    auto h = m_hCurrent;
    m_hCurrent = m_Tree.InsertItem(Helpers::AstNodeTypeToString(decl->NodeType()), m_hCurrent, TVI_LAST);
    decl->ReturnValue()->Accept(this);
    m_hCurrent = h;
    return Value();
}

Value TreeViewVisitor::VisitBreakContinue(BreakOrContinueStatement const* stmt) {
    m_Tree.InsertItem(CString(stmt->ToString().c_str()), m_hCurrent, TVI_LAST);
    return Value();
}

Value TreeViewVisitor::VisitFor(ForStatement const* stmt) {
    auto h = m_hCurrent;
    m_hCurrent = m_Tree.InsertItem(Helpers::AstNodeTypeToString(stmt->NodeType()), m_hCurrent, TVI_LAST);
    if (stmt->Init())
        stmt->Init()->Accept(this);
    else
        m_Tree.InsertItem(L"(No Initializer)", m_hCurrent, TVI_LAST);
    if (stmt->While())
        stmt->While()->Accept(this);
    else
        m_Tree.InsertItem(L"(true)", m_hCurrent, TVI_LAST);
    if (stmt->Inc())
        stmt->Inc()->Accept(this);
    else
        m_Tree.InsertItem(L"(No inc)", m_hCurrent, TVI_LAST);
    stmt->Body()->Accept(this);
    m_hCurrent = h;
    return Value();
}

Value TreeViewVisitor::VisitStatements(Statements const* stmts) {
    auto h = m_hCurrent;
    m_hCurrent = m_Tree.InsertItem(Helpers::AstNodeTypeToString(stmts->NodeType()), m_hCurrent, TVI_LAST);
    for (auto& stmt : stmts->All()) {
        stmt->Accept(this);
    }
    m_hCurrent = h;
    return Value();
}

Value TreeViewVisitor::VisitAnonymousFunction(AnonymousFunctionExpression const* func) {
    return Value();
}

Value TreeViewVisitor::VisitEnumDeclaration(EnumDeclaration const* decl) {
    auto h = m_hCurrent;
    m_hCurrent = m_Tree.InsertItem(CString(("Enum " + decl->Name()).c_str()), m_hCurrent, TVI_LAST);
    for (auto& [name, value] : decl->Values()) {
        m_Tree.InsertItem(CString(std::format("{} = {}", name, value).c_str()), m_hCurrent, TVI_LAST);
    }
    m_hCurrent = h;
    return Value();
}

Value TreeViewVisitor::VisitExpressionStatement(ExpressionStatement const* expr) {
    return Value();
}

Value TreeViewVisitor::VisitArrayExpression(ArrayExpression const* expr) {
    return Value();
}

Value TreeViewVisitor::VisitGetMember(GetMemberExpression const* expr) {
    return Value();
}

Value TreeViewVisitor::VisitAccessArray(AccessArrayExpression const* expr) {
    return Value();
}

Value TreeViewVisitor::VisitAssignArrayIndex(AssignArrayIndexExpression const* expr) {
    return Value();
}

Value TreeViewVisitor::VisitClassDeclaration(ClassDeclaration const* decl) {
    return Value();
}

Value TreeViewVisitor::VisitNewObjectExpression(NewObjectExpression const* expr) {
    return Value();
}

Value TreeViewVisitor::VisitAssignField(AssignFieldExpression const* expr) {
    return Value();
}

Value TreeViewVisitor::VisitForEach(ForEachStatement const* stmt) {
    return Value();
}

Value TreeViewVisitor::VisitRange(RangeExpression const* expr) {
    return Value();
}

Value TreeViewVisitor::VisitMatch(MatchExpression const* expr) {
    return Value();
}

Value TreeViewVisitor::VisitUse(UseStatement const* use) {
    return Value();
}
