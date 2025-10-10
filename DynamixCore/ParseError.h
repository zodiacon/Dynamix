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
		InvalidLhs,
		UnexpectedToken,
		IllegalThis,
		ModifierConflict,
		WrongParameterCount,
	};

	struct ParseError {
		ParseError(ParseErrorType type, Token const& token, std::string desc = "");
		ParseError(ParseErrorType type, CodeLocation location, std::string desc = "");

		std::string const& Description() const {
			return m_Description;
		}

		CodeLocation const& Location() const {
			return m_Location;
		}

		ParseErrorType Type() const {
			return m_Type;
		}

	private:
		ParseErrorType m_Type;
		CodeLocation m_Location;
		std::string m_Description;
	};
}


