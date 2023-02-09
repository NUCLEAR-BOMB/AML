#pragma once

#include <AML/Functions.hpp>
#include <AML/Containers.hpp>

#include <string_view>
#include <cstddef>

namespace aml
{

template<std::size_t StringSize>
class Expression
{
public:

	using cstring_type = const char*;

	using string_view_type = std::string_view;

	using size_type = std::size_t;

	using number_type = double;

private:
	enum class Operator_associativity {
		Left,
		Right
	};

	using operator_args_type = aml::fixed_vector<number_type, StringSize>;

	struct operator_signature
	{
		string_view_type name;
		unsigned int precedence;
		Operator_associativity associativity;

		unsigned int argc;
		std::add_pointer_t<number_type(const number_type*)> function;
	};

	static constexpr std::array operator_table
	{
		operator_signature{"+", 1, Operator_associativity::Left,
			2, +[](const number_type* args) { return args[0] + args[1]; }},
		operator_signature{"-", 1, Operator_associativity::Left,
			2, +[](const number_type* args) { return args[0] - args[1]; }},
		operator_signature{"*", 2, Operator_associativity::Left,
			2, +[](const number_type* args) { return args[0] * args[1]; }},
		operator_signature{"/", 2, Operator_associativity::Left,
			2, +[](const number_type* args) { return args[0] / args[1]; }},
	};

public:

class Token
{
public:

	enum class Type {
		Number,
		Operator,
		Bracket,
	};

	constexpr
	Token() noexcept 
		: m_type(static_cast<Type>(INT_MAX)), m_value() {}

	constexpr
	Token(Type type, cstring_type str, size_type cnt) noexcept
		: m_type(type), m_value(str, cnt) {}

	constexpr
	Token(cstring_type str) noexcept 
		: Token()
	{
		constexpr auto is_digit = [](char chr) {
			return aml::is_digit(static_cast<unsigned char>(chr)) || chr == '.';
		};
		constexpr auto is_bracket = [](char chr) {
			return chr == '(' || chr == ')';
		};

		do {
		if (is_digit(str[0])) {
			m_type = Type::Number; break;
		}
		if (is_bracket(str[0])) {
			m_type = Type::Bracket;
			m_value = string_view_type(str, static_cast<size_type>(1));
			return;
		}
		m_type = Type::Operator;
		} while (0);

		auto end = str + 1;
		for (auto it = end; *it != '\0'; ++it)
		{
			if (m_type == Type::Number && !is_digit(*it)) {
				end = it; break;
			}
			if (m_type == Type::Operator && 
				(!aml::is_graph(static_cast<unsigned char>(*it)) || is_digit(*it) || is_bracket(*it))) 
			{
				end = it; break;
			}
		}

		m_value = string_view_type(str, static_cast<size_type>(end - str));
	}

	constexpr
	const string_view_type value() const noexcept {
		return m_value;
	}

	constexpr
	Type type() const noexcept {
		return m_type;
	}

private:
	Type m_type;
	string_view_type m_value;
}; // class Token

	using stack_type = aml::fixed_vector<Token, StringSize>;

private:
	constexpr
	const auto& find_operator(string_view_type opname) const noexcept 
	{
		auto find_res = [&]() {
			auto opsign = operator_table.cbegin();
			for (; opsign != operator_table.cend(); ++opsign) 
			{
				if (opsign->name == opname) return opsign;
			}
			return opsign;
		}();
		
		if (find_res != operator_table.end()) {
			return *find_res;
		} else {
			AML_DEBUG_ERROR("Unknown token operator: %s", opname.data());
		}
	}

	constexpr
	void add_operator_token(const Token& token) noexcept
	{
		if (m_operator_stack.empty()) {
			m_operator_stack.push_back(token); return;
		}

		auto& last_token = m_operator_stack.back();

		if (last_token.type() == Token::Type::Bracket) 
		{
			m_operator_stack.push_back(token);
			return;
		}

		auto find_res = this->find_operator(token.value());
		auto back_find_res = this->find_operator(last_token.value());

		auto first_associativity = find_res.associativity;

		auto first_precedence = find_res.precedence;
		auto second_precedence = back_find_res.precedence;

		if (first_associativity != Operator_associativity::Right 
			&& first_precedence == second_precedence)
		{
			m_output_stack.push_back(m_operator_stack.pop_back());
			m_operator_stack.push_back(token);
			return;
		}

		if (first_precedence < second_precedence) 
		{
			m_output_stack.push_back(m_operator_stack.pop_back());
		}
		m_operator_stack.push_back(token);
	}

	constexpr
	void add_bracket_token(const Token& token) noexcept
	{
		if (token.value() == string_view_type("(")) {
			m_operator_stack.push_back(token); return;
		}

		for (auto it = m_operator_stack.rbegin(); it->type() != Token::Type::Bracket; ++it)
		{
			m_output_stack.push_back(m_operator_stack.pop_back());
		}
		m_operator_stack.pop_back(); // pop )
	}

	constexpr
	number_type invoke_operator(operator_args_type& stack, const Token& optoken) const noexcept
	{
		auto op_sign = this->find_operator(optoken.value());
		auto op_invoke_func = op_sign.function;

		auto args_count = op_sign.argc;

		auto result = op_invoke_func(&stack.back() - (args_count - 1));

		stack.resize(stack.size() - args_count);

		return result;
	}

public:

	AML_CONSTEVAL 
	Expression(const char(&str)[StringSize]) noexcept 
	{
		this->generate(str);
	}

	constexpr
	const stack_type& generate(const char(&str)[StringSize]) noexcept
	{
		for (auto it = std::begin(str); (*it) != '\0';)
		{
			if (*it == ' ') {
				++it; continue;
			}

			auto token = Token(it);
			it = token.value().data() + token.value().size();
			
			switch (token.type())
			{
			case Token::Type::Bracket:
				this->add_bracket_token(token); continue;

			case Token::Type::Number:
				m_output_stack.push_back(token); continue;

			case Token::Type::Operator:
				this->add_operator_token(token); continue;

			default:
				AML_DEBUG_ERROR("Unknown token type");
			}
		}

		while (m_operator_stack.size() != 0) {
			m_output_stack.push_back(m_operator_stack.pop_back());
		}

		return m_output_stack;
	}

	constexpr
	number_type calculate() const noexcept
	{	
		operator_args_type number_stack;
		for (const auto& token : m_output_stack)
		{
			if (token.type() == Token::Type::Number) {
				number_stack.push_back(aml::string_to_double(token.value().data(), token.value().size()));
			} else {
				number_stack.push_back(
					this->invoke_operator(number_stack, token)
				);
			}
		}

		AML_DEBUG_VERIFY(number_stack.size() == 1, "Stack must has one value");

		return number_stack.pop_back();
	}

private:
	stack_type m_output_stack;
	stack_type m_operator_stack;
};

template<std::size_t N>
Expression(const char(&)[N]) -> Expression<N>;

}