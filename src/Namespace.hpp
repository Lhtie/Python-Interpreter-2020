#ifndef PYTHON_INTERPRETER_NAMESPACE_HPP
#define PYTHON_INTERPRETER_NAMESPACE_HPP

#include "AnyType.hpp"
using namespace std;

class Namespace{
//global variables
private:
    map<string, AnyType> global;

    AnyType &get_global(const string &name){
        return global[name];
    }

//stack for functions
private:
    vector<map<string, AnyType> > stack;

    AnyType &get_stack(const string &name){
        int size = stack.size();
        auto res = stack[size-1].find(name);
        if (res != stack[size-1].end()) return res->second;
		res = global.find(name);
		if (res != global.end()) return res->second;
        return stack[size-1][name];
    }

public:
	Namespace(){
		global.clear();
		stack.clear();
	}

	AnyType &operator[](const string &name){
		if (stack.empty()) return get_global(name);
		else return get_stack(name);
	}

	void add_layer(const map<string, AnyType> &init){
		stack.emplace_back(init);
	}

	void del_layer(){
		stack.erase(stack.end() - 1);
	}
};

#endif //PYTHON_INTERPRETER_NAMESPACE_HPP
