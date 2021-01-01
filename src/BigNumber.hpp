#ifndef PYTHON_INTERPRETER_BIGNUMBER_HPP
#define PYTHON_INTERPRETER_BIGNUMBER_HPP

#include <vector>
#include <cassert>
#include <sstream>
#include <algorithm>
using namespace std;

class BigNumber{
	friend bool operator==(const BigNumber &lhs, const BigNumber &rhs){
		if (lhs.empty() && rhs.empty()) return true;
		if (lhs.sign == rhs.sign && lhs.digit.size() == rhs.digit.size()){
			for (int i = 0; i < lhs.digit.size(); ++i)
				if (lhs.digit[i] != rhs.digit[i]) return false;
			return true;
		} else return false;
	}

	friend bool operator!=(const BigNumber &lhs, const BigNumber &rhs){
		return !(lhs == rhs);
	}

	friend bool operator<(const BigNumber &lhs, const BigNumber &rhs){
		if (lhs == rhs || lhs.sign && !rhs.sign) return false;
		if (!lhs.sign && rhs.sign) return true;
		int ls = lhs.digit.size(), rs = rhs.digit.size();
		if (lhs.sign){
			if (ls < rs) return true;
			if (ls > rs) return false;
			for (int i = ls - 1; i >= 0; --i){
				if (lhs.digit[i] < rhs.digit[i]) return true;
				if (lhs.digit[i] > rhs.digit[i]) return false;
			}
		} else {
			if (ls < rs) return false;
			if (ls > rs) return true;
			for (int i = ls - 1; i >= 0; --i){
				if (lhs.digit[i] < rhs.digit[i]) return false;
				if (lhs.digit[i] > rhs.digit[i]) return true;
			}
		}
		return false;
	}

	friend bool operator>(const BigNumber &lhs, const BigNumber &rhs){
		return rhs < lhs;
	}

	friend bool operator<=(const BigNumber &lhs, const BigNumber &rhs){
		return lhs == rhs || lhs < rhs;
	}

	friend bool operator>=(const BigNumber &lhs, const BigNumber &rhs){
		return lhs == rhs || lhs > rhs;
	}

	friend BigNumber abs(const BigNumber &x){
		return BigNumber(true, x.digit);
	}

	friend BigNumber operator+(const BigNumber &lhs, const BigNumber &rhs){
		vector<int> cur;
		for (int i = 0; i < max(lhs.digit.size(), rhs.digit.size()); ++i){
			int s = 0;
			if (i < lhs.digit.size()) 
				s += (lhs.sign ? 1 : -1) * lhs.digit[i];
			if (i < rhs.digit.size()) 
				s += (rhs.sign ? 1 : -1) * rhs.digit[i];
			cur.push_back(s);
		}
		int len = cur.size();
		for (int i = 0; i < len; ++i){
			if (cur[i] >= 10){
				if (i + 1 >= len) cur.push_back(cur[i] / 10);
				else cur[i+1] += cur[i] / 10;
				cur[i] %= 10;
			} else if (cur[i] < 0){
				int carrier = (-cur[i] - 1) / 10 + 1;
				if (i + 1 >= len) cur.push_back(-carrier);
				else cur[i+1] -= carrier;
				cur[i] += carrier * 10;
			}
		}
		int sign_ = true;
		if (*(cur.end() - 1) < 0) {
			for (auto &iter : cur) iter *= -1;
			for (int i = 0; i < cur.size() - 1; ++i) {
				if (cur[i] < 0) {
					int carrier = (-cur[i] - 1) / 10 + 1;
					cur[i + 1] -= carrier;
					cur[i] += carrier * 10;
				}
			}
			sign_ = false;
		}
		while (cur.size() > 1 && *(cur.end() - 1) == 0) cur.erase(cur.end() - 1);
		return BigNumber(sign_, cur);
	}

	friend BigNumber operator-(const BigNumber &lhs, const BigNumber &rhs){
		return lhs + BigNumber(!rhs.sign, rhs.digit);
	}

	friend BigNumber operator*(const BigNumber &lhs, const BigNumber &rhs){
		vector<int> cur;
		cur.resize(lhs.digit.size() + rhs.digit.size() - 1);
		for (int i = 0; i < lhs.digit.size(); ++i)
			for (int j = 0; j < rhs.digit.size(); ++j)
				cur[i+j] += lhs.digit[i] * rhs.digit[j];
		int len = cur.size();
		for (int i = 0; i < len; ++i){
			if (cur[i] >= 10){
				if (i + 1 >= len) cur.push_back(cur[i] / 10);
				else cur[i+1] += cur[i] / 10;
				cur[i] %= 10;
			}
		}
		while (cur.size() > 1 && *(cur.end() - 1) == 0) cur.erase(cur.end() - 1);
		return BigNumber(!(lhs.sign ^ rhs.sign), cur);
	}

	friend BigNumber operator/(const BigNumber &lhs, const BigNumber &rhs){
		assert(!rhs.empty());
		vector<int> cur;
		BigNumber remains, arhs = abs(rhs);
		for (int i = (int)lhs.digit.size() - 1; i >= 0; --i){
			if (remains.empty()) remains.digit[0] = lhs.digit[i];
			else {
				remains.digit.push_back(*(remains.digit.end() - 1));
				for (int j = (int)remains.digit.size() - 2; j >= 1; --j)
					remains.digit[j] = remains.digit[j-1];
				remains.digit[0] = lhs.digit[i];
			}
			int attempt = 0;
			for (; remains >= arhs; remains -= arhs, ++attempt) ;
			cur.push_back(attempt);
		}
		reverse(cur.begin(), cur.end());
		while (cur.size() > 1 && *(cur.end() - 1) == 0) cur.erase(cur.end() - 1);
		int sign_ = !(lhs.sign ^ rhs.sign);
		if (!sign_ && !remains.empty())
			return BigNumber(sign_, cur) - 1;
		else return BigNumber(sign_, cur);
	}

	friend BigNumber operator%(const BigNumber &lhs, const BigNumber &rhs){
		return lhs - lhs / rhs * rhs;
	}

private:
	//sign = 1 : positive & sign = 0 : negative
	bool sign;
	vector<int> digit;

public:
	BigNumber() : sign(true){
		digit.clear();
		digit.push_back(0);
	}

	BigNumber(bool sign_, const vector<int> &digit_) : sign(sign_), digit(digit_) {}

	explicit BigNumber(const string &str) : sign(true){
		int len = str.length();
		for (int i = len - 1; i >= 0; --i)
			if (isdigit(str[i])) digit.push_back(str[i] - '0');
			else if (str[i] == '-') sign = false;
        while (digit.size() > 1 && *(digit.end() - 1) == 0)
            digit.erase(digit.end() - 1);
	}

	BigNumber(long long num) : sign(num >= 0){
		digit.clear();
		if (!num) digit.push_back(0);
		for (num = abs(num); num; num /= 10) 
			digit.push_back(num % 10);
	}

	bool empty() const{return digit.size() == 1 && digit[0] == 0;}

	BigNumber &operator+=(const BigNumber &other){
		return *this = *this + other;
	}

	BigNumber &operator-=(const BigNumber &other){
		return *this = *this - other;
	}

	BigNumber &operator*=(const BigNumber &other){
		return *this = *this * other;
	}

	BigNumber &operator/=(const BigNumber &other){
		return *this = *this / other;
	}

    BigNumber &operator%=(const BigNumber &other){
        return *this = *this % other;
    }

	BigNumber &operator++(){
		return *this += 1;
	}

	BigNumber operator++(int){
		BigNumber tmp = *this;
		*this = *this + 1;
		return tmp;
	}

	BigNumber &operator--(){
		return *this -= 1;
	}

	BigNumber operator--(int){
		BigNumber tmp = *this;
		*this = *this - 1;
		return tmp;
	}

	explicit operator string() const{
		if (empty()){
			return "0";
		} else {
			string ret;
			if (!sign) ret += '-';
			for (int i = (int)digit.size() - 1; i >= 0; --i)
				ret += char(digit[i] + '0');
			return ret;
		}
	}

	explicit operator double() const{
	    double ret = 0;
	    for (int i = (int)digit.size() - 1; i >= 0; --i)
	        ret = ret * 10 + digit[i];
	    if (!sign) ret *= -1;
	    return ret;
	}

};

#endif //PYTHON_INTERPRETER_BIGNUMBER_HPP
