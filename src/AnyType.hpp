#ifndef PYTHON_INTERPRETER_AnyType_HPP
#define PYTHON_INTERPRETER_AnyType_HPP

#include "BigNumber.hpp"
#include <string>
using namespace std;

enum Type{INT, FLOAT, BOOL, STR, NONE};

class AnyType{
private:
    Type type_name;
    BigNumber int_type;
    double float_type;
    bool bool_type;
    string str_type;

public:
    AnyType() = default;
    AnyType(Type x) : type_name(x) {}
    AnyType(antlrcpp::Any x) : float_type(0), bool_type(false) {
        if (x.is<BigNumber>()) type_name = INT, int_type = x.as<BigNumber>();
        if (x.is<int>()) type_name = INT, int_type = x.as<int>();
        if (x.is<double>()) type_name = FLOAT, float_type = x.as<double>();
        if (x.is<bool>()) type_name = BOOL, bool_type = x.as<bool>();
        if (x.is<string>()) type_name = STR, str_type = x.as<string>();
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
        if (lhs.type_name != BOOL) lhs.bool_type = !lhs.int_type.empty() || lhs.float_type;
        if (rhs.type_name != BOOL) rhs.bool_type = !rhs.int_type.empty() || rhs.float_type;
        return AnyType(lhs.bool_type || rhs.bool_type);
    }

    friend AnyType operator&(AnyType lhs, AnyType rhs){
        if (lhs.type_name != BOOL) lhs.bool_type = !lhs.int_type.empty() || lhs.float_type;
        if (rhs.type_name != BOOL) rhs.bool_type = !rhs.int_type.empty() || rhs.float_type;
        return AnyType(lhs.bool_type && rhs.bool_type);
    }

    friend AnyType operator!(AnyType x){
        if (x.type_name != BOOL) x.bool_type = !x.int_type.empty() || x.float_type;
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
        if (lhs.type_name == STR && rhs.type_name == STR){
            return lhs.str_type > rhs.str_type;
        }
        if (lhs.type_name == BOOL) lhs.int_type = lhs.bool_type;
        if (rhs.type_name == BOOL) rhs.int_type = rhs.bool_type;
        if (lhs.type_name == FLOAT || rhs.type_name == FLOAT){
            if (lhs.type_name != FLOAT) lhs.float_type = lhs.int_type.put2double();
            if (rhs.type_name != FLOAT) rhs.float_type = rhs.int_type.put2double();
            return lhs.float_type > rhs.float_type;
        }
        return lhs.int_type > rhs.int_type;
    }

    friend bool operator==(AnyType lhs, AnyType rhs){
        if (lhs.type_name == STR && rhs.type_name == STR){
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

    friend bool operator>=(const AnyType &lhs, const AnyType &rhs){
        return lhs > rhs || lhs == rhs;
    }

    friend bool operator<=(const AnyType &lhs, const AnyType &rhs){
        return lhs < rhs || lhs == rhs;
    }

    friend bool operator!=(const AnyType &lhs, const AnyType &rhs){
        return !(lhs == rhs);
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

    friend ostream &operator<<(ostream &os, const AnyType &x){
        if (x.type_name == INT) os << x.int_type.put2string();
        if (x.type_name == FLOAT) os << setprecision(6) << x.float_type;
        if (x.type_name == BOOL) os << (x.bool_type ? "True" : "False");
        if (x.type_name == STR) os << x.str_type;
        if (x.type_name == NONE) os << "None";
        return os;
    }

};

#endif //PYTHON_INTERPRETER_AnyType_HPP