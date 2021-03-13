#ifndef BK_ARITHMETIC_H
#define BK_ARITHMETIC_H

#include <stack>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <stdlib.h>
#include <vector>

class BkArithmetic
{
public:
	void Split(std::vector<CStringA>& ret, CStringA &str,const CStringA find, int limit=0)
	{
		int start = 0;
		int pos = str.Find(find,start);
		int len = find.GetLength();
		int i = 0;
		while(true){
			if(pos<0 || (limit>0 && i+1==limit)){ //NO FIND   
				ret.push_back(str.Mid(start));
				break;
			}else{
				ret.push_back(str.Mid(start,pos-start));
				start = pos+len;
				pos = str.Find(find,start);
			}
			i++;
		}
	}


	bool IsDigit(char c)
	{
		int a = c - '0';
		if(a >= 0 && a <= 9)
		{
			return true;
		}
		else
		{
			return false;
		}
	}


	bool IsDigitStr(char* c)
	{
		int size = strlen(c);
		for (int i = 0; i < size; i++)
		{
			if (!IsDigit(c[i]) && c[i] != '.')
			{
				return FALSE;
			}
		}
		return TRUE;
	}


	bool IsSpace(char c)
	{
		if( c == ' '){
			return true;
		}else{
			return false;
		}
	}

	bool IsBracket(char c)
	{
		if( c == '(' || c == ')'){
			return true;
		}else{
			return false;
		}
	}

	bool IsOperator(char c)
	{
		if( c == '+' || c == '-' || c == '*' || c == '/'){
			return true;
		}else{
			return false;
		}
	}

	double StringToArithmetic(char* str)
	{
		double result = SuffixToArithmetic(InfixToSuffix(str));
		return result;
	}

	int StringToArithmeticInt(char* str)
	{
		double result = SuffixToArithmetic(InfixToSuffix(str));
		return int(result+0.5);
	}

	double SuffixToArithmetic(CStringA postfix)
	{
		std::vector<CStringA> strvec;
		std::stack<double> stack;
		Split(strvec, postfix, " ");

		std::vector<CStringA>::iterator str;

		for (str = strvec.begin();
			str != strvec.end();
			str++)
		{
			//string = string.trim();
			//if (string.equals("")) {
			//	continue;
			//}
			if (IsDigitStr(str->GetBuffer())) {

				stack.push(StrToIntA(str->GetBuffer()));
			} else {

				double y = stack.top();
				stack.pop();
				double x = stack.top();
				stack.pop();
				stack.push(Caculate(x, y, str->GetAt(0)));
			}
		}
		double retv = stack.top();
		stack.pop();
		return retv;
	}

	double Caculate(double x, double y, char simble)
	{
		if (simble == '+') {
			return x + y;
		}
		if (simble == '-') {
			return x - y;
		}
		if (simble == '*') {
			return x * y;
		}
		if (simble == '/') {
			return x / y;
		}
		return 0;
	}

	CStringA InfixToSuffix(char* infix)
	{
		std::stack<char> stack;
		CStringA suffix;

		int length = strlen(infix);
		for (int i = 0; i < length; i++) {
			char temp;
			char c = (char) infix[i];

			if (!IsDigit(c) && !IsOperator(c) && !IsSpace(c) && !IsBracket(c))
			{
				break;
			}

			switch (c) {
				case ' ':
					break;
				case '(':
					stack.push(c);
					break;
				case '+':
				case '-':
					while (stack.size() != 0) {
						temp = stack.top();
						stack.pop();
						if (temp == '(') {
							stack.push('(');
							break;
						}
						suffix.AppendChar(' ');
						suffix.AppendChar(temp);
					}
					stack.push(c);
					suffix.AppendChar(' ');
					break;
				case '*':
				case '/':
					while (stack.size() != 0) {
						temp = stack.top();
						stack.pop();
						if (temp == '(' || temp == '+' || temp == '-') {
							stack.push(temp);
							break;
						} else {
							suffix.AppendChar(' ');
							suffix.AppendChar(temp);
						}
					}
					stack.push(c);
					suffix.AppendChar(' ');
					break;

				case ')':
					while (stack.size() != 0) {
						temp = stack.top();
						stack.pop();
						if (temp == '(') {
							break;
						} else {
							suffix.AppendChar(' ');
							suffix.AppendChar(temp);
						}
					}
					break;
				default:
					suffix.AppendChar(c);
			}
		}
		while (stack.size() != 0) {
			suffix.AppendChar(' ');
			suffix.AppendChar(stack.top());
			stack.pop();
		}
		return suffix;
	}
};

#endif // BK_ARITHMETIC_H
