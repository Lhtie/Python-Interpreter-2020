#include "Evalvisitor.h"
#include "BigNumber.hpp"
#include <iostream>
#include <map>
#include <string>
#include <vector>
using namespace std;

map<string, BigNumber> Integer;

antlrcpp::Any EvalVisitor::visitFile_input(Python3Parser::File_inputContext *ctx){
    auto iter = ctx->stmt();
    bool ret = true;
    for (int i = 0; i < iter.size(); ++i) {
        ret &= visitStmt(ctx->stmt(i)).as<bool>();
    }
    return ret;
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
                if (r[j].is<string>()) r[j] = Integer[r[j].as<string>()];
                if (type == 0) {
                    Integer[l[j].as<string>()] += r[j].as<BigNumber>();
                } else if (type == 1) {
                    Integer[l[j].as<string>()] -= r[j].as<BigNumber>();
                } else if (type == 2) {
                    Integer[l[j].as<string>()] *= r[j].as<BigNumber>();
                } else if (type == 4) {
                    Integer[l[j].as<string>()] /= r[j].as<BigNumber>();
                } else if (type == 5) {
                    Integer[l[j].as<string>()] %= r[j].as<BigNumber>();
                }
            }
        }
        return true;
    } else if (ctx->ASSIGN().size()){
        int num = ctx->ASSIGN().size();
        for (int i = num; i >= 1; --i){
            antlrcpp::Any lhs = visitTestlist(ctx->testlist(i-1)), rhs = visitTestlist(ctx->testlist(i));
            auto l = lhs.as<vector<antlrcpp::Any> >();
            auto r = rhs.as<vector<antlrcpp::Any> >();
            if (l.size() == r.size()) {
                for (int j = 0; j < l.size(); ++j) {
                    if (r[j].is<string>()) r[j] = Integer[r[j].as<string>()];
                    Integer[l[j].as<string>()] = r[j].as<BigNumber>();
                }
            }
        }
        return true;
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
    for (int i = 1; i < size; ++i){
        other = visitAnd_test(ctx->and_test(i));
        if (ret.is<bool>() && other.is<bool>())
            ret = ret.as<bool>() | other.as<bool>();
    }
    return ret;
}

antlrcpp::Any EvalVisitor::visitAnd_test(Python3Parser::And_testContext *ctx){
    int size = ctx->not_test().size();
    antlrcpp::Any ret = visitNot_test(ctx->not_test(0)), other;
    for (int i = 1; i < size; ++i){
        other = visitNot_test(ctx->not_test(i));
        if (ret.is<bool>() && other.is<bool>())
            ret = ret.as<bool>() & other.as<bool>();
    }
    return ret;
}

antlrcpp::Any EvalVisitor::visitNot_test(Python3Parser::Not_testContext *ctx){
    if (ctx->not_test()){
        antlrcpp::Any ret = visitNot_test(ctx->not_test());
        if (ret.is<bool>()) return !(ret.as<bool>());
    } else if (ctx->comparison()){
        return visitComparison(ctx->comparison());
    }
}

antlrcpp::Any EvalVisitor::visitComparison(Python3Parser::ComparisonContext *ctx){
    int size = ctx->arith_expr().size();
    antlrcpp::Any ret = visitArith_expr(ctx->arith_expr(0));
    if (size > 1){

    }
    return ret;
}

antlrcpp::Any EvalVisitor::visitComp_op(Python3Parser::Comp_opContext *ctx){

}

antlrcpp::Any EvalVisitor::visitArith_expr(Python3Parser::Arith_exprContext *ctx){
    int size = ctx->term().size();
    antlrcpp::Any ret = visitTerm(ctx->term(0));
    if (size == 1) return ret;
    if (ret.is<string>()) ret = Integer[ret.as<string>()];
    for (int i = 1; i < size; ++i){
        antlrcpp::Any other = visitTerm(ctx->term(i));
        if (other.is<string>()) other = Integer[other.as<string>()];
        if (visitAddorsub_op(ctx->addorsub_op(i-1)).as<int>() == 0){
            ret = ret.as<BigNumber>() + other.as<BigNumber>();
        } else ret = ret.as<BigNumber>() - other.as<BigNumber>();
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
    if (ret.is<string>()) ret = Integer[ret.as<string>()];
    for (int i = 1; i < size; ++i){
        antlrcpp::Any other = visitFactor(ctx->factor(i));
        if (other.is<string>()) other = Integer[other.as<string>()];
        int type = visitMuldivmod_op(ctx->muldivmod_op(i-1)).as<int>();
        if (type == 0){
            ret = ret.as<BigNumber>() * other.as<BigNumber>();
        } else if (type == 2){
            ret = ret.as<BigNumber>() / other.as<BigNumber>();
        } else if (type == 3){
            ret = ret.as<BigNumber>() % other.as<BigNumber>();
        }
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
        if (ret.is<string>()) ret = Integer[ret.as<string>()];
        if (ctx->MINUS()) ret = 0 - ret.as<BigNumber>();
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
                antlrcpp::Any x = visitTrailer(ctx->trailer());
                if (x.is<string>()) x = Integer[x.as<string>()];
                cout << x.as<BigNumber>().put2string() << endl;
            }
        }
        return true;
    } else return visitAtom(ctx->atom());
}

antlrcpp::Any EvalVisitor::visitTrailer(Python3Parser::TrailerContext *ctx){
    if (ctx->arglist())
        return visitArglist(ctx->arglist());
}

antlrcpp::Any EvalVisitor::visitAtom(Python3Parser::AtomContext *ctx){
    if (ctx->NAME()) return ctx->NAME()->getText();
    if (ctx->NUMBER()) return BigNumber(ctx->NUMBER()->getText());
    if (ctx->NONE()) return ctx->NONE()->getText();
    if (ctx->TRUE()) return true;
    if (ctx->FALSE()) return false;
    if (ctx->test()) return visitTest(ctx->test());
    int size = ctx->STRING().size();
    if (size){
        vector<string> ret;
        for (int i = 0; i < size; ++i)
            ret.push_back(ctx->STRING(i)->getText());
        return ret;
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
    return visitArgument(ctx->argument(0));
}

antlrcpp::Any EvalVisitor::visitArgument(Python3Parser::ArgumentContext *ctx){
    return visitTest(ctx->test(0));
}