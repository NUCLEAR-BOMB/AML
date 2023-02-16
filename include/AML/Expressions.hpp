#pragma once

#include <AML/Functions.hpp>
#include <AML/Containers.hpp>

#include <string_view>
#include <cstddef>
#include <variant>
#include <utility>
#include <algorithm>

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
			2, [](const number_type* args) { return args[0] + args[1]; }},
		operator_signature{"-", 1, Operator_associativity::Left,
			2, [](const number_type* args) { return args[0] - args[1]; }},
		operator_signature{"*", 2, Operator_associativity::Left,
			2, [](const number_type* args) { return args[0] * args[1]; }},
		operator_signature{"/", 2, Operator_associativity::Left,
			2, [](const number_type* args) { return args[0] / args[1]; }},
	};

public:

class Token
{
public:

	enum class Type {
		Number,
		Operator,
		Bracket,
		Variable,
	};

	using value_type = std::variant<number_type, string_view_type>;
	using size_type = std::size_t;

	constexpr
	Token() noexcept 
		: m_type(static_cast<Type>(INT_MAX)), m_value() {}

	constexpr
	explicit Token(Type type, string_view_type str) noexcept
		: m_type(type), m_value([&] {
			if (type == Type::Number) {
				return value_type{ aml::string_to_double(str.data(), str.size()) };
			} else {
				return value_type{ str };
			}
		}()) {}
	constexpr
	explicit Token(number_type num) noexcept
		: m_type(Type::Number), m_value(num) {}

	constexpr
	const string_view_type str() const noexcept {
		if (const auto* val_ptr = std::get_if<string_view_type>(&m_value)) {
			return *val_ptr;
		} else {
			AML_DEBUG_ERROR("Value is not a string");
		}
	}
	constexpr
	number_type number() const noexcept {
		if (const auto* val_ptr = std::get_if<number_type>(&m_value)) {
			return *val_ptr;
		} else {
			AML_DEBUG_ERROR("Value is not a number");
		}
	}

	constexpr
	Type type() const noexcept {
		return m_type;
	}

private:
	Type m_type;
	value_type m_value;
}; // class Token

	constexpr static
	std::pair<Token, size_type> make_token(const cstring_type str) noexcept
	{
		constexpr auto is_digit = [](char chr) {
			return aml::is_digit(static_cast<unsigned char>(chr)) || chr == '.';
		};
		constexpr auto is_bracket = [](char chr) {
			return chr == '(' || chr == ')';
		};

		typename Token::Type type{};
		bool maybe_variable = false;

		if (is_digit(str[0])) {
			type = Token::Type::Number;
		} else
		if (is_bracket(str[0])) {
			return { Token{
				Token::Type::Bracket,
				string_view_type(str, 1)
			}, 1};
		} else {
			type = Token::Type::Operator;
			if (aml::is_alpha(str[0])) {
				maybe_variable = true;
			}
		}

		auto end = (str + 1);
		for (; *end != '\0'; ++end)
		{
			if (type == Token::Type::Number && !is_digit(*end)) {
				break;
			}
			if (type == Token::Type::Operator &&
				(!aml::is_graph(*end) || is_digit(*end) || is_bracket(*end)))
			{
				break;
			}
		}

		if (maybe_variable && ((*end != '\0') ? (*(end + 1) != '(') : false)) {
			type = Token::Type::Variable;
		}

		const auto str_size = static_cast<size_type>(end - str);
		return { Token{
			type,
			string_view_type{str, str_size}
		}, str_size};
	}

	using stack_type = aml::fixed_vector<Token, StringSize>;

private:
	constexpr
	const operator_signature& find_operator(string_view_type opname) const noexcept
	{
		auto op_sign = operator_table.begin();
		for (; op_sign != operator_table.end(); ++op_sign)
		{
			if (op_sign->name == opname) break;
		}
		
		if (op_sign != operator_table.end()) {
			return *op_sign;
		} else {
			AML_DEBUG_ERROR("Unknown token operator: %s", opname.data());
		}
	}

	constexpr
	void add_operator_token(Token&& token) noexcept
	{
		if (m_operator_stack.empty()) {
			m_operator_stack.push_back(std::move(token)); return;
		}

		auto& last_token = m_operator_stack.back();

		if (last_token.type() == Token::Type::Bracket) 
		{
			m_operator_stack.push_back(std::move(token)); return;
		}

		auto find_res = this->find_operator(token.str());
		auto back_find_res = this->find_operator(last_token.str());

		auto first_associativity = find_res.associativity;

		auto first_precedence = find_res.precedence;
		auto second_precedence = back_find_res.precedence;

		if (first_associativity != Operator_associativity::Right 
			&& first_precedence == second_precedence)
		{
			m_output_stack.push_back(m_operator_stack.pop_back());
			m_operator_stack.push_back(std::move(token));
			return;
		}

		if (first_precedence < second_precedence) 
		{
			m_output_stack.push_back(m_operator_stack.pop_back());
		}
		m_operator_stack.push_back(std::move(token));
	}

	constexpr
	void add_bracket_token(Token&& token) noexcept
	{
		if (token.str() == string_view_type("(")) {
			m_operator_stack.push_back(std::move(token)); return;
		}

		for (auto it = m_operator_stack.rbegin(); it->type() != Token::Type::Bracket; ++it)
		{
			AML_DEBUG_VERIFY(it != m_operator_stack.rend(), "Can't find a opening bracket");
			m_output_stack.push_back(m_operator_stack.pop_back());
		}
		m_operator_stack.pop_back(); // pop )
	}

	constexpr
	void process_operator_call(operator_args_type& stack, const Token& optoken) const noexcept
	{
		const auto& op_sign = this->find_operator(optoken.str());
		const auto& op_invoke_func = op_sign.function;

		const auto& args_count = op_sign.argc;

		auto&& result = op_invoke_func(&stack.back() - (args_count - 1));

		stack.resize(stack.size() - args_count);
		stack.push_back(std::move(result));
	}

	constexpr
	void clear() noexcept {
		m_operator_stack.clear();
		m_output_stack.clear();
	}

public:

	AML_MSVC_CONSTEVAL
	Expression(const char(&str)[StringSize]) noexcept 
	{
		this->generate(str);
	}

	constexpr
	Expression(const Expression& other) noexcept 
		: m_output_stack(other.m_output_stack) {}
	constexpr
	Expression& operator=(const Expression& other) noexcept {
		m_output_stack = other.m_output_stack;
		return *this;
	}

	constexpr
	const stack_type& generate(const char(&str)[StringSize]) noexcept
	{
		this->clear();

		for (auto it = std::begin(str); *it != '\0';)
		{
			while (*it == ' ') ++it;

			auto [token, token_lenght] = make_token(it);
			it += token_lenght;
			
			switch (token.type())
			{
			case Token::Type::Bracket:
				this->add_bracket_token(std::move(token)); break;

			case Token::Type::Number:
			case Token::Type::Variable:
				m_output_stack.push_back(std::move(token)); break;

			case Token::Type::Operator:
				this->add_operator_token(std::move(token)); break;

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
			switch (token.type())
			{
			case Token::Type::Number:
				number_stack.push_back(token.number()); break;

			case Token::Type::Operator:
				this->process_operator_call(number_stack, token); break;

			case Token::Type::Variable:
				AML_DEBUG_ERROR("Using an unset variable");

			default:
				AML_DEBUG_ERROR("Unknown token type");
			}
		}

		AML_DEBUG_VERIFY(number_stack.size() == 1, "Stack must has one value");

		return number_stack.pop_back();
	}

	constexpr
	auto& set_var(const string_view_type varname, number_type value) noexcept
	{
		for (auto& token : m_output_stack) 
		{
			if (token.type() != Token::Type::Variable 
			  || token.str() != varname) continue;

			token = Token{value};
		}
		return *this;
	}

	template<class... Strings> constexpr
	decltype(auto) function(const Strings&... strs) const noexcept
	{
		return [&](auto... vals) {
			Expression out{*this};
			((out.set_var({ strs, std::size(strs) - 1 }, vals)), ...);
			return out.calculate();
		};
	}
	template<> constexpr
	decltype(auto) function() const noexcept {
		return this->function("x");
	}


private:
	stack_type m_output_stack;
	stack_type m_operator_stack;
};

template<std::size_t N>
Expression(const char(&)[N]) -> Expression<N>;

}