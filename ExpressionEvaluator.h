// ExpressionEvaluator
// Created by kiki on 2020/8/18.8:53
#pragma once

#include <string>
#include <string_view>
using std::string, std::string_view;
#include <stack>
#include <unordered_map>
using std::stack, std::unordered_map, std::pair;
#include <exception>
using std::exception;
#include <optional>
using std::optional, std::nullopt;

class ExpressionEvaluator
{
private:
	static const unordered_map<char, short> priority; // 运算符优先级
	stack< pair<char, size_t> > operator_stack; // 存放运算符和运算符在表达式中的下标
	stack<double> operand_stack; // 存放操作数

private:
	void pre_process(string& expression); // 预处理用户输入的表达式
    string preprocessed_expression; // 预处理后的表达式
public:
    string_view get_preprocessed_expression() const { return preprocessed_expression; }

private:
	static string_view get_char_type(char c);
	static double compute(double left, double right, char binary_operator, size_t index); // index为运算符在表达式中的下标
	static double compute(double num, char unary_operator, size_t index);
	void evaluate_unary(char unary_operator, size_t index);
	void evaluate_binary(char binary_operator, size_t index);
public:
    double evaluate(string expression); // 返回计算结果。该函数会修改传入的表达式，因此以值传递
};

/*输入错误的表达式时，抛出此异常*/
class ExpressionException : public exception
{
private:
    string message;
    optional<size_t> err_index;
public:
    ExpressionException(string m) : message(move(m)), err_index(nullopt) { }
    ExpressionException(string m, size_t e) : message(move(m)), err_index(e) { }
    [[nodiscard]] const char* what() const noexcept override { return message.data(); }
    [[nodiscard]] optional<size_t> error_index() const noexcept { return err_index; }
};

extern ExpressionEvaluator evaluator;
