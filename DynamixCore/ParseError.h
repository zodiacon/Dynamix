#pragma once

#include <string_view>
#include "Token.h"

namespace Dynamix {
	enum class ParseErrorType {
		NoError,
		Syntax,
		Expected,
		UnknownOperator,
		IdentifierExpected,
		MissingInitExpression,
		SemicolonExpected,
		AssignExpected,
		CommaExpected,
		CommaOrCloseParenExpected,
		DuplicateDefinition,
		UndefinedSymbol,
		CannotModifyConst,
		OpenParenExpected,
		CloseParenExpected,
		OpenBraceExpected,
		CloseBraceExpected,
		InvalidStatement,
		ConditionExpressionExpected,
		BreakContinueNoLoop,
		ExpressionOrVarExpected,
		IllegalExpression,
		TooManyFunctionArgs,
	};

	struct ParseError {
		ParseErrorType Type;
		Token Info;
		std::string Description;
	};
}


