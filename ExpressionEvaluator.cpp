// ExpressionEvaluator
// Created by kiki on 2020/8/18.8:53
#include "ExpressionEvaluator.h"

#include <cmath>
using std::sin, std::cos, std::tan;
#include <cstring>
using std::strchr;
#include <algorithm>
using std::transform;
#include <sstream>
using std::stringstream;

const unordered_map<char, short> ExpressionEvaluator::priority
{
	{'+', 2},{'-', 2},
	{'*', 3},{'/', 3},
	{'%', 3},{'^', 4},
	{'(', 0},{')', 5}
};

/*
表达式预处理：
将表达式中的所有字母转换为小写字母
去除表达式中的所有空白
移除表达式结尾可能存在的=
替换特殊字符
*/
// fixme:效率！！！遍历一次字符串完成全部操作
// std::tolower函数存在两个重载，第一个定义于头文件cctype,第二个定义于头文件locale
// 在全局名称空间中，tolower只有一种声明形式, 因此使用::tolower可避免冲突
void ExpressionEvaluator::pre_process(string& expression)
{
    transform(expression.begin(), expression.end(), expression.begin(), ::tolower);

    // 移除全部空格
	auto position = expression.find(' ');
	while(position != string::npos)
	{
		expression.erase(position, 1);
		position = expression.find(' ', position);
	}

    // 如果表达式结尾有= , 则移除
	expression.erase(remove(expression.end() - 1, expression.end(), '='), expression.end());

    // 替换特殊字符
	auto replace_all =
	[](string& expression, string_view old_str, string_view new_str)
	{
		auto position = expression.find(old_str);
		while(position != string::npos)
		{
			expression.replace(position, old_str.length(), new_str);
			position = expression.find(old_str, position);
		}
	};
	replace_all(expression, "x", "*");
	replace_all(expression, "sin", "s");
	replace_all(expression, "cos", "c");
	replace_all(expression, "tan", "t");
	replace_all(expression, "ln", "n");
	replace_all(expression, "lg", "g");
	replace_all(expression, "sqrt", "q");
    replace_all(expression, "pi", "p");

    if(expression.empty()){ throw ExpressionException("您输入了空的表达式"); }

    this->preprocessed_expression = move(expression);
}


/*
给定两个操作数left, right和二元运算符binary_operator，求其值；
index为运算符在表达式中的下标；
当操作数不合法时，抛出异常，index指示发生错误的位置。
*/
double ExpressionEvaluator::compute(double left, double right, char binary_operator, size_t prev_operator_index)
{
	switch(binary_operator)
	{
		case '+':
			return left + right; // fixme: 溢出处理
		case '-':
			return left - right;
		case '*':
			return left * right;
		case '/':
			if(right == 0.0)
				{throw ExpressionException("/运算符的右操作数为0.0", prev_operator_index);}
			return left / right;
		case '%':
            if(static_cast<int>(right) == 0)
				{ throw ExpressionException("%运算符的操作数非法", prev_operator_index); }
			return static_cast<double>( static_cast<int>(left) % static_cast<int>(right) );
		case '^':
		{ // 在case中声明变量时，应当加{}
			double result = pow(left, right);
            if(!std::isnormal(result) && result != 0.0) // 判断结果是否为nan或inf
			{throw ExpressionException("pow的运算结果溢出或非法", prev_operator_index);}
			return result;
		}
		default:
			throw ExpressionException("未知的二元运算符", prev_operator_index);
	}
}
/* 给定操作数num和一元运算符unary_operator，求其值 */
double ExpressionEvaluator::compute(double num, char unary_operator, size_t prev_operator_index)
{
	switch(unary_operator)
	{
		case '!':
		{
			double result = std::tgamma(num + 1);
            if (!std::isnormal(result) && result != 0.0)
				{ throw ExpressionException("!运算符的操作数非法", prev_operator_index); }
			return result;
		}
		case 's':
			return sin(num);
		case 'c':
			return cos(num);
		case 't':
			return tan(num);
		case 'n':
		{
			double result = log(num);
            if (!std::isnormal(result) && result != 0.0)
                { throw ExpressionException("ln运算符的操作数非法", prev_operator_index); }
			return result;
		}
		case 'g':
		{
			double result = log(num)/log(10.0);
            if (!std::isnormal(result) && result != 0.0)
                { throw ExpressionException("log运算符的操作数非法", prev_operator_index); }
			return result;
		}
		case 'q':
		{
			double result = sqrt(num);
            if (!std::isnormal(result) && result != 0.0)
				{ throw ExpressionException("sqrt运算符的操作数非法", prev_operator_index); }
			return result;
		}
		case '~':
			return -num;
		default:
			throw ExpressionException("未知的一元运算符", prev_operator_index);
	}
}

string_view ExpressionEvaluator::get_char_type(char c)
{
	char binary_operator[] = "+, -, *, /, %, ^";
	char unary_needs_right[] = "s, c, t, g, n, ~, q"; // sin cos tan lg ln negative sqrt
	char unary_needs_left[] = "!";

	if( ( c >= '0' && c <= '9') || c == '.' ){ return "is_digital"; }
    else if (c == 'e' || c== 'p') { return "is_constant"; }
	else if(c == '(' || c == ')'){ return "is_brackets"; }
	else if( strchr(binary_operator, c) ){ return "is_binary_operator"; }
	else if( strchr(unary_needs_right, c) ){ return "unary_needs_right"; }
	else if( strchr(unary_needs_left, c) ){ return "unary_needs_left"; }
	else { return "cannot_find"; }
}

void ExpressionEvaluator::evaluate_unary(char unary_operator, size_t index)
{
    if(operand_stack.empty())
        { throw ExpressionException(string("一元运算符")+preprocessed_expression[index]+"缺失操作数", index); }

	double top_copy = operand_stack.top(); operand_stack.pop();
	operand_stack.push(compute(top_copy, unary_operator, index));
}
void ExpressionEvaluator::evaluate_binary(char binary_operator, size_t index)
{
    if(operand_stack.size() < 2)
        { throw ExpressionException(string("二元运算符")+preprocessed_expression[index]+"缺失操作数", index); }

	double right_operand = operand_stack.top(); operand_stack.pop();
	double left_operand = operand_stack.top(); operand_stack.pop();

	operand_stack.push( compute(left_operand, right_operand, binary_operator, index) );
}

double ExpressionEvaluator::evaluate(string expression)
{
	operand_stack = stack<double> (); // 清空双栈
	operator_stack = stack< pair<char, size_t> > ();

	pre_process(expression);

    auto str_size = preprocessed_expression.size();
	for(size_t i = 0; i < str_size; ++i)
	{
        char current_char = preprocessed_expression[i];
		string_view char_type = get_char_type(current_char);

		if(char_type == "cannot_find") { throw ExpressionException("未知的字符", i); }
		else if(char_type == "is_digital")
		{
			int decimal_point_num = 0; // 小数点个数
			if(current_char == '.'){++ decimal_point_num;}

			size_t j = i;
            while(get_char_type(preprocessed_expression[j + 1]) == "is_digital")
			{
				++j;
                if(preprocessed_expression[j] == '.'){++ decimal_point_num;}
			}

			if(decimal_point_num > 1)
				{ throw ExpressionException("操作数中有多个小数点", i); }
            if( (i != 0 && (get_char_type(preprocessed_expression[i - 1]) == "unary_needs_left" || preprocessed_expression[i - 1] == ')')) || get_char_type(preprocessed_expression[j + 1]) == "unary_needs_right"|| preprocessed_expression[j + 1] == '(')
				{ throw ExpressionException("操作数不能出现在)或需要左操作数的一元运算符后", i); }

            stringstream iss(preprocessed_expression.substr(i, j - i + 1));
			double n; iss >> n;
			operand_stack.push(n);

			i = j + 1;
			--i; // for循环会将i加1
		}
        else if (char_type == "is_constant")
        {
            if (i > 0 && (get_char_type(preprocessed_expression[i - 1]) == "is_digital" || get_char_type(preprocessed_expression[i - 1]) == "is_constant") )
                { throw ExpressionException("常量前的乘号不能省略", i); }
            if (i < str_size - 1 && (get_char_type(preprocessed_expression[i + 1]) == "is_digital" || get_char_type(preprocessed_expression[i - 1]) == "is_constant") )
                { throw ExpressionException("常量后不能出现数字", i); }

            current_char == 'e' ? operand_stack.push(2.718282) : operand_stack.push(3.141592653);
        }
		else if(char_type == "unary_needs_left") { evaluate_unary(current_char, i); }
		else if(char_type == "unary_needs_right") { operator_stack.emplace(current_char, i); }
        else if(current_char == '-' && (i == 0 || get_char_type(preprocessed_expression[i - 1]) == "is_binary_operator" || get_char_type(preprocessed_expression[i - 1]) == "unary_needs_right" ||preprocessed_expression[i - 1] == '(') )
		{
			operator_stack.emplace('~', i);
		}
		else if(char_type == "is_binary_operator")
		{
			while(!operator_stack.empty())
			{
				char prev_operator = operator_stack.top().first;
				size_t prev_operator_index = operator_stack.top().second;
				string_view prev_operator_type = get_char_type(prev_operator);

				if(prev_operator_type == "unary_needs_right")
					{ evaluate_unary(prev_operator, prev_operator_index); }
				else if(prev_operator_type == "is_binary_operator" && (priority.at(current_char) <= priority.at(prev_operator)) )
					{ evaluate_binary(prev_operator, prev_operator_index); }
				else{ break; }

				operator_stack.pop();
			}

			operator_stack.emplace(current_char, i);
		}
		else if(current_char == '(') { operator_stack.emplace(current_char, i); }
		else if(current_char == ')')
		{
			if(operator_stack.empty()){ throw ExpressionException(")缺失与之对应的(", i); }
            if(i != preprocessed_expression.size() - 1 && preprocessed_expression[i+1] == '('){ throw ExpressionException(")与(之间缺失运算符", i); }

			char prev_operator = operator_stack.top().first;
			unsigned int prev_operator_index = operator_stack.top().second;
			string_view prev_operator_type = get_char_type(prev_operator);

			while(prev_operator != '(')
			{
				if( prev_operator_type == "unary_needs_right")
					{ evaluate_unary(prev_operator, prev_operator_index); }
				else if(prev_operator_type == "is_binary_operator")
					{ evaluate_binary(prev_operator, prev_operator_index); }

                operator_stack.pop(); // fixme: what?

				if(operator_stack.empty()){ throw ExpressionException(")缺失与之对应的(", i); }

				prev_operator = operator_stack.top().first;
				prev_operator_index = operator_stack.top().second;
				prev_operator_type = get_char_type(prev_operator);
			}

			operator_stack.pop(); // 运算符栈中弹出(
		}
	}

	while (!operator_stack.empty())
	{
		char operators = operator_stack.top().first;
		unsigned int index = operator_stack.top().second;

		if(operators == '(') { throw ExpressionException("(缺失与之对应的)", index); }
		else if (get_char_type(operators) == "unary_needs_right")
			{ evaluate_unary(operators, index); }
		else
			{ evaluate_binary(operators, index); }

		operator_stack.pop();
	}

    if(operand_stack.empty()){ throw ExpressionException("操作数栈为空"); }
    if(operand_stack.size() > 1){ throw ExpressionException("操作数栈中有多个元素");}

    return operand_stack.top();
}

ExpressionEvaluator evaluator;
