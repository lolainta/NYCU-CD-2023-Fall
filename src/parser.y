%{
#include "AST/BinaryOperator.hpp"
#include "AST/CompoundStatement.hpp"
#include "AST/ConstantValue.hpp"
#include "AST/FunctionInvocation.hpp"
#include "AST/UnaryOperator.hpp"
#include "AST/VariableReference.hpp"
#include "AST/assignment.hpp"
#include "AST/ast.hpp"
#include "AST/decl.hpp"
#include "AST/expression.hpp"
#include "AST/for.hpp"
#include "AST/function.hpp"
#include "AST/if.hpp"
#include "AST/print.hpp"
#include "AST/program.hpp"
#include "AST/read.hpp"
#include "AST/return.hpp"
#include "AST/variable.hpp"
#include "AST/while.hpp"

#include "AST/AstDumper.hpp"
#include "enums.hpp"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>


#define YYLTYPE yyltype

typedef struct YYLTYPE {
    uint32_t first_line;
    uint32_t first_column;
    uint32_t last_line;
    uint32_t last_column;
} yyltype;

extern uint32_t line_num;   /* declared in scanner.l */
extern char current_line[]; /* declared in scanner.l */
extern FILE *yyin;          /* declared by lex */
extern char *yytext;        /* declared by lex */

static AstNode *root;

extern "C" int yylex(void);
static void yyerror(const char *msg);
extern int yylex_destroy(void);
%}

%code requires {
    #include <vector>
    #include <string>
    class AstNode;
}

    /* For yylval */
%union {
    /* basic semantic value */
    char *identifier;
    int32_t integer;
    double real;
    bool boolean;
    char *str;
    std::vector<std::string> *str_list;
    AstNode *node;
    std::vector<AstNode*> *node_list;
    struct PType*type;
};

%type <identifier> ProgramName ID FunctionName
%type <integer> INT_LITERAL
%type <real> REAL_LITERAL
%type <str> STRING_LITERAL
%type <node> Declaration LiteralConstant
%type <node> Statement CompoundStatement Simple Condition While For Return FunctionCall FunctionInvocation
%type <node> StringAndBoolean IntegerAndReal Expression VariableReference ElseOrNot
%type <boolean> NegOrNot 
%type <node> Program ProgramUnit Function FunctionDeclaration FunctionDefinition FormalArg
%type <node_list> DeclarationList Declarations FunctionList Functions StatementList Statements IdList FormalArgs FormalArgList
%type <node_list> ExpressionList Expressions ArrRefs ArrRefList
%type <type> Type ScalarType ArrType ArrDecl ReturnType

%token COMMA SEMICOLON COLON
%token L_PARENTHESIS R_PARENTHESIS
%token L_BRACKET R_BRACKET

    /* Operator */
%token ASSIGN
%left OR
%left AND
%right NOT
%left LESS LESS_OR_EQUAL EQUAL GREATER GREATER_OR_EQUAL NOT_EQUAL
%left PLUS MINUS
%left MULTIPLY DIVIDE MOD
%right UNARY_MINUS

    /* Keyword */
%token ARRAY BOOLEAN INTEGER REAL STRING
%token END BEGIN_ /* Use BEGIN_ since BEGIN is a keyword in lex */
%token DO ELSE FOR IF THEN WHILE
%token DEF OF TO RETURN VAR
%token FALSE TRUE
%token PRINT READ

    /* Identifier */
%token ID

    /* Literal */
%token INT_LITERAL
%token REAL_LITERAL
%token STRING_LITERAL

%%

ProgramUnit:
    Program
    |
    Function
;

Program:
    ProgramName SEMICOLON
    /* ProgramBody */
    DeclarationList FunctionList CompoundStatement
    /* End of ProgramBody */
    END {
        root = new ProgramNode(@1.first_line, @1.first_column, $1, $3, $4, $5);
        free($1);
    }
;

ProgramName:
    ID
;

DeclarationList:
    Epsilon {
        $$ = new std::vector<AstNode *>();
    }
    |
    Declarations
    {
        $$ = $1;
    }
;

Declarations:
    Declaration {
        $$ = new std::vector<AstNode *>();
        $$->push_back($1);
    }
    |
    Declarations Declaration {
        $$ = $1;
        $$->push_back($2);
    }
;

FunctionList:
    Epsilon {
        $$ = new std::vector<AstNode *>();
    }
    |
    Functions {
        $$ = $1;
    }
;

Functions:
    Function {
        $$ = new std::vector<AstNode *>();
        $$->push_back($1);
    }
    |
    Functions Function {
        $$ = $1;
        $$->push_back($2);
    }
;

Function:
    FunctionDeclaration
    |
    FunctionDefinition
;

FunctionDeclaration:
    FunctionName L_PARENTHESIS FormalArgList R_PARENTHESIS ReturnType SEMICOLON {
        $$ = new FunctionNode(@1.first_line, @1.first_column, $1, $3, $5);
    }
;

FunctionDefinition:
    FunctionName L_PARENTHESIS FormalArgList R_PARENTHESIS ReturnType
    CompoundStatement
    END {
        auto tmp = new FunctionNode(@1.first_line, @1.first_column, $1, $3, $5);
        auto ret = static_cast<FunctionNode *>(tmp);
        ret->setBody($6);
        $$ = ret;
    }
;

FunctionName:
    ID
;

FormalArgList:
    Epsilon {
        $$ = new std::vector<AstNode *>();
    }
    |
    FormalArgs {
        $$ = $1;
    }
;

FormalArgs:
    FormalArg {
        $$ = new std::vector<AstNode*>();
        $$->push_back($1);
    }
    |
    FormalArgs SEMICOLON FormalArg {
        $$ = $1;
        $$->push_back($3);
    }
;

FormalArg:
    IdList COLON Type {
        auto var_nodes = new std::vector<VariableNode *>();
        for (auto &var : *$1) {
            auto vnode = static_cast<VariableNode *>(var);
            vnode->setType(*static_cast<PType*>($3));
            var_nodes->push_back(vnode);
        }
        $$ = new DeclNode(@1.first_line, @1.first_column, var_nodes);
    }
;

IdList:
    ID {
        $$ = new std::vector<AstNode*>();
        $$->push_back(new VariableNode(@1.first_line, @1.first_column, PType(SType::unknown_t), $1));
    }
    |
    IdList COMMA ID {
        $$ = $1;
        $$->push_back(new VariableNode(@3.first_line, @3.first_column, PType(SType::unknown_t), $3));
    }
;

ReturnType:
    COLON ScalarType {
        $$ = new PType(*static_cast<PType*>($2));
    }
    |
    Epsilon {
        $$ = new PType(SType::void_t);
    }
;

Declaration:
    VAR IdList COLON Type SEMICOLON {
        auto var_nodes = new std::vector<VariableNode *>();
        for (auto &var : *$2) {
            auto vnode = static_cast<VariableNode *>(var);
            vnode->setType(*static_cast<PType*>($4));
            var_nodes->push_back(vnode);
        }
        $$ = new DeclNode(@1.first_line, @1.first_column, var_nodes);
    }
    |
    VAR IdList COLON LiteralConstant SEMICOLON {
        auto literal = (ConstantValueNode *)($4);
        auto var_nodes = new std::vector<VariableNode *>();
        for (auto &var : *$2) {
            auto vnode = static_cast<VariableNode *>(var);
            vnode->setType(literal->getType());
            vnode->setConstVal(literal);
            var_nodes->push_back(vnode);
        }
        $$ = new DeclNode(@1.first_line, @1.first_column, var_nodes);
    }
;

Type:
    ScalarType
    |
    ArrType
;

ScalarType:
    INTEGER {
        $$ = new PType(SType::int_t);
    }
    |
    REAL {
        $$ = new PType(SType::real_t);
    }
    |
    STRING {
        $$ = new PType(SType::string_t);
    }
    |
    BOOLEAN {
        $$ = new PType(SType::bool_t);
    }
;

ArrType:
    ArrDecl ScalarType {
        auto arr_type = static_cast<PType*>($1);
        arr_type->setBaseType($2->stype);
        $$ = arr_type;
    }
;

ArrDecl:
    ARRAY INT_LITERAL OF {
        $$ = new PType(SType::unknown_t, std::vector<int>(1,$2));
    }
    |
    ArrDecl ARRAY INT_LITERAL OF {
        auto arr_type = static_cast<PType*>($1);
        arr_type->addDim($3);
        $$ = arr_type;
    }
;

LiteralConstant:
    NegOrNot INT_LITERAL {
        if ($1) {
            $$ = new ConstantValueNode(@1.first_line, @1.first_column, PType(SType::int_t), "", -$2, 0, 0);
        } else {
            $$ = new ConstantValueNode(@2.first_line, @2.first_column, PType(SType::int_t), "", $2, 0, 0);
        }
    }
    |
    NegOrNot REAL_LITERAL {
        if ($1) {
            $$ = new ConstantValueNode(@1.first_line, @1.first_column, PType(SType::real_t), "", 0, -$2, 0);
        } else {
            $$ = new ConstantValueNode(@2.first_line, @2.first_column, PType(SType::real_t), "", 0, $2, 0);
        }
    }
    |
    StringAndBoolean {
        $$ = $1;
    }
;

NegOrNot:
    Epsilon {
        $$ = false;
    }
    |
    MINUS %prec UNARY_MINUS {
        $$ = true;
    }
;

StringAndBoolean:
    STRING_LITERAL {
        $$ = new ConstantValueNode(@1.first_line, @1.first_column, PType(SType::string_t), $1, 0, 0, 0);
    }
    |
    TRUE {
        $$ = new ConstantValueNode(@1.first_line, @1.first_column, PType(SType::bool_t), "", 0, 0, true);
    }
    |
    FALSE {
        $$ = new ConstantValueNode(@1.first_line, @1.first_column, PType(SType::bool_t), "", 0, 0, false);
    }
;

IntegerAndReal:
    INT_LITERAL {
        $$ = new ConstantValueNode(@1.first_line, @1.first_column, PType(SType::int_t), 0, $1, 0, 0);
    }
    |
    REAL_LITERAL {
        $$ = new ConstantValueNode(@1.first_line, @1.first_column, PType(SType::real_t), 0, 0, $1, 0);
    }
;

Statement:
    CompoundStatement
    |
    Simple
    |
    Condition
    |
    While
    |
    For
    |
    Return
    |
    FunctionCall
;

CompoundStatement:
    BEGIN_
    DeclarationList
    StatementList
    END {
        $$ = new CompoundStatementNode(@1.first_line, @1.first_column, $2, $3);
    }
;

Simple:
    VariableReference ASSIGN Expression SEMICOLON {
        $$ = new AssignmentNode(@2.first_line, @2.first_column, $1, $3);
    }
    |
    PRINT Expression SEMICOLON {
        $$ = new PrintNode(@1.first_line, @1.first_column, $2);
    }
    |
    READ VariableReference SEMICOLON {
        $$ = new ReadNode(@1.first_line, @1.first_column, $2);
    }
;

VariableReference:
    ID ArrRefList {
        $$ = new VariableReferenceNode(@1.first_line, @1.first_column, $1, $2);
    }
;

ArrRefList:
    Epsilon {
        $$ = new std::vector<AstNode *>();
    }
    |
    ArrRefs {
        $$ = $1;
    }
;

ArrRefs:
    L_BRACKET Expression R_BRACKET {
        $$ = new std::vector<AstNode *>();
        $$->push_back($2);
    }
    |
    ArrRefs L_BRACKET Expression R_BRACKET {
        $$ = $1;
        $$->push_back($3);
    }
;

Condition:
    IF Expression THEN
    CompoundStatement
    ElseOrNot
    END IF {
        $$ = new IfNode(@1.first_line, @1.first_column, $2, $4, $5);
    }
;

ElseOrNot:
    ELSE
    CompoundStatement {
        $$ = $2;
    }
    |
    Epsilon {
        $$ = nullptr;
    }
;

While:
    WHILE Expression DO
    CompoundStatement
    END DO {
        $$ = new WhileNode(@1.first_line, @1.first_column, $2, $4);
    }
;

For:
    FOR ID ASSIGN INT_LITERAL TO INT_LITERAL DO
    CompoundStatement
    END DO {
        auto var =  new VariableNode(@2.first_line, @2.first_column, PType(SType::int_t), $2);
        auto decl = new DeclNode(@2.first_line, @2.first_column, new std::vector<VariableNode *>({var}));
        auto var_ref = new VariableReferenceNode(@2.first_line, @2.first_column, var->getName().c_str(), new std::vector<AstNode *>());
        auto init = new AssignmentNode(@3.first_line, @3.first_column, var_ref, new ConstantValueNode(@4.first_line, @4.first_column, PType(SType::int_t), "", $4, 0, 0));
        auto cnst = new ConstantValueNode(@6.first_line, @6.first_column, PType(SType::int_t), "", $6, 0, 0);
        $$ = new ForNode(@1.first_line, @1.first_column, decl, init, cnst, $8);
    }
;

Return:
    RETURN Expression SEMICOLON {
        $$ = new ReturnNode(@1.first_line, @1.first_column, $2);
    }
;

FunctionCall:
    FunctionInvocation SEMICOLON
;

FunctionInvocation:
    ID L_PARENTHESIS ExpressionList R_PARENTHESIS {
        $$ = new FunctionInvocationNode(@1.first_line, @1.first_column, $1, $3);
    }
;

ExpressionList:
    Epsilon {
        $$ = new std::vector<AstNode *>();
    }
    |
    Expressions {
        $$ = $1;
    }
;

Expressions:
    Expression {
        $$ = new std::vector<AstNode *>();
        $$->push_back($1);
    }
    |
    Expressions COMMA Expression {
        $$ = $1;
        $$->push_back($3);
    }
;

StatementList:
    Epsilon {
        $$ = new std::vector<AstNode *>();
    }
    |
    Statements {
        $$ = $1;
    }
;

Statements:
    Statement {
        $$ = new std::vector<AstNode *>();
        $$->push_back($1);
    }
    |
    Statements Statement {
        $$ = $1;
        $$->push_back($2);
    }
;

Expression:
    L_PARENTHESIS Expression R_PARENTHESIS {
        $$ = $2;
    }
    |
    MINUS Expression %prec UNARY_MINUS {
        $$ = new UnaryOperatorNode(@1.first_line, @1.first_column, Operator::NEGATIVE_op, $2);
    }
    |
    Expression MULTIPLY Expression {
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, Operator::MULTIPLY_op, $1, $3);
    }
    |
    Expression DIVIDE Expression {
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, Operator::DIVIDE_op, $1, $3);
    }
    |
    Expression MOD Expression {
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, Operator::MOD_op, $1, $3);
    }
    |
    Expression PLUS Expression {
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, Operator::PLUS_op, $1, $3);
    }
    |
    Expression MINUS Expression {
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, Operator::MINUS_op, $1, $3);
    }
    |
    Expression LESS Expression {
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, Operator::LESS_op, $1, $3);
    }
    |
    Expression LESS_OR_EQUAL Expression {
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, Operator::LESS_OR_EQUAL_op, $1, $3);
    }
    |
    Expression GREATER Expression {
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, Operator::GREATER_op, $1, $3);
    }
    |
    Expression GREATER_OR_EQUAL Expression {
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, Operator::GREATER_OR_EQUAL_op, $1, $3);}
    |
    Expression EQUAL Expression {
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, Operator::EQUAL_op, $1, $3);
    }
    |
    Expression NOT_EQUAL Expression {
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, Operator::NOT_EQUAL_op, $1, $3);
    }
    |
    NOT Expression {
        $$ = new UnaryOperatorNode(@1.first_line, @1.first_column, Operator::NOT_op, $2);
    }
    |
    Expression AND Expression {
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, Operator::AND_op, $1, $3);
    }
    |
    Expression OR Expression {
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, Operator::OR_op, $1, $3);
    }
    |
    IntegerAndReal {
        $$ = $1;
    }
    |
    StringAndBoolean {
        $$ = $1;
    }
    |
    VariableReference {
        $$ = $1;
    }
    |
    FunctionInvocation {
        $$ = $1;
    }
;

Epsilon:
;

%%

void yyerror(const char *msg) {
    fprintf(stderr,
            "\n"
            "|-----------------------------------------------------------------"
            "---------\n"
            "| Error found in Line #%d: %s\n"
            "|\n"
            "| Unmatched token: %s\n"
            "|-----------------------------------------------------------------"
            "---------\n",
            line_num, current_line, yytext);
    exit(-1);
}

int main(int argc, const char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename> [--dump-ast]\n", argv[0]);
        exit(-1);
    }

    yyin = fopen(argv[1], "r");
    if (yyin == NULL) {
        perror("fopen() failed");
        exit(-1);
    }

    yyparse();

    if (argc >= 3 && strcmp(argv[2], "--dump-ast") == 0) {
        AstDumper dumper;
        root->accept(dumper);
    }

    printf("\n"
           "|--------------------------------|\n"
           "|  There is no syntactic error!  |\n"
           "|--------------------------------|\n");

    delete root;
    fclose(yyin);
    yylex_destroy();
    return 0;
}
