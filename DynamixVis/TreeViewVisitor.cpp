#include "pch.h"
#include "TreeViewVisitor.h"
#include "Helpers.h"
#include <Value.h>
#include <AstNode.h>
#include <format>

using namespace Dynamix;

TreeViewVisitor::TreeViewVisitor(HWND hTreeView, Tokenizer& t) : m_Tree(hTreeView), m_Tokenizer(t) {
}

void TreeViewVisitor::Visit(AstNode* node, HTREEITEM hRoot) {
    m_hCurrent = hRoot;
    node->Accept(this);
}

Value TreeViewVisitor::VisitLiteral(LiteralExpression const* expr) {
    auto h = m_Tree.InsertItem(LocationAsString(expr) + L"Literal: " + CString(expr->Literal().ToString().c_str()), m_hCurrent, TVI_LAST);
    m_Tree.SetItemData(h, reinterpret_cast<DWORD_PTR>(expr));
    return Value();
}

Value TreeViewVisitor::VisitBinary(BinaryExpression const* expr) {
    auto h = m_hCurrent;
    m_hCurrent = m_Tree.InsertItem(LocationAsString(expr) + CString(std::format("Binary Operator: {}", m_Tokenizer.TokenTypeToString(expr->Operator())).c_str()), m_hCurrent, TVI_LAST);
    m_Tree.SetItemData(m_hCurrent, reinterpret_cast<DWORD_PTR>(expr));

    expr->Left()->Accept(this);
    expr->Right()->Accept(this);
    m_hCurrent = h;

    return Value();
}

Value TreeViewVisitor::VisitUnary(UnaryExpression const* expr) {
    auto h = m_hCurrent;
    m_hCurrent = m_Tree.InsertItem(LocationAsString(expr) + CString(std::format("Unary Operator: {}", m_Tokenizer.TokenTypeToString(expr->Operator())).c_str()), m_hCurrent, TVI_LAST); 
    m_Tree.SetItemData(m_hCurrent, reinterpret_cast<DWORD_PTR>(expr));
    expr->Arg()->Accept(this);
    m_hCurrent = h;
    return Value();
}

Value TreeViewVisitor::VisitName(NameExpression const* expr) {
    auto h = m_Tree.InsertItem(LocationAsString(expr) + CString(std::format("Name: {}", expr->Name()).c_str()) , m_hCurrent, TVI_LAST);
    m_Tree.SetItemData(h, reinterpret_cast<DWORD_PTR>(expr));
    return Value();
}

Value TreeViewVisitor::VisitVar(VarValStatement const* expr) {
    auto h = m_hCurrent;
    m_hCurrent = m_Tree.InsertItem(LocationAsString(expr) + (expr->IsConst() ? L"val " : L"var ") + CString(expr->Name().c_str()), m_hCurrent, TVI_LAST);
    m_Tree.SetItemData(m_hCurrent, reinterpret_cast<DWORD_PTR>(expr));
    if(expr->Init())
        expr->Init()->Accept(this);
    m_hCurrent = h;
    return Value();
}

Value TreeViewVisitor::VisitAssign(AssignExpression const* expr) {
    auto h = m_hCurrent;
    m_hCurrent = m_Tree.InsertItem(LocationAsString(expr) + L"Assign: " + CString(expr->Lhs().c_str()) + L" " + CString(m_Tokenizer.TokenTypeToString(expr->AssignType()).data()), m_hCurrent, TVI_LAST);
    m_Tree.SetItemData(m_hCurrent, reinterpret_cast<DWORD_PTR>(expr));
    expr->Value()->Accept(this);
    m_hCurrent = h;
    return Value();
}

Value TreeViewVisitor::VisitInvokeFunction(InvokeFunctionExpression const* expr) {
    auto h = m_hCurrent;
    m_hCurrent = m_Tree.InsertItem(LocationAsString(expr) + L"Invoke Function (callable)", m_hCurrent, TVI_LAST);
    m_Tree.SetItemData(m_hCurrent, reinterpret_cast<DWORD_PTR>(expr));
    expr->Callable()->Accept(this);
    if (!expr->Arguments().empty()) {
        m_hCurrent = m_Tree.InsertItem(L"Arguments", m_hCurrent, TVI_LAST);
        for (auto& arg : expr->Arguments())
            arg->Accept(this);
    }
    m_hCurrent = h;
    return Value();
}

Value TreeViewVisitor::VisitRepeat(RepeatStatement const* expr) {
    auto h = m_hCurrent;
    m_hCurrent = m_Tree.InsertItem(LocationAsString(expr) + L"Repeat (times, body)", m_hCurrent, TVI_LAST);
    m_Tree.SetItemData(m_hCurrent, reinterpret_cast<DWORD_PTR>(expr));
    expr->Times()->Accept(this);
    expr->Body()->Accept(this);
    m_hCurrent = h;
    return Value();
}

Value TreeViewVisitor::VisitWhile(WhileStatement const* stmt) {
    auto h = m_hCurrent;
    m_hCurrent = m_Tree.InsertItem(LocationAsString(stmt) + L"While (expr, body)", m_hCurrent, TVI_LAST);
    m_Tree.SetItemData(m_hCurrent, reinterpret_cast<DWORD_PTR>(stmt));
    stmt->Condition()->Accept(this);
    stmt->Body()->Accept(this);
    m_hCurrent = h;
    return Value();
}

Value TreeViewVisitor::VisitIfThenElse(IfThenElseExpression const* expr) {
    auto h = m_hCurrent;
    m_hCurrent = m_Tree.InsertItem(LocationAsString(expr) + L"If/Then/Else", m_hCurrent, TVI_LAST);
    m_Tree.SetItemData(m_hCurrent, reinterpret_cast<DWORD_PTR>(expr));

    expr->Condition()->Accept(this);
    auto hRoot = m_hCurrent;
    m_hCurrent = m_Tree.InsertItem(L"Then", m_hCurrent, TVI_LAST);
    expr->Then()->Accept(this);
    if (expr->Else()) {
        m_hCurrent = m_Tree.InsertItem(L"Else", hRoot, TVI_LAST);
        expr->Else()->Accept(this);
    }
    m_hCurrent = h;
    return Value();
}

Value TreeViewVisitor::VisitFunctionDeclaration(FunctionDeclaration const* decl) {
    auto h = m_hCurrent;
    m_hCurrent = m_Tree.InsertItem(LocationAsString(decl) + CString(std::format("Function Declaration: {}", decl->Name()).c_str()), m_hCurrent, TVI_LAST);
    m_Tree.SetItemData(m_hCurrent, reinterpret_cast<DWORD_PTR>(decl));

    decl->Body()->Accept(this);
    m_hCurrent = h;
    return Value();
}

Value TreeViewVisitor::VisitReturn(ReturnStatement const* decl) {
    auto h = m_hCurrent;
    m_hCurrent = m_Tree.InsertItem(LocationAsString(decl) + L"Return", m_hCurrent, TVI_LAST);
    m_Tree.SetItemData(m_hCurrent, reinterpret_cast<DWORD_PTR>(decl));
    decl->ReturnValue()->Accept(this);
    m_hCurrent = h;
    return Value();
}

Value TreeViewVisitor::VisitBreakContinue(BreakOrContinueStatement const* stmt) {
    auto h = m_Tree.InsertItem(LocationAsString(stmt) + CString(stmt->ToString().c_str()), m_hCurrent, TVI_LAST);
    m_Tree.SetItemData(h, reinterpret_cast<DWORD_PTR>(stmt));
    return Value();
}

Value TreeViewVisitor::VisitFor(ForStatement const* stmt) {
    auto h = m_hCurrent;
    m_hCurrent = m_Tree.InsertItem(LocationAsString(stmt) + L"For", m_hCurrent, TVI_LAST);
    m_Tree.SetItemData(m_hCurrent, reinterpret_cast<DWORD_PTR>(stmt));
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
    for (auto& stmt : stmts->All()) {
        stmt->Accept(this);
    }
    return Value();
}

Value TreeViewVisitor::VisitAnonymousFunction(AnonymousFunctionExpression const* func) {
    auto h = m_hCurrent;
    m_hCurrent = m_Tree.InsertItem(LocationAsString(func) + Helpers::AstNodeTypeToString(func->NodeType()), m_hCurrent, TVI_LAST);
    m_Tree.SetItemData(m_hCurrent, reinterpret_cast<DWORD_PTR>(func));
    if (!func->Parameters().empty()) {
        CString params;
        for (auto& param : func->Parameters()) {
            params += CString(param.Name.c_str()) + L", ";
        }
        m_Tree.InsertItem(L"Parameters: " + params.Left(params.GetLength() - 2), m_hCurrent, TVI_LAST);
    }
    func->Body()->Accept(this);
    m_hCurrent = h;
    return Value();
}

Value TreeViewVisitor::VisitEnumDeclaration(EnumDeclaration const* decl) {
    auto h = m_hCurrent;
    m_hCurrent = m_Tree.InsertItem(LocationAsString(decl) + CString(("Enum " + decl->Name()).c_str()), m_hCurrent, TVI_LAST);
    m_Tree.SetItemData(m_hCurrent, reinterpret_cast<DWORD_PTR>(decl));
    for (auto& [name, value] : decl->Values()) {
        m_Tree.InsertItem(CString(std::format("{} = {}", name, value).c_str()), m_hCurrent, TVI_LAST);
    }
    m_hCurrent = h;
    return Value();
}

Value TreeViewVisitor::VisitExpressionStatement(ExpressionStatement const* expr) {
    return expr->Expr()->Accept(this);
}

Value TreeViewVisitor::VisitArrayExpression(ArrayExpression const* expr) {  
    auto h = m_hCurrent;
    m_hCurrent = m_Tree.InsertItem(LocationAsString(expr) + L"Array", m_hCurrent, TVI_LAST);
    m_Tree.SetItemData(m_hCurrent, reinterpret_cast<DWORD_PTR>(expr));
    for (auto& item : expr->Items())
        item->Accept(this);
    m_hCurrent = h;
    return Value();
}

Value TreeViewVisitor::VisitGetMember(GetMemberExpression const* expr) {
    auto h = m_hCurrent;
    m_hCurrent = m_Tree.InsertItem(LocationAsString(expr) + CString(("Get Member: " + expr->Member()).c_str()), m_hCurrent, TVI_LAST);
    m_Tree.SetItemData(m_hCurrent, reinterpret_cast<DWORD_PTR>(expr));
    expr->Left()->Accept(this);
    m_hCurrent = h;
    return Value();
}

Value TreeViewVisitor::VisitAccessArray(AccessArrayExpression const* expr) {
    auto h = m_hCurrent;
    m_hCurrent = m_Tree.InsertItem(LocationAsString(expr) + L"Access Array", m_hCurrent, TVI_LAST);
    m_Tree.SetItemData(m_hCurrent, reinterpret_cast<DWORD_PTR>(expr));
    expr->Left()->Accept(this);
    expr->Index()->Accept(this);
    m_hCurrent = h;
    return Value();
}

Value TreeViewVisitor::VisitAssignArrayIndex(AssignArrayIndexExpression const* expr) {
    auto h = m_hCurrent;
    m_hCurrent = m_Tree.InsertItem(LocationAsString(expr) + L"Assign Array Index: " + CString(m_Tokenizer.TokenTypeToString(expr->AssignType()).data()), m_hCurrent, TVI_LAST);
    expr->ArrayAccess()->Accept(this);
    expr->Value()->Accept(this);
    m_hCurrent = h;
    return Value();
}

Value TreeViewVisitor::VisitClassDeclaration(ClassDeclaration const* decl) {
    auto h = m_hCurrent;
    CString title = L"Class: " + CString(decl->Name().c_str());
    if (!decl->BaseName().empty())
        title += L" Base: " + CString(decl->BaseName().c_str());
    m_hCurrent = m_Tree.InsertItem(LocationAsString(decl) + title, m_hCurrent, TVI_LAST);
    m_Tree.SetItemData(m_hCurrent, reinterpret_cast<DWORD_PTR>(decl));
    auto hRoot = m_hCurrent;
    if (!decl->Fields().empty()) {
        m_hCurrent = m_Tree.InsertItem(L"Fields", hRoot, TVI_LAST);
        for (auto& f : decl->Fields())
            f->Accept(this);
    }
    if (!decl->Methods().empty()) {
        m_hCurrent = m_Tree.InsertItem(L"Methods", hRoot, TVI_LAST);
        for (auto& m : decl->Methods())
            m->Accept(this);
    }
    if (!decl->Types().empty()) {
        m_hCurrent = m_Tree.InsertItem(L"Types", hRoot, TVI_LAST);
        for (auto& t : decl->Types())
            t->Accept(this);
    }

    m_hCurrent = h;
    return Value();
}

Value TreeViewVisitor::VisitNewObjectExpression(NewObjectExpression const* expr) {
    auto h = m_hCurrent;
    m_hCurrent = m_Tree.InsertItem(LocationAsString(expr) + CString(("New Object: " + expr->ClassName()).c_str()), m_hCurrent, TVI_LAST);
    m_Tree.SetItemData(m_hCurrent, reinterpret_cast<DWORD_PTR>(expr));
    auto hRoot = m_hCurrent;
    if (!expr->Arguments().empty()) {
        m_hCurrent = m_Tree.InsertItem(L"Arguments", m_hCurrent, TVI_LAST);
        for (auto& arg : expr->Arguments())
            arg->Accept(this);
    }
    if (!expr->FieldInitializers().empty()) {
        hRoot = m_Tree.InsertItem(L"Field initializers", hRoot, TVI_LAST);
        for (auto& fi : expr->FieldInitializers()) {
            m_hCurrent = m_Tree.InsertItem(CString(fi.Name.c_str()), hRoot, TVI_LAST);
            fi.Init->Accept(this);
        }
    }
    m_hCurrent = h;
    return Value();
}

Value TreeViewVisitor::VisitAssignField(AssignFieldExpression const* expr) {
    auto h = m_hCurrent;
    m_hCurrent = m_Tree.InsertItem(LocationAsString(expr) + L"Assign Field (field, value)", m_hCurrent, TVI_LAST);
    m_Tree.SetItemData(m_hCurrent, reinterpret_cast<DWORD_PTR>(expr));
    expr->Lhs()->Accept(this);
    expr->Value()->Accept(this);
    m_hCurrent = h;
    return Value();
}

Value TreeViewVisitor::VisitForEach(ForEachStatement const* stmt) {
    auto h = m_hCurrent;
    m_hCurrent = m_Tree.InsertItem(LocationAsString(stmt) + L"For Each " + CString(stmt->Name().c_str()), m_hCurrent, TVI_LAST);
    m_Tree.SetItemData(m_hCurrent, reinterpret_cast<DWORD_PTR>(stmt));
    stmt->Collection()->Accept(this);
    stmt->Body()->Accept(this);
    m_hCurrent = h;
    return Value();
}

Value TreeViewVisitor::VisitRange(RangeExpression const* expr) {
    auto h = m_hCurrent;
    m_hCurrent = m_Tree.InsertItem(LocationAsString(expr) + CString(L"Range (start, end)") + (expr->EndInclusive() ? L"[End Inclusive]" : L""), m_hCurrent, TVI_LAST);
    m_Tree.SetItemData(m_hCurrent, reinterpret_cast<DWORD_PTR>(expr));
    expr->Start()->Accept(this);
    expr->End()->Accept(this);
    m_hCurrent = h;
    return Value();
}

Value TreeViewVisitor::VisitMatch(MatchExpression const* expr) {
    m_Tree.InsertItem(LocationAsString(expr) + Helpers::AstNodeTypeToString(expr->NodeType()), m_hCurrent, TVI_LAST);
    return Value();
}

Value TreeViewVisitor::VisitUse(UseStatement const* use) {
    auto h = m_Tree.InsertItem(LocationAsString(use) + L"Use " + CString(use->Name().c_str()), m_hCurrent, TVI_LAST);
    m_Tree.SetItemData(h, reinterpret_cast<DWORD_PTR>(use));
    return Value();
}

CString TreeViewVisitor::LocationAsString(AstNode const* node) const {
    CString loc;
    loc.Format(L"(%d,%d); ", node->Location().Line, node->Location().Col);
    return loc;
}
