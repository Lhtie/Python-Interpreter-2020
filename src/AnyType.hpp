#ifndef PYTHON_INTERPRETER_AnyType_HPP
#define PYTHON_INTERPRETER_AnyType_HPP

#include "BigNumber.hpp"
#include <string>
#include <iomanip>
using namespace std;

enum Type{INT, FLOAT, BOOL, STR, NONE, BREAK, CONTINUE, RETURN};

class AnyType{
private:
    Type type_name;
    BigNumber int_type;
    double float_type;
    bool bool_type;
    string str_type;

public:
    vector<AnyType> return_type;

    AnyType() = default;
    AnyType(Type x) : type_name(x) {}
    AnyType(antlrcpp::Any x) : float_type(0), bool_type(false) {
        if (x.is<BigNumber>()) type_name = INT, int_type = x.as<BigNumber>();
        if (x.is<int>()) type_name = INT, int_type = x.as<int>();
        if (x.is<double>()) type_name = FLOAT, float_type = x.as<double>();
        if (x.is<bool>()) type_name = BOOL, bool_type = x.as<bool>();
        if (x.is<string>()) type_name = STR, str_type = x.as<string>();
    }
    AnyType(Type t, antlrcpp::Any x) : type_name(t){
        if (t == INT){
            if (x.is<int>()) int_type = BigNumber(x.as<int>());
            else int_type = BigNumber(x.as<BigNumber>());
        }
        if (t == FLOAT) float_type = x.as<double>();
        if (t == BOOL) bool_type = x.as<bool>();
        if (t == STR) str_type = x.as<string>();
        if (t == RETURN) return_type = x.as<vector<AnyType> >();
    }

    friend AnyType operator+(AnyType lhs, AnyType rhs){
        if (lhs.type_name == STR && rhs.type_name == STR){
            return AnyType(lhs.str_type + rhs.str_type);
        }
        if (lhs.type_name == BOOL) lhs.int_type = lhs.bool_type;
        if (rhs.type_name == BOOL) rhs.int_type = rhs.bool_type;
        if (lhs.type_name == FLOAT || rhs.type_name == FLOAT){
            if (lhs.type_name != FLOAT) lhs.float_type = lhs.int_type.put2double();
            if (rhs.type_name != FLOAT) rhs.float_type = rhs.int_type.put2double();
            return AnyType(lhs.float_type + rhs.float_type);
        }
        return AnyType(lhs.int_type + rhs.int_type);
    }

    friend AnyType operator-(AnyType lhs, AnyType rhs){
        if (lhs.type_name == BOOL) lhs.int_type = lhs.bool_type;
        if (rhs.type_name == BOOL) rhs.int_type = rhs.bool_type;
        if (lhs.type_name == FLOAT || rhs.type_name == FLOAT){
            if (lhs.type_name != FLOAT) lhs.float_type = lhs.int_type.put2double();
            if (rhs.type_name != FLOAT) rhs.float_type = rhs.int_type.put2double();
            return AnyType(lhs.float_type - rhs.float_type);
        }
        return AnyType(lhs.int_type - rhs.int_type);
    }

    friend AnyType operator*(AnyType lhs, AnyType rhs){
        if (lhs.type_name == STR || rhs.type_name == STR){
            if (rhs.type_name == STR) swap(lhs, rhs);
            if (rhs.type_name == BOOL) rhs.int_type = rhs.bool_type;
            string ret;
            for (int i = 1; i <= rhs.int_type; ++i)
                ret += lhs.str_type;
            return AnyType(ret);
        }
        if (lhs.type_name == BOOL) lhs.int_type = lhs.bool_type;
        if (rhs.type_name == BOOL) rhs.int_type = rhs.bool_type;
        if (lhs.type_name == FLOAT || rhs.type_name == FLOAT){
            if (lhs.type_name != FLOAT) lhs.float_type = lhs.int_type.put2double();
            if (rhs.type_name != FLOAT) rhs.float_type = rhs.int_type.put2double();
            return AnyType(lhs.float_type * rhs.float_type);
        }
        return AnyType(lhs.int_type * rhs.int_type);
    }

    friend AnyType operator/(AnyType lhs, AnyType rhs){
        if (lhs.type_name == BOOL) lhs.int_type = lhs.bool_type;
        if (rhs.type_name == BOOL) rhs.int_type = rhs.bool_type;
        if (lhs.type_name != FLOAT) lhs.float_type = lhs.int_type.put2double();
        if (rhs.type_name != FLOAT) rhs.float_type = rhs.int_type.put2double();
        return AnyType(lhs.float_type / rhs.float_type);
    }

    friend AnyType div(AnyType lhs, AnyType rhs){
        if (lhs.type_name == BOOL) lhs.int_type = lhs.bool_type;
        if (rhs.type_name == BOOL) rhs.int_type = rhs.bool_type;
        return AnyType(lhs.int_type / rhs.int_type);
    }

    friend AnyType operator%(AnyType lhs, AnyType rhs){
        if (lhs.type_name == BOOL) lhs.int_type = lhs.bool_type;
        if (rhs.type_name == BOOL) rhs.int_type = rhs.bool_type;
        return AnyType(lhs.int_type % rhs.int_type);
    }

    friend AnyType operator|(AnyType lhs, AnyType rhs){
        if (lhs.type_name != BOOL) lhs.bool_type = !lhs.int_type.empty() || !lhs.str_type.empty() || lhs.float_type;
        if (rhs.type_name != BOOL) rhs.bool_type = !rhs.int_type.empty() || !rhs.str_type.empty() || rhs.float_type;
        return AnyType(lhs.bool_type || rhs.bool_type);
    }

    friend AnyType operator&(AnyType lhs, AnyType rhs){
        if (lhs.type_name != BOOL) lhs.bool_type = !lhs.int_type.empty() || !lhs.str_type.empty() || lhs.float_type;
        if (rhs.type_name != BOOL) rhs.bool_type = !rhs.int_type.empty() || !rhs.str_type.empty() || rhs.float_type;
        return AnyType(lhs.bool_type && rhs.bool_type);
    }

    friend AnyType operator!(AnyType x){
        if (x.type_name != BOOL) x.bool_type = !x.int_type.empty() || !x.str_type.empty() || x.float_type;
        return AnyType(!x.bool_type);
    }

    friend bool operator<(AnyType lhs, AnyType rhs){
        if (lhs.type_name == STR && rhs.type_name == STR){
            return lhs.str_type < rhs.str_type;
        }
        if (lhs.type_name == BOOL) lhs.int_type = lhs.bool_type;
        if (rhs.type_name == BOOL) rhs.int_type = rhs.bool_type;
        if (lhs.type_name == FLOAT || rhs.type_name == FLOAT){
            if (lhs.type_name != FLOAT) lhs.float_type = lhs.int_type.put2double();
            if (rhs.type_name != FLOAT) rhs.float_type = rhs.int_type.put2double();
            return lhs.float_type < rhs.float_type;
        }
        return lhs.int_type < rhs.int_type;
    }

    friend bool operator>(AnyType lhs, AnyType rhs){
        return rhs < lhs;
    }

    friend bool operator==(AnyType lhs, AnyType rhs) {
        if (lhs.type_name == NONE || rhs.type_name == NONE)
            return lhs.type_name == rhs.type_name;
        if (lhs.type_name >= 3 || rhs.type_name >= 3){
            if (lhs.type_name != rhs.type_name) return false;
            if (lhs.type_name == STR && rhs.type_name == STR)
                return lhs.str_type == rhs.str_type;
        }
        if (lhs.type_name == BOOL) lhs.int_type = lhs.bool_type;
        if (rhs.type_name == BOOL) rhs.int_type = rhs.bool_type;
        if (lhs.type_name == FLOAT || rhs.type_name == FLOAT){
            if (lhs.type_name != FLOAT) lhs.float_type = lhs.int_type.put2double();
            if (rhs.type_name != FLOAT) rhs.float_type = rhs.int_type.put2double();
            return lhs.float_type == rhs.float_type;
        }
        return lhs.int_type == rhs.int_type;
    }

    friend bool operator==(const AnyType &lhs, Type rhs){
        return lhs.type_name == rhs;
    }

    friend bool operator>=(const AnyType &lhs, const AnyType &rhs){
        return lhs > rhs || lhs == rhs;
    }

    friend bool operator<=(const AnyType &lhs, const AnyType &rhs){
        return lhs < rhs || lhs == rhs;
    }

    friend bool operator!=(const AnyType &lhs, const AnyType &rhs){
        return !(lhs == rhs);
    }
    
    friend bool operator!=(const AnyType &lhs, Type rhs){
        return lhs.type_name != rhs;
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

    void put2int(){
        if (type_name == FLOAT) *this = AnyType((long long)float_type);
        if (type_name == BOOL) *this = AnyType((int)bool_type);
        if (type_name == STR) *this = AnyType(BigNumber(str_type));
    }

    void put2float(){
        if (type_name == INT) *this = AnyType(int_type.put2double());
        if (type_name == BOOL) *this = AnyType((double)bool_type);
        if (type_name == STR){
            string context = str_type;
            int len = context.length();
            double prv = 0, suf = 0;
            for (int i = (context[0] =='-' ? 1 : 0); context[i] != '.'; ++i)
                prv = prv * 10 + context[i] - '0';
            for (int i = len - 1; context[i] != '.'; --i)
                suf = suf / 10 + context[i] - '0';
            *this = AnyType((context[0] == '-' ? -1. : 1.) * (prv + suf / 10));
        }
    }

    void put2str(){
        if (type_name == INT) *this = AnyType(int_type.put2string());
        if (type_name == FLOAT){
            double cur = abs(float_type);
            string ret;
            long long prv = cur;
            double suf = cur - prv, eps = 1e-7;
            for (; prv; prv /= 10)
                ret = char(prv % 10 + '0') + ret;
            ret += '.';
            int precision = 1;
            for (suf *= 10; abs(suf) > eps && precision <= 6; ++precision) {
                long long tmp = suf;
                ret += char(tmp + '0');
                suf = (suf - tmp) * 10;
            }
            for (; precision <= 6; ++precision) ret = ret + '0';
            if (float_type < 0) ret = '-' + ret;
            *this = AnyType(ret);
        }
        if (type_name == BOOL) *this = AnyType(bool_type ? "True" : "False");
    }

    void put2bool(){
        if (type_name == INT) *this = AnyType(!int_type.empty());
        if (type_name == FLOAT) *this = AnyType((bool)float_type);
        if (type_name == STR) *this = AnyType(!str_type.empty());
    }

    friend ostream &operator<<(ostream &os, const AnyType &x){
        if (x.type_name == INT) os << x.int_type.put2string();
        if (x.type_name == FLOAT) os << setiosflags(ios::fixed) << setprecision(6) << x.float_type;
        if (x.type_name == BOOL) os << (x.bool_type ? "True" : "False");
        if (x.type_name == STR) os << x.str_type;
        if (x.type_name == NONE) os << "None";
        return os;
    }

};

#endif //PYTHON_INTERPRETER_AnyType_HPP
