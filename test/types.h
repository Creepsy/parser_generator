#pragma once

#include <vector>
#include <memory>
#include <string>

#include "lexer.h"

namespace parser {
	struct TypeBase {
		virtual ~TypeBase(){}
	};

	struct Value {
		virtual ~Value(){}
	};

	struct Numeric {
		virtual int calc() = 0;
		virtual ~Numeric(){}
	};

	struct List : public TypeBase, public Value {
		std::vector<std::shared_ptr<Value>> member;

		List(const std::vector<std::shared_ptr<Value>>& member) : member(member){}
	};

	struct Integer : public Numeric, public TypeBase, public Value {
		lexer::token num;

		Integer(const lexer::token& num) : num(num){}
		int calc() override { return std::stoi(this->num.identifier); }
	};

	struct Addition : public Numeric, public TypeBase, public Value {
		std::shared_ptr<Numeric> first;
		lexer::token op;
		std::shared_ptr<Numeric> second;

		Addition(const std::shared_ptr<Numeric>& first, const lexer::token& op, const std::shared_ptr<Numeric>& second) : first(first), op(op), second(second){}
		int calc() override { 
			return first->calc() + second->calc();
		}
	};

}
