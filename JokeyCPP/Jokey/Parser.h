#pragma once
#include <vector>
#include <string>
#include <memory>
#include "AST.h"
#include "Token.h"

class Parser
{
public:
    Parser(const std::vector<Token> &tokens);
    ~Parser() = default;

    std::shared_ptr<ProgramNode> parseProgram();

    bool eof() const;
    const Token &peek(int k = 0) const;
    Token advance();
    bool check(const std::string &typeOrLexeme) const;
    Token consume(const std::string &expectedTypeOrLexeme, const std::string &errMsg);

private:
    std::vector<Token> tokens;
    size_t pos = 0;

    std::shared_ptr<ASTNode> parseDeclarationOrStatement();
    std::shared_ptr<ASTNode> parseDeclaration();
    std::shared_ptr<VarDeclNode> parseVarDecl();
    std::shared_ptr<ReturnNode> parseReturn();
    std::shared_ptr<FuncDeclNode> parseFuncDecl();

    std::vector<std::shared_ptr<ASTNode>> parseBlockDeclarations();
    std::vector<std::shared_ptr<ASTNode>> parseBlockCommands();

    std::shared_ptr<ASTNode> parseStatement();
    std::shared_ptr<IfNode> parseIf();
    std::shared_ptr<ASTNode> parseLoop();
    std::shared_ptr<AssignNode> parseAssign();
    std::shared_ptr<ShowNode> parseShow();

    ExprNodePtr parseExpression();
    ExprNodePtr parseLogic();
    ExprNodePtr parseRelational();
    ExprNodePtr parseAdd();
    ExprNodePtr parseTerm();
    ExprNodePtr parseFactor();

    bool isTypeKeyword(const std::string &lex) const;
    bool isReservedWord(const std::string &lex) const;
};
