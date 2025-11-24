#pragma once
#include "AST.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>

class CodeGenerator {
public:
    CodeGenerator();
    ~CodeGenerator() = default;

    // Gera código LLVM IR a partir da AST
    std::string generate(const std::shared_ptr<ProgramNode>& program);
    
    // Retorna o IR gerado como string
    const std::string& getIR() const { return irCode; }

private:
    std::string irCode;
    std::ostringstream output;
    
    // Tabela de símbolos para variáveis (nome -> (nome LLVM, tipo))
    std::vector<std::unordered_map<std::string, std::pair<std::string, std::string>>> symbolTable;
    
    // Contadores para gerar nomes únicos
    int tempCounter = 0;
    int labelCounter = 0;
    int stringCounter = 0;
    
    // Armazena strings globais para gerar no início do módulo
    std::vector<std::pair<std::string, std::string>> globalStrings; // (nome, conteúdo)
    std::unordered_map<std::string, std::string> stringMap; // (conteúdo da string -> nome LLVM)
    
    // Função atual sendo gerada
    std::string currentFunction;
    std::string currentReturnType;
    
    // Mapeamento de tipos da linguagem para tipos LLVM
    std::string getLLVMType(const std::string& typeName);
    
    // Gerenciamento de escopos
    void pushScope();
    void popScope();
    void declareVariable(const std::string& name, const std::string& llvmName, const std::string& type);
    std::pair<std::string, std::string> lookupVariable(const std::string& name); // retorna (nome LLVM, tipo)
    
    // Gera nomes únicos
    std::string getTempName();
    std::string getLabelName();
    std::string getStringName();
    
    // Coleta strings globais da AST
    void collectStrings(const std::shared_ptr<ProgramNode>& node);
    void collectStringsFromStatement(const std::shared_ptr<ASTNode>& stmt);
    void collectStringsFromExpr(const std::shared_ptr<ExprNode>& expr);
    
    // Geração de código para nós da AST
    void generateProgram(const std::shared_ptr<ProgramNode>& node);
    void generateVarDecl(const std::shared_ptr<VarDeclNode>& node);
    void generateFuncDecl(const std::shared_ptr<FuncDeclNode>& node);
    void generateClassDecl(const std::shared_ptr<ClassDeclNode>& node);
    void generateContractDecl(const std::shared_ptr<ContractDeclNode>& node);
    
    std::string generateExpr(const std::shared_ptr<ExprNode>& expr);
    std::string generateIdentifier(const std::shared_ptr<IdentifierNode>& node);
    std::string generateLiteral(const std::shared_ptr<LiteralNode>& node);
    std::string generateArrayLiteral(const std::shared_ptr<ArrayLiteralNode>& node);
    std::string generateBinaryOp(const std::shared_ptr<BinaryOpNode>& node);
    std::string generateUnaryOp(const std::shared_ptr<UnaryOpNode>& node);
    
    void generateIf(const std::shared_ptr<IfNode>& node);
    void generateAssign(const std::shared_ptr<AssignNode>& node);
    void generateShow(const std::shared_ptr<ShowNode>& node);
    void generateDuring(const std::shared_ptr<DuringNode>& node);
    void generateForEach(const std::shared_ptr<ForEachNode>& node);
    void generateRepeat(const std::shared_ptr<RepeatNode>& node);
    void generateStatement(const std::shared_ptr<ASTNode>& node);
    
    // Funções auxiliares
    void declarePrintfFunctions();
    
    // Converte string para número
    double parseNumber(const std::string& str);
    bool isInteger(const std::string& str);
    
    // Utilitários para formatação
    std::string escapeString(const std::string& str);
};
