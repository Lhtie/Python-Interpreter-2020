#include "Evalvisitor.h"
#include "BigNumber.hpp"
#include "AnyType.hpp"
#include "Namespace.hpp"
#include <iostream>
#include <map>
#include <string>
#include <vector>
using namespace std;

Namespace data_manager;
map<string, Python3Parser::SuiteContext *> suites;
map<string, vector<string> > parameters_name;
map<string, vector<AnyType> > parameters_value;
AnyType True = AnyType(true), False = AnyType(false), None = AnyType(NONE);

antlrcpp::Any EvalVisitor::visitFile_input(Python3Parser::File_inputContext *ctx){
    int size = ctx->stmt().size();
    for (int i = 0; i < size; ++i) {
        visitStmt(ctx->stmt(i));
    }
    return None;
}

antlrcpp::Any EvalVisitor::visitFuncdef(Python3Parser::FuncdefContext *ctx){
    string name = ctx->NAME()->getText();
    auto parameter = visitParameters(ctx->parameters()).as<pair<vector<string>, vector<AnyType> > >();
    parameters_name[name] = parameter.first, parameters_value[name] = parameter.second;
    suites[name] = ctx->suite();
    return None;
}

antlrcpp::Any EvalVisitor::visitParameters(Python3Parser::ParametersContext *ctx){
    if (ctx->typedargslist())
        return visitTypedargslist(ctx->typedargslist());
    return make_pair(vector<string>(), vector<AnyType>());
}

antlrcpp::Any EvalVisitor::visitTypedargslist(Python3Parser::TypedargslistContext *ctx){
    int size1 = ctx->tfpdef().size(), size2 = ctx->test().size();
    vector<string> x;
    vector<AnyType> y;
    for (int i = 0; i < size1; ++i)
        x.push_back(visitTfpdef(ctx->tfpdef(i)).as<string>());
    for (int i = 0; i < size2; ++i) {
        antlrcpp::Any res = visitTest(ctx->test(i));
        if (res.is<string>()) res = data_manager[res.as<string>()];
        y.push_back(res.as<AnyType>());
    }
    return make_pair(x, y);
}

antlrcpp::Any EvalVisitor::visitTfpdef(Python3Parser::TfpdefContext *ctx){
    return ctx->NAME()->getText();
}

antlrcpp::Any EvalVisitor::visitStmt(Python3Parser::StmtContext *ctx){
    if (ctx->simple_stmt())
        return visitSimple_stmt(ctx->simple_stmt());
    else return visitCompound_stmt(ctx->compound_stmt());
}

antlrcpp::Any EvalVisitor::visitSimple_stmt(Python3Parser::Simple_stmtContext *ctx){
    return visitSmall_stmt(ctx->small_stmt());
}

antlrcpp::Any EvalVisitor::visitSmall_stmt(Python3Parser::Small_stmtContext *ctx){
    if (ctx->expr_stmt()) {
        return visitExpr_stmt(ctx->expr_stmt());
    } else return visitFlow_stmt(ctx->flow_stmt());
}

antlrcpp::Any EvalVisitor::visitExpr_stmt(Python3Parser::Expr_stmtContext *ctx){
    if (ctx->augassign()){
        int type = visitAugassign(ctx->augassign());
        auto l = visitTestlist(ctx->testlist(0)).as<vector<antlrcpp::Any> >();
        auto r = visitTestlist(ctx->testlist(1)).as<vector<antlrcpp::Any> >();
        if (l.size() == r.size()) {
            for (int j = 0; j < l.size(); ++j) {
                if (r[j].is<string>()) r[j] = data_manager[r[j].as<string>()];
                if (type == 0) data_manager[l[j]] += r[j].as<AnyType>();
                if (type == 1) data_manager[l[j]] -= r[j].as<AnyType>();
                if (type == 2) data_manager[l[j]] *= r[j].as<AnyType>();
                if (type == 3) data_manager[l[j]] /= r[j].as<AnyType>();
                if (type == 4) data_manager[l[j]] = div(data_manager[l[j]], r[j].as<AnyType>());
                if (type == 5) data_manager[l[j]] %= r[j].as<AnyType>();
            }
        }
        return None;
    } else if (!ctx->ASSIGN().empty()){
        int num = ctx->ASSIGN().size();
        for (int i = num; i >= 1; --i){
            auto l = visitTestlist(ctx->testlist(i - 1)).as<vector<antlrcpp::Any> >();
            auto r = visitTestlist(ctx->testlist(i)).as<vector<antlrcpp::Any> >();
            if (l.size() == r.size()) {
                for (int j = 0; j < l.size(); ++j) {
                    if (r[j].is<string>()) r[j] = data_manager[r[j].as<string>()];
                    data_manager[l[j]] = r[j].as<AnyType>();
                }
            }
        }
        return None;
    }
    return visitTestlist(ctx->testlist(0)).as<vector<antlrcpp::Any> >()[0];
}

antlrcpp::Any EvalVisitor::visitAugassign(Python3Parser::AugassignContext *ctx){
    if (ctx->ADD_ASSIGN()) return 0;
    if (ctx->SUB_ASSIGN()) return 1;
    if (ctx->MULT_ASSIGN()) return 2;
    if (ctx->DIV_ASSIGN()) return 3;
    if (ctx->IDIV_ASSIGN()) return 4;
    if (ctx->MOD_ASSIGN()) return 5;
}

antlrcpp::Any EvalVisitor::visitFlow_stmt(Python3Parser::Flow_stmtContext *ctx){
    if (ctx->break_stmt())
        return visitBreak_stmt(ctx->break_stmt());
    else if (ctx->continue_stmt())
        return visitContinue_stmt(ctx->continue_stmt());
    else return visitReturn_stmt(ctx->return_stmt());
}

antlrcpp::Any EvalVisitor::visitBreak_stmt(Python3Parser::Break_stmtContext *ctx){
    return AnyType(BREAK);
}

antlrcpp::Any EvalVisitor::visitContinue_stmt(Python3Parser::Continue_stmtContext *ctx){
    return AnyType(CONTINUE);
}

antlrcpp::Any EvalVisitor::visitReturn_stmt(Python3Parser::Return_stmtContext *ctx){
    if (ctx->testlist()) {
        auto res = visitTestlist(ctx->testlist()).as<vector<antlrcpp::Any> >();
        vector<AnyType> ret;
        for (auto &iter : res) {
            if (iter.is<string>()) iter = data_manager[iter.as<string>()];
            ret.push_back(iter.as<AnyType>());
        }
        return AnyType(ret);
    } else return AnyType(vector<AnyType>());
}

antlrcpp::Any EvalVisitor::visitCompound_stmt(Python3Parser::Compound_stmtContext *ctx){
    if (ctx->if_stmt())
        return visitIf_stmt(ctx->if_stmt());
    else if (ctx->while_stmt())
        return visitWhile_stmt(ctx->while_stmt());
    else return visitFuncdef(ctx->funcdef());
}

antlrcpp::Any EvalVisitor::visitIf_stmt(Python3Parser::If_stmtContext *ctx){
    int size_test = ctx->test().size(), size_suite = ctx->suite().size();
    for (int i = 0; i < size_test; ++i){
		auto res = visitTest(ctx->test(i));
		if (res.is<string>()) res = data_manager[res.as<string>()];
		if (res.as<AnyType>()){
			return visitSuite(ctx->suite(i));
		}
	}
	if (ctx->ELSE()) return visitSuite(ctx->suite(size_suite - 1));
    return None;
}

antlrcpp::Any EvalVisitor::visitWhile_stmt(Python3Parser::While_stmtContext *ctx){
    auto Judger = visitTest(ctx->test());
    if (Judger.is<string>()) Judger = data_manager[Judger.as<string>()];
    while (Judger.as<AnyType>()) {
        auto res = visitSuite(ctx->suite()).as<AnyType>();
        if (res == BREAK) break;
        if (res == CONTINUE) continue;
        if (res == RETURN) return res;
        Judger = visitTest(ctx->test());
        if (Judger.is<string>()) Judger = data_manager[Judger.as<string>()];
    }
    return None;
}

antlrcpp::Any EvalVisitor::visitSuite(Python3Parser::SuiteContext *ctx){
    if (ctx->simple_stmt())
        return visitSimple_stmt(ctx->simple_stmt());
    int size = ctx->stmt().size();
    for (int i = 0; i < size; ++i) {
        auto res = visitStmt(ctx->stmt(i)).as<AnyType>();
        if (res == BREAK || res == CONTINUE || res == RETURN)
            return res;
    }
    return None;
}

//returned value is string, AnyType, vector<AnyType>(only used by testlist)
//for testlist, returned value is vector<antlrcpp::Any(string | AnyType)>
antlrcpp::Any EvalVisitor::visitTest(Python3Parser::TestContext *ctx){
    return visitOr_test(ctx->or_test());
}

antlrcpp::Any EvalVisitor::visitOr_test(Python3Parser::Or_testContext *ctx){
    int size = ctx->and_test().size();
    auto ret = visitAnd_test(ctx->and_test(0));
    if (size == 1) return ret;
    if (ret.is<string>()) ret = data_manager[ret.as<string>()];
    for (int i = 1; i < size; ++i){
    	if (ret.as<AnyType>()) break;
        auto other = visitAnd_test(ctx->and_test(i));
        if (other.is<string>()) other = data_manager[other.as<string>()];
        ret = ret.as<AnyType>() | other.as<AnyType>();
    }
    return ret;
}

antlrcpp::Any EvalVisitor::visitAnd_test(Python3Parser::And_testContext *ctx){
	int size = ctx->not_test().size();
	auto ret = visitNot_test(ctx->not_test(0));
	if (size == 1) return ret;
	if (ret.is<string>()) ret = data_manager[ret.as<string>()];
	for (int i = 1; i < size; ++i){
		if (!ret.as<AnyType>()) break;
		auto other = visitNot_test(ctx->not_test(i));
		if (other.is<string>()) other = data_manager[other.as<string>()];
		ret = ret.as<AnyType>() & other.as<AnyType>();
	}
	return ret;
}

antlrcpp::Any EvalVisitor::visitNot_test(Python3Parser::Not_testContext *ctx){
    if (ctx->not_test()) {
		auto ret = visitNot_test(ctx->not_test());
		if (ret.is<string>()) ret = data_manager[ret.as<string>()];
		return !(ret.as<AnyType>());
	} else return visitComparison(ctx->comparison());
}

antlrcpp::Any EvalVisitor::visitComparison(Python3Parser::ComparisonContext *ctx){
    int size = ctx->arith_expr().size();
	auto prv = visitArith_expr(ctx->arith_expr(0));
    if (size == 1) return prv;
    if (prv.is<string>()) prv = data_manager[prv.as<string>()];
    AnyType ret(true);
    for (int i = 1; i < size; ++i){
        auto cur = visitArith_expr(ctx->arith_expr(i));
        if (cur.is<string>()) cur = data_manager[cur.as<string>()];
        int type = visitComp_op(ctx->comp_op(i - 1)).as<int>();
        if (type == 0) ret &= prv.as<AnyType>() < cur.as<AnyType>();
        if (type == 1) ret &= prv.as<AnyType>() > cur.as<AnyType>();
        if (type == 2) ret &= prv.as<AnyType>() == cur.as<AnyType>();
        if (type == 3) ret &= prv.as<AnyType>() >= cur.as<AnyType>();
        if (type == 4) ret &= prv.as<AnyType>() <= cur.as<AnyType>();
        if (type == 5) ret &= prv.as<AnyType>() != cur.as<AnyType>();
        prv = cur;
        if (!ret) break;
    }
    return ret;
}

antlrcpp::Any EvalVisitor::visitComp_op(Python3Parser::Comp_opContext *ctx){
    if (ctx->LESS_THAN()) return 0;
    if (ctx->GREATER_THAN()) return 1;
    if (ctx->EQUALS()) return 2;
    if (ctx->GT_EQ()) return 3;
    if (ctx->LT_EQ()) return 4;
    if (ctx->NOT_EQ_2()) return 5;
}

antlrcpp::Any EvalVisitor::visitArith_expr(Python3Parser::Arith_exprContext *ctx){
    int size = ctx->term().size();
    auto ret = visitTerm(ctx->term(0));
    if (size == 1) return ret;
    if (ret.is<string>()) ret = data_manager[ret.as<string>()];
    for (int i = 1; i < size; ++i){
        auto other = visitTerm(ctx->term(i));
        if (other.is<string>()) other = data_manager[other.as<string>()];
        if (visitAddorsub_op(ctx->addorsub_op(i-1)).as<int>() == 0){
            ret = ret.as<AnyType>() + other.as<AnyType>();
        } else ret = ret.as<AnyType>() - other.as<AnyType>();
    }
    return ret;
}

antlrcpp::Any EvalVisitor::visitAddorsub_op(Python3Parser::Addorsub_opContext *ctx){
    if (ctx->ADD()) return 0;
    if (ctx->MINUS()) return 1;
}

antlrcpp::Any EvalVisitor::visitTerm(Python3Parser::TermContext *ctx){
    int size = ctx->factor().size();
    auto ret = visitFactor(ctx->factor(0));
    if (size == 1) return ret;
    if (ret.is<string>()) ret = data_manager[ret.as<string>()];
    for (int i = 1; i < size; ++i){
        auto other = visitFactor(ctx->factor(i));
        if (other.is<string>()) other = data_manager[other.as<string>()];
        int type = visitMuldivmod_op(ctx->muldivmod_op(i - 1)).as<int>();
        if (type == 0) ret = ret.as<AnyType>() * other.as<AnyType>();
        if (type == 1) ret = ret.as<AnyType>() / other.as<AnyType>();
        if (type == 2) ret = div(ret.as<AnyType>(), other.as<AnyType>());
        if (type == 3) ret = ret.as<AnyType>() % other.as<AnyType>();
    }
    return ret;
}

antlrcpp::Any EvalVisitor::visitMuldivmod_op(Python3Parser::Muldivmod_opContext *ctx){
    if (ctx->STAR()) return 0;
    if (ctx->DIV()) return 1;
    if (ctx->IDIV()) return 2;
    if (ctx->MOD()) return 3;
}

antlrcpp::Any EvalVisitor::visitFactor(Python3Parser::FactorContext *ctx){
    if (ctx->factor()){
        auto ret = visitFactor(ctx->factor());
        if (ret.is<string>()) ret = data_manager[ret.as<string>()];
        if (ctx->MINUS()) ret = -ret.as<AnyType>();
        return ret;
    } else return visitAtom_expr(ctx->atom_expr());
}

antlrcpp::Any EvalVisitor::visitAtom_expr(Python3Parser::Atom_exprContext *ctx){
    if (ctx->trailer()){
        auto name = visitAtom(ctx->atom()).as<string>();
		auto trailer = visitTrailer(ctx->trailer()).as<pair<vector<antlrcpp::Any>, vector<AnyType> > >();
		auto x = trailer.first;
		auto y = trailer.second;
		if (name == "print") {
			for (int i = 0; i < x.size(); ++i) {
				if (x[i].is<string>()) x[i] = data_manager[x[i].as<string>()];
				cout << (i ? " " : "") << x[i].as<AnyType>();
			}
			cout << endl;
			return None;
		}
		if (name == "int"){
			if (x[0].is<string>()) x[0] = data_manager[x[0].as<string>()];
			return x[0].as<AnyType>().put2int();
		}
		if (name == "float"){
			if (x[0].is<string>()) x[0] = data_manager[x[0].as<string>()];
			return x[0].as<AnyType>().put2float();
		}
		if (name == "str"){
			if (x[0].is<string>()) x[0] = data_manager[x[0].as<string>()];
			return x[0].as<AnyType>().put2str();
		}
		if (name == "bool"){
			if (x[0].is<string>()) x[0] = data_manager[x[0].as<string>()];
			return x[0].as<AnyType>().put2bool();
		}

		//run defined function
		vector<string> x_def = parameters_name[name];
		vector<AnyType> y_def = parameters_value[name];
		map<string, AnyType> new_layer;
		for (int i = 0; i < y_def.size(); ++i) {
			new_layer[x_def[i+x_def.size()-y_def.size()]] = y_def[i];
		}
		for (int i = 0; i < x.size() - y.size(); ++i){
			if (x[i].is<string>()) x[i] = data_manager[x[i].as<string>()];
			new_layer[x_def[i]] = x[i].as<AnyType>();
		}
		for (int i = 0; i < y.size(); ++i){
			new_layer[x[i+x.size()-y.size()].as<string>()] = y[i];
		}

		data_manager.add_layer(new_layer);
		auto suite = visitSuite(suites[name]).as<AnyType>();
		data_manager.del_layer();
		if (suite == RETURN){
			if (suite.return_type.empty())
				return None;
			else if (suite.return_type.size() == 1)
				return suite.return_type[0];
			else return suite.return_type;
		}
		return None;
    } else return visitAtom(ctx->atom());
}

antlrcpp::Any EvalVisitor::visitTrailer(Python3Parser::TrailerContext *ctx){
    if (ctx->arglist())
        return visitArglist(ctx->arglist());
    return make_pair(vector<antlrcpp::Any>(), vector<AnyType>());
}

antlrcpp::Any EvalVisitor::visitAtom(Python3Parser::AtomContext *ctx){
    if (ctx->NAME()) return ctx->NAME()->getText();
    if (ctx->NUMBER()){
        bool is_float = false;
        string context = ctx->NUMBER()->getText();
        int len = context.length();
        for (int i = 0; i < len; ++i)
            is_float |= context[i] == '.';
        if (is_float) return AnyType(context).put2float();
        else return AnyType(BigNumber(context));
    }
    if (ctx->NONE()) return None;
    if (ctx->TRUE()) return True;
    if (ctx->FALSE()) return False;
    if (ctx->test()) return visitTest(ctx->test());
    int size = ctx->STRING().size();
    if (size){
        string ret;
        for (int i = 0; i < size; ++i){
            string str = ctx->STRING(i)->getText();
            int len = str.length();
            for (int j = 1; j < len - 1; ++j)
                ret += str[j];
        }
        return AnyType(ret);
    }
    return None;
}

antlrcpp::Any EvalVisitor::visitTestlist(Python3Parser::TestlistContext *ctx){
    int size = ctx->test().size();
    vector<antlrcpp::Any> ret;
    for (int i = 0; i < size; ++i){
        auto res = visitTest(ctx->test(i));
        if (res.is<string>() || res.is<AnyType>())
            ret.push_back(res);
        else {
            auto x = res.as<vector<AnyType> >();
            for (auto &iter : x) ret.push_back(iter);
        }
    }
    return ret;
}

antlrcpp::Any EvalVisitor::visitArglist(Python3Parser::ArglistContext *ctx){
    vector<antlrcpp::Any> x;
    vector<AnyType> y;
    int size = ctx->argument().size();
    for (int i = 0; i < size; ++i) {
        auto res = visitArgument(ctx->argument(i));
        if (res.is<pair<antlrcpp::Any, antlrcpp::Any> >()){
            auto t = res.as<pair<antlrcpp::Any, antlrcpp::Any> >();
            x.push_back(t.first);
            if (t.second.is<string>()) t.second = data_manager[t.second.as<string>()];
            y.push_back(t.second.as<AnyType>());
        } else x.push_back(res);
    }
    return make_pair(x, y);
}

antlrcpp::Any EvalVisitor::visitArgument(Python3Parser::ArgumentContext *ctx){
    if (ctx->test().size() == 1)
        return visitTest(ctx->test(0));
    else return make_pair(visitTest(ctx->test(0)), visitTest(ctx->test(1)));
}
