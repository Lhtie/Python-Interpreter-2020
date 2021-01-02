#ifndef PYTHON_INTERPRETER_AnyType_HPP
#define PYTHON_INTERPRETER_AnyType_HPP

#include "BigNumber.hpp"
#include <string>
#include <iomanip>
#include <cmath>
using namespace std;

class AnyType;
enum Type{INT, FLOAT, BOOL, STR, NONE, BREAK, CONTINUE, RETURN};
const double eps = 1e-8;

class AnyType{
	friend AnyType operator+(const AnyType &lhs, const AnyType &rhs){
		if (lhs.type_name == STR && rhs.type_name == STR){
			return AnyType(lhs.str_type + rhs.str_type);
		}
		AnyType l = lhs, r = rhs;
		convert(l, r);
		if (l.type_name == FLOAT) return AnyType(l.float_type + r.float_type);
		return AnyType(l.int_type + r.int_type);
	}

	friend AnyType operator-(const AnyType &lhs, const AnyType &rhs){
		AnyType l = lhs, r = rhs;
		convert(l, r);
		if (l.type_name == FLOAT) return AnyType(l.float_type - r.float_type);
		return AnyType(l.int_type - r.int_type);
	}

	friend AnyType operator-(const AnyType &x){
		return AnyType(0) - x;
	}

	friend AnyType operator*(const AnyType &lhs, const AnyType &rhs){
		AnyType l = lhs, r = rhs;
		if (l.type_name == STR || r.type_name == STR){
			if (r.type_name == STR) swap(l, r);
			if (r.type_name == BOOL) r = r.put2int();
			string ret;
			for (int i = 1; i <= r.int_type; ++i)
				ret += l.str_type;
			return AnyType(ret);
		}
		convert(l, r);
		if (l.type_name == FLOAT) return AnyType(l.float_type * r.float_type);
		return AnyType(l.int_type * r.int_type);
	}

	friend AnyType operator/(const AnyType &lhs, const AnyType &rhs){
		AnyType l = lhs, r = rhs;
		l = l.put2float(), r = r.put2float();
		return AnyType(l.float_type / r.float_type);
	}

	friend AnyType div(const AnyType &lhs, const AnyType &rhs){
		AnyType l = lhs, r = rhs;
		l = l.put2int(), r = r.put2int();
		return AnyType(l.int_type / r.int_type);
	}

	friend AnyType operator%(const AnyType &lhs, const AnyType &rhs){
		AnyType l = lhs, r = rhs;
		l = l.put2int(), r = r.put2int();
		return AnyType(l.int_type % r.int_type);
	}

	friend AnyType operator|(const AnyType &lhs, const AnyType &rhs){
		AnyType l = lhs, r = rhs;
		l = l.put2bool(), r = r.put2bool();
		return AnyType(l.bool_type || r.bool_type);
	}

	friend AnyType operator&(const AnyType &lhs, const AnyType &rhs){
		AnyType l = lhs, r = rhs;
		l = l.put2bool(), r = r.put2bool();
		return AnyType(l.bool_type && r.bool_type);
	}

	friend AnyType operator!(const AnyType &x){
		AnyType ret = x;
		ret = ret.put2bool();
		return AnyType(!ret.bool_type);
	}

	friend AnyType operator<(const AnyType &lhs, const AnyType &rhs){
		if (lhs.type_name == STR && rhs.type_name == STR){
			return AnyType(lhs.str_type < rhs.str_type);
		}
		AnyType l = lhs, r = rhs;
		convert(l, r);
		if (l.type_name == FLOAT) return AnyType(l.float_type < r.float_type);
		return AnyType(l.int_type < r.int_type);
	}

	friend AnyType operator>(const AnyType &lhs, const AnyType &rhs){
		return rhs < lhs;
	}

	friend AnyType operator==(const AnyType &lhs, const AnyType &rhs){
		if (lhs.type_name >= 3 || rhs.type_name >= 3){
			if (lhs.type_name != rhs.type_name) return AnyType(false);
			if (lhs.type_name == STR && rhs.type_name == STR)
				return AnyType(lhs.str_type == rhs.str_type);
			return AnyType(true);
		}
		AnyType l = lhs, r = rhs;
		convert(l, r);
		if (l.type_name == FLOAT) return AnyType(fabs(l.float_type - r.float_type) < eps);
		return AnyType(l.int_type == r.int_type);
	}

	friend bool operator==(const AnyType &lhs, Type rhs){
		return lhs.type_name == rhs;
	}

	friend AnyType operator>=(const AnyType &lhs, const AnyType &rhs){
		return lhs == rhs | lhs > rhs;
	}

	friend AnyType operator<=(const AnyType &lhs, const AnyType &rhs){
		return lhs == rhs | lhs < rhs;
	}

	friend AnyType operator!=(const AnyType &lhs, const AnyType &rhs){
		return !(lhs == rhs);
	}

	friend bool operator!=(const AnyType &lhs, Type rhs){
		return lhs.type_name != rhs;
	}

private:
    Type type_name;
    BigNumber int_type;
    double float_type = 0;
    bool bool_type = false;
    string str_type;

    static void convert(AnyType &lhs, AnyType &rhs){
		if (lhs.type_name == BOOL) lhs = lhs.put2int();
		if (rhs.type_name == BOOL) rhs = rhs.put2int();
		if (lhs.type_name == FLOAT || rhs.type_name == FLOAT){
			lhs = lhs.put2float(), rhs = rhs.put2float();
		}
    }

public:
    vector<AnyType> return_type;

    explicit AnyType(Type x = NONE) : type_name(x) {}
    explicit AnyType(const antlrcpp::Any &x){
        if (x.is<BigNumber>()) type_name = INT, int_type = x.as<BigNumber>();
        if (x.is<int>()) type_name = INT, int_type = x.as<int>();
        if (x.is<long long>()) type_name = INT, int_type = x.as<long long>();
        if (x.is<double>()) type_name = FLOAT, float_type = x.as<double>();
        if (x.is<bool>()) type_name = BOOL, bool_type = x.as<bool>();
        if (x.is<string>()) type_name = STR, str_type = x.as<string>();
        if (x.is<vector<AnyType> >()) type_name = RETURN, return_type = x.as<vector<AnyType> >();
    }
    AnyType(Type t, const antlrcpp::Any &x) : AnyType(x) {
    	type_name = t;
    }

    AnyType &operator+=(const AnyType &other){
        return *this = *this + other;
    }

    AnyType &operator-=(const AnyType &other){
        return *this = *this - other;
    }

    AnyType &operator*=(const AnyType &other){
        return *this = *this * other;
    }

    AnyType &operator/=(const AnyType &other){
        return *this = *this / other;
    }

    AnyType &operator%=(const AnyType &other){
        return *this = *this % other;
    }

    AnyType &operator|=(const AnyType &other){
        return *this = *this | other;
    }

    AnyType &operator&=(const AnyType &other){
        return *this = *this & other;
    }

    AnyType put2int() const{
        if (type_name == FLOAT) return AnyType(BigNumber((long long)float_type));
        if (type_name == BOOL) return AnyType((int)bool_type);
        if (type_name == STR) return AnyType(BigNumber(str_type));
        return *this;
    }

    AnyType put2float() const{
        if (type_name == INT) return AnyType((double)int_type);
        if (type_name == BOOL) return AnyType((double)bool_type);
        if (type_name == STR){
            string context = str_type;
            int len = context.length();
            double prv = 0, suf = 0;
            bool have_dot = false;
            for (int i = 0; i < len; ++i){
            	if (context[i] == '.') {have_dot = true; break;}
            }
            for (int i = (context[0] =='-' ? 1 : 0); i < len && context[i] != '.'; ++i)
                prv = prv * 10 + context[i] - '0';
            if (have_dot){
		        for (int i = len - 1; context[i] != '.'; --i)
		            suf = suf / 10 + context[i] - '0';
			}
            return AnyType((context[0] == '-' ? -1. : 1.) * (prv + suf / 10 + eps));
        }
        return *this;
    }

    AnyType put2str() const{
        if (type_name == INT) return AnyType((string)int_type);
        if (type_name == FLOAT){
            double cur = abs(float_type);
            string ret;
            auto prv = (long long)cur;
            double suf = cur - prv;
            if (!prv) ret = '0';
            for (; prv; prv /= 10)
                ret = char(prv % 10 + '0') + ret;
            ret += '.';
            int precision = 1;
            for (suf *= 10; abs(suf) > eps && precision <= 6; ++precision) {
                auto tmp = (long long)(suf + eps);
                ret += char(tmp + '0');
                suf = (suf - tmp) * 10;
            }
            for (; precision <= 6; ++precision) ret += '0';
            if (float_type < 0) ret = '-' + ret;
            return AnyType(ret);
        }
        if (type_name == BOOL) return AnyType(bool_type ? string("True") : string("False"));
        if (type_name == NONE) return AnyType(string("None"));
        return *this;
    }

    AnyType put2bool() const{
        if (type_name == INT) return AnyType(!int_type.empty());
        if (type_name == FLOAT) return AnyType((bool)float_type);
        if (type_name == STR) return AnyType(!str_type.empty());
        if (type_name == NONE) return AnyType(false);
        return *this;
    }

    operator bool() const{
    	AnyType x = this->put2bool();
    	return x.bool_type;
    }

    friend ostream &operator<<(ostream &os, const AnyType &x){
        if (x.type_name == INT) os << string(x.int_type);
        if (x.type_name == FLOAT) os << setiosflags(ios::fixed) << setprecision(6) << x.float_type;
        if (x.type_name == BOOL) os << (x.bool_type ? "True" : "False");
        if (x.type_name == STR) os << x.str_type;
        if (x.type_name == NONE) os << "None";
        return os;
    }

};

#endif //PYTHON_INTERPRETER_AnyType_HPP
