#pragma once
#include "AST.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

struct Symbol
{
    std::string type;
    bool isStatic = false;
    bool isMutable = true;
};

class SemanticAnalyzer
{
public:
    SemanticAnalyzer();
    void analyze(const std::shared_ptr<ProgramNode> &prog);
    const std::vector<std::string> &errors() const { return errs; }

private:
    std::vector<std::unordered_map<std::string, Symbol>> scopes;
    std::vector<std::string> errs;

    void pushScope();
    void popScope();
    bool declare(const std::string &name, const std::string &type, bool isStatic = false, bool isMutable = true);
    Symbol *lookup(const std::string &name);

    void analyzeNode(const std::shared_ptr<ASTNode> &node);
    void analyzeVarDecl(const std::shared_ptr<VarDeclNode> &node);
    void analyzeFuncDecl(const std::shared_ptr<FuncDeclNode> &node);
    void analyzeIf(const std::shared_ptr<IfNode> &node);
    void analyzeAssign(const std::shared_ptr<AssignNode> &node);
    void analyzeShow(const std::shared_ptr<ShowNode> &node);
    void analyzeDuring(const std::shared_ptr<DuringNode> &node);
    void analyzeForEach(const std::shared_ptr<ForEachNode> &node);
    void analyzeRepeat(const std::shared_ptr<RepeatNode> &node);
    void analyzeReturn(const std::shared_ptr<ReturnNode> &node);

    std::string analyzeExpr(const std::shared_ptr<ExprNode> &expr);
    bool isNumericType(const std::string &t) const;
    bool typeEquals(const std::string &a, const std::string &b) const;
    void reportError(int line, const std::string &msg);

    std::string currentFuncReturnType;
};