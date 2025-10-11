#include "DebugHelper.h"

using namespace Dynamix;

const char* DebugHelper::AstNodeTypeToString(AstNodeType type) noexcept {
	switch (type) {
		case AstNodeType::Literal: return "Literal";
		case AstNodeType::IfThenElse: return "If/Then/Else";
		case AstNodeType::Name: return "Name";
		case AstNodeType::InvokeFunction: return "Invoke Function";
		case AstNodeType::AnonymousFunction: return "Anonymous Function";
		case AstNodeType::Binary: return "Binary";
		case AstNodeType::Unary: return "Unary";
		case AstNodeType::ArrayAccess: return "Array Access";

		case AstNodeType::Statements: return "Statements";
		case AstNodeType::FunctionDeclaration: return "Function Declaration";
		case AstNodeType::VarValStatement: return "Var/Val Declaration";
		case AstNodeType::GetMember: return "Get Member";
		case AstNodeType::ClassDeclaration: return "Class Declaration";
		case AstNodeType::EnumDeclararion: return "Enum Declaration";
		case AstNodeType::InterfaceDeclaration: return "Interface Declaration";
	}
	return "(Unknown)";
}
