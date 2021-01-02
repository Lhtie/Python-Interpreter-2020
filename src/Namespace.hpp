#ifndef PYTHON_INTERPRETER_NAMESPACE_HPP
#define PYTHON_INTERPRETER_NAMESPACE_HPP

#include "AnyType.hpp"
using namespace std;

class Namespace{
private:
    //type 0 : global variables
    //type 1 : stack for while or if
    //type 2 : stack for functions
    int type;
    vector<int> stack0;

public:
    Namespace() : type(0) {
        stack0.clear();
        stack0.push_back(type);
        global.clear();
        stack1.clear();
        stack2.clear();
    }

    AnyType &operator[](const string &name){
        if (type == 0) return get_global(name);
        if (type == 1) return get_stack1(name);
        if (type == 2) return get_stack2(name);
    }

    void add_layer(){
        add_layer1();
    }

    void add_layer(const map<string, AnyType> &init){
        add_layer2(init);
    }

    void del_layer(){
        if (type == 1) del_layer1();
        else if (type == 2) del_layer2();
    }

//global variables
private:
    map<string, AnyType> global;

    AnyType &get_global(const string &name){
        return global[name];
    }

//stack for while or if
private:
    vector<map<string, AnyType> > stack1;

    AnyType &get_stack1(const string &name){
        int size = stack1.size(), j = stack0.size() - 1;
        for (int i = size - 1; i >= 0; --i, --j){
            if (stack0[j] != 1) break;
            auto res = stack1[i].find(name);
            if (res != stack1[i].end()) return res->second;
        }
        if (stack0[j] == 2){
            int size = stack2.size();
            auto res = stack2[size-1].find(name);
            if (res != stack2[size-1].end()) return res->second;
        }
		auto res = global.find(name);
		if (res != global.end()) return res->second;
		else return stack1[size-1][name];
    }

    void add_layer1(){
        type = 1;
        stack0.push_back(type);
        stack1.emplace_back(map<string, AnyType>());
    }

    void del_layer1(){
        stack1.erase(stack1.end() - 1);
        stack0.erase(stack0.end() - 1);
        type = *(stack0.end() - 1);
    }

//stack for functions
private:
    vector<map<string, AnyType> > stack2;

    AnyType &get_stack2(const string &name){
        int size = stack2.size();
        auto res = stack2[size-1].find(name);
        if (res != stack2[size-1].end()) return res->second;
		res = global.find(name);
		if (res != global.end()) return res->second;
        return stack2[size-1][name];
    }

    void add_layer2(const map<string, AnyType> &init){
        type = 2;
        stack0.push_back(type);
        stack2.emplace_back(init);
    }

    void del_layer2(){
        stack2.erase(stack2.end() - 1);
        stack0.erase(stack0.end() - 1);
        type = *(stack0.end() - 1);
    }

};

#endif //PYTHON_INTERPRETER_NAMESPACE_HPP
