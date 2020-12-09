#ifndef PYTHON_INTERPRETER_BIGNUMBER_HPP
#define PYTHON_INTERPRETER_BIGNUMBER_HPP

#include <vector>
using namespace std;

class BigNumber{
	friend bool operator==(const BigNumber &lhs, const BigNumber &rhs){
		if (lhs.empty() && rhs.empty()) return true;
		bool ret = lhs.sign == rhs.sign && lhs.digit.size() == rhs.digit.size();
		if (ret){
			for (int i = 0; i < lhs.digit.size(); ++i)
				ret &= lhs.digit[i] == rhs.digit[i];
		}
		return ret;
	}

	friend bool operator!=(const BigNumber &lhs, const BigNumber &rhs){
		return !(lhs == rhs);
	}

	friend bool operator<(const BigNumber &lhs, const BigNumber &rhs){
		return lhs != rhs && (lhs - rhs).sign == false;
	}

	friend bool operator>(const BigNumber &lhs, const BigNumber &rhs){
		return lhs != rhs && (lhs - rhs).sign == true;
	}

	friend bool operator<=(const BigNumber &lhs, const BigNumber &rhs){
		return lhs == rhs || (lhs - rhs).sign == false;
	}

	friend bool operator>=(const BigNumber &lhs, const BigNumber &rhs){
		return lhs == rhs || (lhs - rhs).sign == true;
	}

	friend BigNumber abs(BigNumber x){
		x.sign = true;
		return x;
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
		if (cur.size() > 0 && *(cur.end() - 1) < 0){
			for (int i = 0; i < cur.size(); ++i)
				cur[i] *= -1;
			for (int i = 0; i < cur.size() - 1; ++i){
				if (cur[i] < 0){
					int carrier = (-cur[i] - 1) / 10 + 1;
					cur[i+1] -= carrier;
					cur[i] += carrier * 10;
				}
			}
			while (cur.size() > 1 && *(cur.end() - 1) == 0) cur.erase(cur.end() - 1);
			return BigNumber(false, cur);
		} else {
			while (cur.size() > 1 && *(cur.end() - 1) == 0) cur.erase(cur.end() - 1);
			return BigNumber(true, cur);
		}
	}

	friend BigNumber operator-(const BigNumber &lhs, BigNumber rhs){
		rhs.sign ^= 1;
		return lhs + rhs;
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
		return BigNumber(lhs.sign ^ rhs.sign ^ 1, cur);
	}

	friend BigNumber operator/(const BigNumber &lhs, const BigNumber &rhs){
		assert(!rhs.empty());
		vector<int> cur;
		BigNumber remains, arhs = abs(rhs);
		for (int i = lhs.digit.size() - 1; i >= 0; --i){
			remains = remains * 10 + lhs.digit[i];
			int attempt = 0;
			for (; attempt < 10 && remains >= arhs * attempt; ++attempt) ;
			attempt--;
			cur.push_back(attempt);
			remains -= arhs * attempt;
		}
		reverse(cur.begin(), cur.end());
		while (cur.size() > 1 && *(cur.end() - 1) == 0) cur.erase(cur.end() - 1);
		if ((lhs.sign ^ rhs.sign ^ 1) == false && !remains.empty())
			return BigNumber(lhs.sign ^ rhs.sign ^ 1, cur) - 1;
		else return BigNumber(lhs.sign ^ rhs.sign ^ 1, cur);
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

	BigNumber(bool sign_, const vector<int> &digit_) : sign(sign_){
		digit.clear();
		for (int i = 0; i < digit_.size(); ++i)
			digit.push_back(digit_[i]);
	}

	BigNumber(string str) : sign(true){
		int len = str.length();
		for (int i = len - 1; i >= 0; --i)
			if (isdigit(str[i])) digit.push_back(str[i] - '0');
			else if (str[i] == '-') sign = false;
	}

	BigNumber(int num) : sign(num >= 0){
		digit.clear();
		if (!num) digit.push_back(0);
		for (num = abs(num); num; num /= 10) 
			digit.push_back(num % 10);
	}

	bool empty() const{return digit.size() == 1 && digit[0] == 0;}

	BigNumber &operator=(const BigNumber &other){
		if (this == &other) return *this;
		sign = other.sign;
		digit.clear();
		for (int i = 0; i < other.digit.size(); ++i)
			digit.push_back(other.digit[i]);
		return *this;
	}

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

	BigNumber operator++(int x){
		BigNumber tmp = *this;
		*this = *this + 1;
		return tmp;
	}

	BigNumber &operator--(){
		return *this -= 1;
	}

	BigNumber operator--(int x){
		BigNumber tmp = *this;
		*this = *this + 1;
		return tmp;
	}

	string put2string() const{
		if (digit.empty()){
			return "0";
		} else {
			string ret;
			if (!sign) ret += '-';
			for (int i = (int)digit.size() - 1; i >= 0; --i)
				ret += digit[i] + '0';
			return ret;
		}
	}

	double put2double() const{
        stringstream ss(this->put2string());
        double ret;
        ss >> ret;
        return ret;
	}

};

#endif //PYTHON_INTERPRETER_BIGNUMBER_HPP
