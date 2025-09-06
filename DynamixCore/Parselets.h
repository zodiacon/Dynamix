#pragma once

#include <memory>

namespace Dynamix {
	class Parser;
	class Expression;
	struct Token;

	struct InfixParslet {
		virtual std::unique_ptr<Expression> Parse(Parser& parser, std::unique_ptr<Expression> left, Token const& token) = 0;
		virtual int Precedence() const = 0;
	};

	struct PrefixParslet {
		virtual std::unique_ptr<Expression> Parse(Parser& parser, Token const& token) = 0;
		virtual int Precedence() const {
			return 0;
		}
	};

	struct LiteralParslet : PrefixParslet {
		std::unique_ptr<Expression> Parse(Parser& parser, Token const& token) override;
	};

	struct NameParslet : PrefixParslet {
		explicit NameParslet(bool isthis = false) : m_IsThis(isthis) {}
		std::unique_ptr<Expression> Parse(Parser& parser, Token const& token) override;

	private:
		bool m_IsThis;
	};

	struct NewOperatorParslet : PrefixParslet {
		std::unique_ptr<Expression> Parse(Parser& parser, Token const& token) override;
	};

	struct GroupParslet : PrefixParslet {
		std::unique_ptr<Expression> Parse(Parser& parser, Token const& token) override;
		int Precedence() const override;
	};

	struct PrefixOperatorParslet : PrefixParslet {
		explicit PrefixOperatorParslet(int precedence);

		std::unique_ptr<Expression> Parse(Parser& parser, Token const& token) override;
		int Precedence() const override;

	private:
		int m_Precedence;
	};

	struct PostfixOperatorParslet : InfixParslet {
		explicit PostfixOperatorParslet(int precedence);
		int Precedence() const override;

	private:
		int m_Precedence;
	};

	struct InvokeFunctionParslet : PostfixOperatorParslet {
		InvokeFunctionParslet();
		std::unique_ptr<Expression> Parse(Parser& parser, std::unique_ptr<Expression> left, Token const& token) override;
	};

	struct EnumValueParslet : PostfixOperatorParslet {
		EnumValueParslet() : PostfixOperatorParslet(1300) {}
		std::unique_ptr<Expression> Parse(Parser& parser, std::unique_ptr<Expression> left, Token const& token) override;
	};
	struct GetMemberParslet : PostfixOperatorParslet {
		GetMemberParslet() : PostfixOperatorParslet(1200) {}
		std::unique_ptr<Expression> Parse(Parser& parser, std::unique_ptr<Expression> left, Token const& token) override;
	};

	struct ArrayAccessParslet : PostfixOperatorParslet {
		ArrayAccessParslet() : PostfixOperatorParslet(1250) {}
		std::unique_ptr<Expression> Parse(Parser& parser, std::unique_ptr<Expression> left, Token const& token) override;
	};

	struct BinaryOperatorParslet : InfixParslet {
		explicit BinaryOperatorParslet(int precedence, bool right = false);
		std::unique_ptr<Expression> Parse(Parser& parser, std::unique_ptr<Expression> left, Token const& token) override;
		int Precedence() const override;

	private:
		int m_Precedence;
		bool m_RightAssoc;
	};

	struct AssignParslet : InfixParslet {
		std::unique_ptr<Expression> Parse(Parser& parser, std::unique_ptr<Expression> left, Token const& token) override;
		int Precedence() const override;
	};

	struct IfThenElseParslet : PrefixParslet {
		std::unique_ptr<Expression> Parse(Parser& parser, Token const& token) override;
	};

	struct AnonymousFunctionParslet : PrefixParslet {
		std::unique_ptr<Expression> Parse(Parser& parser, Token const& token) override;
		int Precedence() const override;
	};

	struct ArrayExpressionParslet : PrefixParslet {
		std::unique_ptr<Expression> Parse(Parser& parser, Token const& token) override;
		int Precedence() const override {
			return 0;
		}
	};
}
