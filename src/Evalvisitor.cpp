#include "Evalvisitor.h"
#include "BigNumber.hpp"
#include "AnyType.hpp"
#include <iostream>
#include <map>
#include <string>
#include <vector>
using namespace std;

map<string, AnyType> vars;

antlrcpp::Any EvalVisitor::visitFile_input(Python3Parser::File_inputContext *ctx){
    int size = ctx->stmt().size();
    for (int i = 0; i < size; ++i) {
        visitStmt(ctx->stmt(i));
    }
    return AnyType(NONE);
}

antlrcpp::Any EvalVisitor::visitFuncdef(Python3Parser::FuncdefContext *ctx){

}

antlrcpp::Any EvalVisitor::visitParameters(Python3Parser::ParametersContext *ctx){

}

antlrcpp::Any EvalVisitor::visitTypedargslist(Python3Parser::TypedargslistContext *ctx){

}

antlrcpp::Any EvalVisitor::visitTfpdef(Python3Parser::TfpdefContext *ctx){

}

antlrcpp::Any EvalVisitor::visitStmt(Python3Parser::StmtContext *ctx){
    if (ctx->simple_stmt())
        return visitSimple_stmt(ctx->simple_stmt());
    else if (ctx->compound_stmt())
        return visitCompound_stmt(ctx->compound_stmt());
}

antlrcpp::Any EvalVisitor::visitSimple_stmt(Python3Parser::Simple_stmtContext *ctx){
    return visitSmall_stmt(ctx->small_stmt());
}

antlrcpp::Any EvalVisitor::visitSmall_stmt(Python3Parser::Small_stmtContext *ctx){
    if (ctx->expr_stmt())
        return visitExpr_stmt(ctx->expr_stmt());
    else if (ctx->flow_stmt())
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
                if (r[j].is<string>()) r[j] = vars[r[j].as<string>()];
                if (type == 0) vars[l[j]] += r[j].as<AnyType>();
                if (type == 1) vars[l[j]] -= r[j].as<AnyType>();
                if (type == 2) vars[l[j]] *= r[j].as<AnyType>();
                if (type == 3) vars[l[j]] /= r[j].as<AnyType>();
                if (type == 4) vars[l[j]] = div(vars[l[j]], r[j].as<AnyType>());
                if (type == 5) vars[l[j]] %= r[j].as<AnyType>();
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
                    if (r[j].is<string>()) r[j] = vars[r[j].as<string>()];
                    vars[l[j].as<string>()] = r[j].as<AnyType>();
                }
            }
        }
        return AnyType(NONE);
    } return visitTestlist(ctx->testlist(0)).as<vector<antlrcpp::Any> >()[0];
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

}

antlrcpp::Any EvalVisitor::visitBreak_stmt(Python3Parser::Break_stmtContext *ctx){

}

antlrcpp::Any EvalVisitor::visitContinue_stmt(Python3Parser::Continue_stmtContext *ctx){

}

antlrcpp::Any EvalVisitor::visitReturn_stmt(Python3Parser::Return_stmtContext *ctx){

}

antlrcpp::Any EvalVisitor::visitCompound_stmt(Python3Parser::Compound_stmtContext *ctx){

}

antlrcpp::Any EvalVisitor::visitIf_stmt(Python3Parser::If_stmtContext *ctx){

}

antlrcpp::Any EvalVisitor::visitWhile_stmt(Python3Parser::While_stmtContext *ctx){

}

antlrcpp::Any EvalVisitor::visitSuite(Python3Parser::SuiteContext *ctx){

}

antlrcpp::Any EvalVisitor::visitTest(Python3Parser::TestContext *ctx){
    return visitOr_test(ctx->or_test());
}

antlrcpp::Any EvalVisitor::visitOr_test(Python3Parser::Or_testContext *ctx){
    int size = ctx->and_test().size();
    antlrcpp::Any ret = visitAnd_test(ctx->and_test(0)), other;
    if (size == 1) return ret;
    if (ret.is<string>()) ret = vars[ret.as<string>()];
    for (int i = 1; i < size; ++i){
        other = visitAnd_test(ctx->and_test(i));
        if (other.is<string>()) other = vars[other.as<string>()];
        ret = ret.as<AnyType>() | other.as<AnyType>();
    }
    return ret;
}

antlrcpp::Any EvalVisitor::visitAnd_test(Python3Parser::And_testContext *ctx){
    int size = ctx->not_test().size();
    antlrcpp::Any ret = visitNot_test(ctx->not_test(0)), other;
    if (size == 1) return ret;
    if (ret.is<string>()) ret = vars[ret.as<string>()];
    for (int i = 1; i < size; ++i){
        other = visitNot_test(ctx->not_test(i));
        if (other.is<string>()) other = vars[other.as<string>()];
        ret = ret.as<AnyType>() & other.as<AnyType>();
    }
    return ret;
}

antlrcpp::Any EvalVisitor::visitNot_test(Python3Parser::Not_testContext *ctx){
    if (ctx->not_test()){
        antlrcpp::Any ret = visitNot_test(ctx->not_test());
        if (ret.is<string>()) ret = vars[ret.as<string>()];
        return !(ret.as<AnyType>());
    } else if (ctx->comparison()){
        return visitComparison(ctx->comparison());
    }
}

antlrcpp::Any EvalVisitor::visitComparison(Python3Parser::ComparisonContext *ctx){
    int size = ctx->arith_expr().size();
    if (size == 1) return visitArith_expr(ctx->arith_expr(0));
    vector<antlrcpp::Any> res;
    for (int i = 0; i < size; ++i)
        res.push_back(visitArith_expr(ctx->arith_expr(i)));
    AnyType ret(true);
    for (int i = 1; i < size; ++i){
        antlrcpp::Any l = res[i-1], r = res[i];
        if (l.is<string>()) l = vars[l.as<string>()];
        if (r.is<string>()) r = vars[r.as<string>()];
        int type = visitComp_op(ctx->comp_op(i - 1)).as<int>();
        if (type == 0) ret &= AnyType(l.as<AnyType>() < r.as<AnyType>());
        if (type == 1) ret &= AnyType(l.as<AnyType>() > r.as<AnyType>());
        if (type == 2) ret &= AnyType(l.as<AnyType>() == r.as<AnyType>());
        if (type == 3) ret &= AnyType(l.as<AnyType>() >= r.as<AnyType>());
        if (type == 4) ret &= AnyType(l.as<AnyType>() <= r.as<AnyType>());
        if (type == 5) ret &= AnyType(l.as<AnyType>() != r.as<AnyType>());
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
    if (ret.is<string>()) ret = vars[ret.as<string>()];
    for (int i = 1; i < size; ++i){
        antlrcpp::Any other = visitTerm(ctx->term(i));
        if (other.is<string>()) other = vars[other.as<string>()];
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
    if (ret.is<string>()) ret = vars[ret.as<string>()];
    for (int i = 1; i < size; ++i){
        antlrcpp::Any other = visitFactor(ctx->factor(i));
        if (other.is<string>()) other = vars[other.as<string>()];
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
        if (ret.is<string>()) ret = vars[ret.as<string>()];
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
            if (res.as<string>() == "print") {
                auto x = visitTrailer(ctx->trailer()).as<vector<antlrcpp::Any> >();
                if (x[0].is<string>()) x[0] = vars[x[0].as<string>()];
                cout << x[0].as<AnyType>() << endl;
                return AnyType(NONE);
            }
        }
    } else return visitAtom(ctx->atom());
}

antlrcpp::Any EvalVisitor::visitTrailer(Python3Parser::TrailerContext *ctx){
    if (ctx->arglist())
        return visitArglist(ctx->arglist());
    vector<antlrcpp::Any> empty;
    return empty;
}

antlrcpp::Any EvalVisitor::visitAtom(Python3Parser::AtomContext *ctx){
    if (ctx->NAME()) return ctx->NAME()->getText();
    if (ctx->NUMBER()) return AnyType(BigNumber(ctx->NUMBER()->getText()));
    if (ctx->NONE()) return AnyType(NONE);
    if (ctx->TRUE()) return AnyType(true);
    if (ctx->FALSE()) return AnyType(false);
    if (ctx->test()) return visitTest(ctx->test());
    int size = ctx->STRING().size();
    if (size){
        if (size == 1){
            string ret, str = ctx->STRING(0)->getText();
            for (int i = 1; str[i] != '"'; ++i)
                ret += str[i];
            return AnyType(ret);
        } else {
            vector<string> ret;
            for (int i = 0; i < size; ++i)
                ret.push_back(ctx->STRING(i)->getText());
            return ret;
        }
    }
}

antlrcpp::Any EvalVisitor::visitTestlist(Python3Parser::TestlistContext *ctx){
    int size = ctx->test().size();
    vector<antlrcpp::Any> ret;
    for (int i = 0; i < size; ++i) {
        ret.push_back(visitTest(ctx->test(i)));
    }
    return ret;
}

antlrcpp::Any EvalVisitor::visitArglist(Python3Parser::ArglistContext *ctx){
    vector<antlrcpp::Any> ret;
    int size = ctx->argument().size();
    for (int i = 0; i < size; ++i)
        ret.push_back(visitArgument(ctx->argument(i)));
    return ret;
}

antlrcpp::Any EvalVisitor::visitArgument(Python3Parser::ArgumentContext *ctx){
    return visitTest(ctx->test(0));
}