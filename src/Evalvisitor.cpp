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
map<string, Python3Parser::ParametersContext *> parameters;
map<string, Python3Parser::SuiteContext *> suites;

antlrcpp::Any EvalVisitor::visitFile_input(Python3Parser::File_inputContext *ctx){
    int size = ctx->stmt().size();
    for (int i = 0; i < size; ++i) {
        visitStmt(ctx->stmt(i)); 
    }
    return AnyType(NONE);
}

antlrcpp::Any EvalVisitor::visitFuncdef(Python3Parser::FuncdefContext *ctx){
    string name = ctx->NAME()->getText();
    parameters[name] = ctx->parameters();
    suites[name] = ctx->suite();
    return AnyType(NONE);
}

antlrcpp::Any EvalVisitor::visitParameters(Python3Parser::ParametersContext *ctx){
    if (ctx->typedargslist())
        return visitTypedargslist(ctx->typedargslist());
    vector<antlrcpp::Any> empty1;
    vector<AnyType> empty2;
    return make_pair(empty1, empty2);
}

antlrcpp::Any EvalVisitor::visitTypedargslist(Python3Parser::TypedargslistContext *ctx){
    int size1 = ctx->tfpdef().size(), size2 = ctx->test().size();
    vector<antlrcpp::Any> x;
    vector<AnyType> y;
    for (int i = 0; i < size1; ++i)
        x.push_back(visitTfpdef(ctx->tfpdef(i)));
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
    if (ctx->compound_stmt())
        return visitCompound_stmt(ctx->compound_stmt());
}

antlrcpp::Any EvalVisitor::visitSimple_stmt(Python3Parser::Simple_stmtContext *ctx){
    return visitSmall_stmt(ctx->small_stmt());
}

antlrcpp::Any EvalVisitor::visitSmall_stmt(Python3Parser::Small_stmtContext *ctx){
    if (ctx->expr_stmt()) {
        visitExpr_stmt(ctx->expr_stmt());
        return AnyType(NONE);
    } else if (ctx->flow_stmt())
        return visitFlow_stmt(ctx->flow_stmt());
}

antlrcpp::Any EvalVisitor::visitExpr_stmt(Python3Parser::Expr_stmtContext *ctx){
    if (ctx->augassign()){
        int type = visitAugassign(ctx->augassign());
        antlrcpp::Any lhs = visitTestlist(ctx->testlist(0)), rhs = visitTestlist(ctx->testlist(1));
        auto l = lhs.as<vector<antlrcpp::Any> >();
        auto r = rhs.as<vector<antlrcpp::Any> >();
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
        return AnyType(NONE);
    } else if (ctx->ASSIGN().size()){
        int num = ctx->ASSIGN().size();
        for (int i = num; i >= 1; --i){
            antlrcpp::Any lhs = visitTestlist(ctx->testlist(i - 1)), rhs = visitTestlist(ctx->testlist(i));
            auto l = lhs.as<vector<antlrcpp::Any> >();
            auto r = rhs.as<vector<antlrcpp::Any> >();
            if (l.size() == r.size()) {
                for (int j = 0; j < l.size(); ++j) {
                    if (r[j].is<string>()) r[j] = data_manager[r[j].as<string>()];
                    data_manager[l[j].as<string>()] = r[j].as<AnyType>();
                }
            }
        }
        return AnyType(NONE);
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
    if (ctx->continue_stmt())
        return visitContinue_stmt(ctx->continue_stmt());
    if (ctx->return_stmt())
        return visitReturn_stmt(ctx->return_stmt());
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
        for (int i = 0; i < res.size(); ++i) {
            if (res[i].is<string>())
                res[i] = data_manager[res[i].as<string>()];
            ret.push_back(res[i].as<AnyType>());
        }
        return AnyType(RETURN, ret);
    } else return AnyType(RETURN, vector<AnyType>());
}

antlrcpp::Any EvalVisitor::visitCompound_stmt(Python3Parser::Compound_stmtContext *ctx){
    if (ctx->if_stmt())
        return visitIf_stmt(ctx->if_stmt());
    if (ctx->while_stmt())
        return visitWhile_stmt(ctx->while_stmt());
    if (ctx->funcdef())
        return visitFuncdef(ctx->funcdef());
}

antlrcpp::Any EvalVisitor::visitIf_stmt(Python3Parser::If_stmtContext *ctx){
    int size_test = ctx->test().size(), size_suite = ctx->suite().size();
    for (int i = 0; i < size_test; ++i){
        antlrcpp::Any res = visitTest(ctx->test(i));
        if (res.is<string>()) res = data_manager[res.as<string>()];
        if (res.as<AnyType>() != BOOL)
        	res.as<AnyType>().put2bool();
        if (res.as<AnyType>() == AnyType(true)) {
            data_manager.add_layer();
            auto res = visitSuite(ctx->suite(i)).as<AnyType>();
            data_manager.del_layer();
            return res;
        }
    }
    if (ctx->ELSE()){
        data_manager.add_layer();
        auto res = visitSuite(ctx->suite(size_suite - 1)).as<AnyType>();
        data_manager.del_layer();
        return res;
    }
    return AnyType(NONE);
}

antlrcpp::Any EvalVisitor::visitWhile_stmt(Python3Parser::While_stmtContext *ctx){
    data_manager.add_layer();
    auto Judger = visitTest(ctx->test());
    if (Judger.is<string>()) Judger = data_manager[Judger.as<string>()];
    if (Judger.as<AnyType>() != BOOL)
        	Judger.as<AnyType>().put2bool();
    while (Judger.as<AnyType>() == AnyType(true)) {
        auto res = visitSuite(ctx->suite()).as<AnyType>();
        if (res == BREAK) break;
        if (res == CONTINUE) continue;
        if (res == RETURN){
        	data_manager.del_layer();
        	return res;
        }
        Judger = visitTest(ctx->test());
        if (Judger.is<string>()) Judger = data_manager[Judger.as<string>()];
		if (Judger.as<AnyType>() != BOOL)
		    Judger.as<AnyType>().put2bool();
    }
    data_manager.del_layer();
    return AnyType(NONE);
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
    return AnyType(NONE);
}

//returned value is string or AnyType
//for function calls, returned value is vector<AnyType>
antlrcpp::Any EvalVisitor::visitTest(Python3Parser::TestContext *ctx){
    return visitOr_test(ctx->or_test());
}

antlrcpp::Any EvalVisitor::visitOr_test(Python3Parser::Or_testContext *ctx){
    int size = ctx->and_test().size();
    antlrcpp::Any ret = visitAnd_test(ctx->and_test(0)), other;
    if (size == 1) return ret;
    if (ret.is<string>()) ret = data_manager[ret.as<string>()];
    if (ret.as<AnyType>() != BOOL) ret.as<AnyType>().put2bool();
    for (int i = 1; i < size; ++i){
    	if (ret.as<AnyType>() == AnyType(true)) break;
        other = visitAnd_test(ctx->and_test(i));
        if (other.is<string>()) other = data_manager[other.as<string>()];
        if (other.as<AnyType>() != BOOL) other.as<AnyType>().put2bool();
        ret = ret.as<AnyType>() | other.as<AnyType>();
    }
    return ret;
}

antlrcpp::Any EvalVisitor::visitAnd_test(Python3Parser::And_testContext *ctx){
    int size = ctx->not_test().size();
    antlrcpp::Any ret = visitNot_test(ctx->not_test(0)), other;
    if (size == 1) return ret;
    if (ret.is<string>()) ret = data_manager[ret.as<string>()];
    if (ret.as<AnyType>() != BOOL) ret.as<AnyType>().put2bool();
    for (int i = 1; i < size; ++i){
    	if (ret.as<AnyType>() == AnyType(false)) break;
        other = visitNot_test(ctx->not_test(i));
        if (other.is<string>()) other = data_manager[other.as<string>()];
        if (other.as<AnyType>() != BOOL) other.as<AnyType>().put2bool();
        ret = ret.as<AnyType>() & other.as<AnyType>();
    }
    return ret;
}

antlrcpp::Any EvalVisitor::visitNot_test(Python3Parser::Not_testContext *ctx){
    if (ctx->not_test()){
        antlrcpp::Any ret = visitNot_test(ctx->not_test());
        if (ret.is<string>()) ret = data_manager[ret.as<string>()];
        return !(ret.as<AnyType>());
    } else if (ctx->comparison()){
        return visitComparison(ctx->comparison());
    }
}

antlrcpp::Any EvalVisitor::visitComparison(Python3Parser::ComparisonContext *ctx){
    int size = ctx->arith_expr().size();
    if (size == 1) return visitArith_expr(ctx->arith_expr(0));
    antlrcpp::Any prv = visitArith_expr(ctx->arith_expr(0));
    if (prv.is<string>()) prv = data_manager[prv.as<string>()];
    AnyType ret(true);
    for (int i = 1; i < size; ++i){
        antlrcpp::Any cur = visitArith_expr(ctx->arith_expr(i));
        if (cur.is<string>()) cur = data_manager[cur.as<string>()];
        int type = visitComp_op(ctx->comp_op(i - 1)).as<int>();
        if (type == 0) ret &= AnyType(prv.as<AnyType>() < cur.as<AnyType>());
        if (type == 1) ret &= AnyType(prv.as<AnyType>() > cur.as<AnyType>());
        if (type == 2) ret &= AnyType(prv.as<AnyType>() == cur.as<AnyType>());
        if (type == 3) ret &= AnyType(prv.as<AnyType>() >= cur.as<AnyType>());
        if (type == 4) ret &= AnyType(prv.as<AnyType>() <= cur.as<AnyType>());
        if (type == 5) ret &= AnyType(prv.as<AnyType>() != cur.as<AnyType>());
        prv = cur;
        if (ret == AnyType(false)) break;
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
    antlrcpp::Any ret = visitTerm(ctx->term(0));
    if (size == 1) return ret;
    if (ret.is<string>()) ret = data_manager[ret.as<string>()];
    for (int i = 1; i < size; ++i){
        antlrcpp::Any other = visitTerm(ctx->term(i));
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
    antlrcpp::Any ret = visitFactor(ctx->factor(0));
    if (size == 1) return ret;
    if (ret.is<string>()) ret = data_manager[ret.as<string>()];
    for (int i = 1; i < size; ++i){
        antlrcpp::Any other = visitFactor(ctx->factor(i));
        if (other.is<string>()) other = data_manager[other.as<string>()];
        int type = visitMuldivmod_op(ctx->muldivmod_op(i-1)).as<int>();
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
        antlrcpp::Any ret = visitFactor(ctx->factor());
        if (ret.is<string>()) ret = data_manager[ret.as<string>()];
        if (ctx->MINUS()) ret = AnyType(0) - ret.as<AnyType>();
        return ret;
    } else if (ctx->atom_expr()){
        return visitAtom_expr(ctx->atom_expr());
    }
}

antlrcpp::Any EvalVisitor::visitAtom_expr(Python3Parser::Atom_exprContext *ctx){
    if (ctx->trailer()){
        antlrcpp::Any res = visitAtom(ctx->atom());
        if (res.is<string>()){
            auto trailer = visitTrailer(ctx->trailer()).as<pair<vector<antlrcpp::Any>, vector<AnyType> > >();
            auto x = trailer.first;
            auto y = trailer.second;
            if (res.as<string>() == "print") {
                for (int i = 0; i < x.size(); ++i) {
                    if (x[i].is<string>()) x[i] = data_manager[x[i].as<string>()];
                    cout << (i ? " " : "") << x[i].as<AnyType>();
                }
                cout << endl;
                return AnyType(NONE);
            }
            if (res.as<string>() == "int"){
                if (x[0].is<string>())
                    data_manager[x[0].as<string>()].put2int();
                else x[0].as<AnyType>().put2int();
                return x[0];
            }
            if (res.as<string>() == "float"){
                if (x[0].is<string>())
                    data_manager[x[0].as<string>()].put2float();
                else x[0].as<AnyType>().put2float();
                return x[0];
            }
            if (res.as<string>() == "str"){
                if (x[0].is<string>())
                    data_manager[x[0].as<string>()].put2str();
                else x[0].as<AnyType>().put2str();
                return x[0];
            }
            if (res.as<string>() == "bool"){
                if (x[0].is<string>())
                    data_manager[x[0].as<string>()].put2bool();
                else x[0].as<AnyType>().put2bool();
                return x[0];
            }
            //else run defined function
            string name = res.as<string>();
            auto parameter = visitParameters(parameters[name]).as<pair<vector<antlrcpp::Any>, vector<AnyType> > >();
            auto x_def = parameter.first;
            auto y_def = parameter.second;
            map<string, AnyType> new_layer;
            for (int i = 0; i < y_def.size(); ++i) {
                new_layer[x_def[i+x_def.size()-y_def.size()].as<string>()] = y_def[i];
            }
            for (int i = 0; i < x.size() - y.size(); ++i){
                if (x[i].is<string>()) x[i] = data_manager[x[i].as<string>()];
                new_layer[x_def[i].as<string>()] = x[i].as<AnyType>();
            }
            for (int i = 0; i < y.size(); ++i){
                new_layer[x[i+x.size()-y.size()].as<string>()] = y[i];
            }
            data_manager.add_layer(new_layer);
            auto suite = visitSuite(suites[name]).as<AnyType>();
            data_manager.del_layer();
            if (suite == RETURN){
                if (suite.return_type.size() == 0)
                    return AnyType(NONE);
                else if (suite.return_type.size() == 1)
                    return suite.return_type[0];
                else return suite.return_type;
            }
            return AnyType(NONE);
        }
    } else return visitAtom(ctx->atom());
}

antlrcpp::Any EvalVisitor::visitTrailer(Python3Parser::TrailerContext *ctx){
    if (ctx->arglist())
        return visitArglist(ctx->arglist());
    vector<antlrcpp::Any> empty1;
    vector<AnyType> empty2;
    return make_pair(empty1, empty2);
}

antlrcpp::Any EvalVisitor::visitAtom(Python3Parser::AtomContext *ctx){
    if (ctx->NAME()) return ctx->NAME()->getText();
    if (ctx->NUMBER()){
        bool is_float = false;
        string context = ctx->NUMBER()->getText();
        int len = context.length();
        for (int i = 0; i < len; ++i)
            is_float |= context[i] == '.';
        if (is_float){
            double ret;
            stringstream ss(context);
            ss >> ret;
            return AnyType(ret);
        } else return AnyType(BigNumber(ctx->NUMBER()->getText()));
    }
    if (ctx->NONE()) return AnyType(NONE);
    if (ctx->TRUE()) return AnyType(true);
    if (ctx->FALSE()) return AnyType(false);
    if (ctx->test()) return visitTest(ctx->test());
    int size = ctx->STRING().size();
    if (size){
        string ret;
        for (int i = 0; i < size; ++i){
            string str = ctx->STRING(i)->getText();
            for (int j = 1; str[j] != '"' && str[j] != '\''; ++j)
                ret += str[j];
        }
        return AnyType(ret);
    }
    return AnyType(NONE);
}

antlrcpp::Any EvalVisitor::visitTestlist(Python3Parser::TestlistContext *ctx){
    int size = ctx->test().size();
    vector<antlrcpp::Any> ret;
    for (int i = 0; i < size; ++i){
        auto res = visitTest(ctx->test(i));
        if (res.is<string>() || res.is<AnyType>())
            ret.push_back(res);
        else {
            auto iter = res.as<vector<AnyType> >();
            for (int j = 0; j < iter.size(); ++j)
                ret.push_back(iter[j]);
        }
    }
    return ret;
}

antlrcpp::Any EvalVisitor::visitArglist(Python3Parser::ArglistContext *ctx){
    vector<antlrcpp::Any> ret1;
    vector<AnyType> ret2;
    int size = ctx->argument().size();
    for (int i = 0; i < size; ++i) {
        auto res = visitArgument(ctx->argument(i)).as<pair<int, pair<antlrcpp::Any, antlrcpp::Any> > >();
        ret1.push_back(res.second.first);
        if (res.first) {
            auto tmp = res.second.second;
            if (tmp.is<string>()) tmp = data_manager[tmp.as<string>()];
            ret2.push_back(tmp.as<AnyType>());
        }
    }
    return make_pair(ret1, ret2);
}

antlrcpp::Any EvalVisitor::visitArgument(Python3Parser::ArgumentContext *ctx){
    if (ctx->test().size() == 1)
        return make_pair(0, make_pair(visitTest(ctx->test(0)), antlrcpp::Any()));
    else return make_pair(1, make_pair(visitTest(ctx->test(0)), visitTest(ctx->test(1))));
}
