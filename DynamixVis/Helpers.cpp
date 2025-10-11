#include "pch.h"
#include "Helpers.h"

using namespace Dynamix;

PCWSTR Helpers::AstNodeTypeToString(AstNodeType type) noexcept {
	switch (type) {
		case AstNodeType::Literal: return L"Literal";
		case AstNodeType::IfThenElse: return L"If/Then/Else";
		case AstNodeType::Name: return L"Name";
		case AstNodeType::InvokeFunction: return L"Invoke Function";
		case AstNodeType::AnonymousFunction: return L"Anonymous Function";
		case AstNodeType::Binary: return L"Binary";
		case AstNodeType::Unary: return L"Unary";
		case AstNodeType::ArrayAccess: return L"Array Access";

		case AstNodeType::Statements: return L"Statements";
		case AstNodeType::FunctionDeclaration: return L"Function Declaration";
		case AstNodeType::VarValStatement: return L"Var/Val Declaration";
		case AstNodeType::GetMember: return L"Get Member";
		case AstNodeType::ClassDeclaration: return L"Class Declaration";
		case AstNodeType::EnumDeclararion: return L"Enum Declaration";
		case AstNodeType::InterfaceDeclaration: return L"Interface Declaration";
	}
	return L"(Unknown)";
}
