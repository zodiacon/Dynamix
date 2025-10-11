#include "pch.h"
#include "Helpers.h"
#include <Tokenizer.h>

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
		case AstNodeType::AssignField: return L"Assign Field";

		case AstNodeType::Statements: return L"Statements";
		case AstNodeType::FunctionDeclaration: return L"Function Declaration";
		case AstNodeType::VarValStatement: return L"Var/Val Declaration";
		case AstNodeType::GetMember: return L"Get Member";
		case AstNodeType::ClassDeclaration: return L"Class Declaration";
		case AstNodeType::EnumDeclararion: return L"Enum Declaration";
		case AstNodeType::InterfaceDeclaration: return L"Interface Declaration";
		case AstNodeType::NewObject: return L"New Object";
		case AstNodeType::For: return L"For";
		case AstNodeType::ForEach: return L"ForEach";
		case AstNodeType::Use: return L"Use";
		case AstNodeType::Array: return L"Array";
		case AstNodeType::Repeat: return L"Repeat";
		case AstNodeType::Return: return L"Return";
		case AstNodeType::Match: return L"Match";
	}
	return L"(Unknown)";
}

