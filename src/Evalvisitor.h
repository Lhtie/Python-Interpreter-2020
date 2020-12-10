#ifndef PYTHON_INTERPRETER_EVALVISITOR_H
#define PYTHON_INTERPRETER_EVALVISITOR_H


#include "Python3BaseVisitor.h"

class EvalVisitor: public Python3BaseVisitor {
//todo:override all methods of Python3BaseVisitor
	antlrcpp::Any visitFile_input(Python3Parser::File_inputContext *ctx) override;

    antlrcpp::Any visitFuncdef(Python3Parser::FuncdefContext *ctx) override;

    antlrcpp::Any visitParameters(Python3Parser::ParametersContext *ctx) override;

    antlrcpp::Any visitTypedargslist(Python3Parser::TypedargslistContext *ctx) override;

    antlrcpp::Any visitTfpdef(Python3Parser::TfpdefContext *ctx) override;

    antlrcpp::Any visitStmt(Python3Parser::StmtContext *ctx) override;

    antlrcpp::Any visitSimple_stmt(Python3Parser::Simple_stmtContext *ctx) override;

    antlrcpp::Any visitSmall_stmt(Python3Parser::Small_stmtContext *ctx) override;

    antlrcpp::Any visitExpr_stmt(Python3Parser::Expr_stmtContext *ctx) override;

    antlrcpp::Any visitAugassign(Python3Parser::AugassignContext *ctx) override;

    antlrcpp::Any visitFlow_stmt(Python3Parser::Flow_stmtContext *ctx) override;

    antlrcpp::Any visitBreak_stmt(Python3Parser::Break_stmtContext *ctx) override;

    antlrcpp::Any visitContinue_stmt(Python3Parser::Continue_stmtContext *ctx) override;

    antlrcpp::Any visitReturn_stmt(Python3Parser::Return_stmtContext *ctx) override;

    antlrcpp::Any visitCompound_stmt(Python3Parser::Compound_stmtContext *ctx) override;

    antlrcpp::Any visitIf_stmt(Python3Parser::If_stmtContext *ctx) override;

    antlrcpp::Any visitWhile_stmt(Python3Parser::While_stmtContext *ctx) override;

    antlrcpp::Any visitSuite(Python3Parser::SuiteContext *ctx) override;

    antlrcpp::Any visitTest(Python3Parser::TestContext *ctx) override;

    antlrcpp::Any visitOr_test(Python3Parser::Or_testContext *ctx) override;

    antlrcpp::Any visitAnd_test(Python3Parser::And_testContext *ctx) override;

    antlrcpp::Any visitNot_test(Python3Parser::Not_testContext *ctx) override;

    antlrcpp::Any visitComparison(Python3Parser::ComparisonContext *ctx) override;

    antlrcpp::Any visitComp_op(Python3Parser::Comp_opContext *ctx) override;

    antlrcpp::Any visitArith_expr(Python3Parser::Arith_exprContext *ctx) override;

    antlrcpp::Any visitAddorsub_op(Python3Parser::Addorsub_opContext *ctx) override;

    antlrcpp::Any visitTerm(Python3Parser::TermContext *ctx) override;

    antlrcpp::Any visitMuldivmod_op(Python3Parser::Muldivmod_opContext *ctx) override;

    antlrcpp::Any visitFactor(Python3Parser::FactorContext *ctx) override;

    antlrcpp::Any visitAtom_expr(Python3Parser::Atom_exprContext *ctx) override;

    antlrcpp::Any visitTrailer(Python3Parser::TrailerContext *ctx) override;

    antlrcpp::Any visitAtom(Python3Parser::AtomContext *ctx) override;

    antlrcpp::Any visitTestlist(Python3Parser::TestlistContext *ctx) override;

    antlrcpp::Any visitArglist(Python3Parser::ArglistContext *ctx) override;

    antlrcpp::Any visitArgument(Python3Parser::ArgumentContext *ctx) override;

};


#endif //PYTHON_INTERPRETER_EVALVISITOR_H
