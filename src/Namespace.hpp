#ifndef PYTHON_INTERPRETER_NAMESPACE_HPP
#define PYTHON_INTERPRETER_NAMESPACE_HPP

#include "AnyType.hpp"
using namespace std;

class Namespace{
private:
    //stack[0] stores global variables
    vector<map<string, AnyType> > stack;
    //0 : stack for variables ; 1 : stack for functions
    vector<bool> type;

public:
    Namespace() {
        stack.push_back(map<string, AnyType>());
        type.push_back(0);
    }

    AnyType &operator[](const string &name){
        int size = stack.size();
        if (stack[size-1].find(name) != stack[size-1].end())
            return stack[size-1][name];
        else if (stack[0].find(name) != stack[0].end())
            return stack[0][name];
        else return stack[size-1][name];
    }

    void add_layer(){
        stack.push_back(*(stack.end() - 1));
        type.push_back(0);
    }

    void add_layer(const map<string, AnyType> &init){
        stack.push_back(init);
        type.push_back(1);
    }

    void del_layer(){
        int cur = (int)type.size() - 1;
        if (!type[cur]){
            for (auto &iter : stack[cur])
                if (stack[cur-1].find(iter.first) != stack[cur-1].end())
                    stack[cur-1][iter.first] = iter.second;
        }
        type.erase(type.end() - 1);
        stack.erase(stack.end() - 1);
    }
};

#endif //PYTHON_INTERPRETER_NAMESPACE_HPP
