#pragma once
#include "AST.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>

class CodeGenerator
{
public:
    CodeGenerator();
    ~CodeGenerator() = default;

    std::string generate(const std::shared_ptr<ProgramNode> &program);

    const std::string &getIR() const { return irCode; }

private:
    std::string irCode;
    std::ostringstream output;

    std::vector<std::unordered_map<std::string, std::pair<std::string, std::string>>> symbolTable;

    int tempCounter = 0;
    int labelCounter = 0;
    int stringCounter = 0;

    std::vector<std::pair<std::string, std::string>> globalStrings;
    std::unordered_map<std::string, std::string> stringMap;
    std::unordered_map<std::string, int> arraySizes;

    std::string currentFunction;
    std::string currentReturnType;

    std::string getLLVMType(const std::string &typeName);

    void pushScope();
    void popScope();
    void declareVariable(const std::string &name, const std::string &llvmName, const std::string &type);
    std::pair<std::string, std::string> lookupVariable(const std::string &name); // retorna (nome LLVM, tipo)

    std::string getTempName();
    std::string getLabelName();
    std::string getStringName();

    void collectStrings(const std::shared_ptr<ProgramNode> &node);
    void collectStringsFromStatement(const std::shared_ptr<ASTNode> &stmt);
    void collectStringsFromExpr(const std::shared_ptr<ExprNode> &expr);

    void generateProgram(const std::shared_ptr<ProgramNode> &node);
    void generateVarDecl(const std::shared_ptr<VarDeclNode> &node);
    void generateReturn(const std::shared_ptr<ReturnNode> &node);
    void generateFuncDecl(const std::shared_ptr<FuncDeclNode> &node);

    std::string generateExpr(const std::shared_ptr<ExprNode> &expr);
    std::string generateIdentifier(const std::shared_ptr<IdentifierNode> &node);
    std::string generateLiteral(const std::shared_ptr<LiteralNode> &node);
    std::string generateArrayLiteral(const std::shared_ptr<ArrayLiteralNode> &node);
    std::string generateBinaryOp(const std::shared_ptr<BinaryOpNode> &node);
    std::string generateUnaryOp(const std::shared_ptr<UnaryOpNode> &node);

    void generateIf(const std::shared_ptr<IfNode> &node);
    void generateAssign(const std::shared_ptr<AssignNode> &node);
    void generateShow(const std::shared_ptr<ShowNode> &node);
    void generateDuring(const std::shared_ptr<DuringNode> &node);
    void generateForEach(const std::shared_ptr<ForEachNode> &node);
    void generateRepeat(const std::shared_ptr<RepeatNode> &node);
    void generateStatement(const std::shared_ptr<ASTNode> &node);

    void declarePrintfFunctions();

    double parseNumber(const std::string &str);
    bool isInteger(const std::string &str);

    std::string escapeString(const std::string &str);
    std::string generateCall(const std::shared_ptr<CallNode> &node);
};
