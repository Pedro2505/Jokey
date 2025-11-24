#include "CodeGenerator.h"
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <cctype>

CodeGenerator::CodeGenerator() {
    pushScope();
    output << "; ModuleID = 'jokey_module'\n";
    output << "target datalayout = \"e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128\"\n";
    output << "target triple = \"x86_64-pc-windows-msvc\"\n\n";
}

std::string CodeGenerator::generate(const std::shared_ptr<ProgramNode>& program) {
    // Primeiro, percorre a AST para coletar todas as strings
    // (isso garante que as strings sejam geradas antes de serem usadas)
    collectStrings(program);
    
    // Agora gera o código
    generateProgram(program);
    irCode = output.str();
    return irCode;
}

std::string CodeGenerator::getLLVMType(const std::string& typeName) {
    // Verifica se é array (termina com [])
    if (typeName.size() >= 2 && typeName.substr(typeName.size() - 2) == "[]") {
        // Arrays são tratados como ponteiros para o tipo base
        std::string baseType = typeName.substr(0, typeName.size() - 2);
        if (baseType == "string" || baseType == "str") {
            return "i8**"; // Ponteiro para ponteiro de char (array de strings)
        } else if (baseType == "int" || baseType == "integer") {
            return "i32*"; // Ponteiro para i32 (array de inteiros)
        } else {
            return "i8*"; // Por padrão, array é ponteiro genérico
        }
    } else if (typeName == "int" || typeName == "integer") {
        return "i32";
    } else if (typeName == "float" || typeName == "real") {
        return "double";
    } else if (typeName == "bool" || typeName == "boolean") {
        return "i1";
    } else if (typeName == "string" || typeName == "str") {
        return "i8*";
    } else if (typeName == "void") {
        return "void";
    }
    // Tipo desconhecido, assume i32
    return "i32";
}

void CodeGenerator::pushScope() {
    symbolTable.emplace_back();
}

void CodeGenerator::popScope() {
    if (!symbolTable.empty()) {
        symbolTable.pop_back();
    }
}

void CodeGenerator::declareVariable(const std::string& name, const std::string& llvmName, const std::string& type) {
    if (!symbolTable.empty()) {
        symbolTable.back()[name] = std::make_pair(llvmName, type);
    }
}

std::pair<std::string, std::string> CodeGenerator::lookupVariable(const std::string& name) {
    // Procura do escopo mais interno para o mais externo
    for (auto it = symbolTable.rbegin(); it != symbolTable.rend(); ++it) {
        auto found = it->find(name);
        if (found != it->end()) {
            return found->second;
        }
    }
    return std::make_pair("", "");
}

std::string CodeGenerator::getTempName() {
    return "%" + std::to_string(tempCounter++);
}

std::string CodeGenerator::getLabelName() {
    return "label" + std::to_string(labelCounter++);
}

std::string CodeGenerator::getStringName() {
    return "@.str" + std::to_string(stringCounter++);
}

void CodeGenerator::collectStrings(const std::shared_ptr<ProgramNode>& node) {
    // Coleta strings de declarações
    for (auto& decl : node->declarations) {
        if (auto varDecl = std::dynamic_pointer_cast<VarDeclNode>(decl)) {
            if (varDecl->init) {
                auto exprInit = std::dynamic_pointer_cast<ExprNode>(varDecl->init);
                if (exprInit) {
                    collectStringsFromExpr(exprInit);
                }
            }
        } else if (auto funcDecl = std::dynamic_pointer_cast<FuncDeclNode>(decl)) {
            if (funcDecl->regressExpr) {
                auto exprRegress = std::dynamic_pointer_cast<ExprNode>(funcDecl->regressExpr);
                if (exprRegress) {
                    collectStringsFromExpr(exprRegress);
                }
            }
            for (auto& stmt : funcDecl->body) {
                collectStringsFromStatement(stmt);
            }
        }
    }
    
    // Coleta strings de statements
    for (auto& stmt : node->statements) {
        collectStringsFromStatement(stmt);
    }
}

void CodeGenerator::collectStringsFromStatement(const std::shared_ptr<ASTNode>& stmt) {
    if (auto showNode = std::dynamic_pointer_cast<ShowNode>(stmt)) {
        for (auto& arg : showNode->args) {
            collectStringsFromExpr(arg);
        }
    } else if (auto ifNode = std::dynamic_pointer_cast<IfNode>(stmt)) {
        collectStringsFromExpr(ifNode->cond);
        for (auto& elsif : ifNode->elsi) {
            collectStringsFromExpr(elsif.first);
            for (auto& st : elsif.second) {
                collectStringsFromStatement(st);
            }
        }
        for (auto& st : ifNode->thenBranch) {
            collectStringsFromStatement(st);
        }
        for (auto& st : ifNode->elseBranch) {
            collectStringsFromStatement(st);
        }
    } else if (auto duringNode = std::dynamic_pointer_cast<DuringNode>(stmt)) {
        collectStringsFromExpr(duringNode->cond);
        for (auto& st : duringNode->body) {
            collectStringsFromStatement(st);
        }
    } else if (auto repeatNode = std::dynamic_pointer_cast<RepeatNode>(stmt)) {
        collectStringsFromExpr(repeatNode->untilExpr);
        for (auto& st : repeatNode->body) {
            collectStringsFromStatement(st);
        }
    } else if (auto forEachNode = std::dynamic_pointer_cast<ForEachNode>(stmt)) {
        collectStringsFromExpr(forEachNode->toExpr);
        for (auto& st : forEachNode->body) {
            collectStringsFromStatement(st);
        }
    } else if (auto assignNode = std::dynamic_pointer_cast<AssignNode>(stmt)) {
        collectStringsFromExpr(assignNode->expr);
    }
}

void CodeGenerator::collectStringsFromExpr(const std::shared_ptr<ExprNode>& expr) {
    if (auto lit = std::dynamic_pointer_cast<LiteralNode>(expr)) {
        std::string litStr = lit->lit;
        // Se for string, coleta
        if (litStr.size() >= 2 && (litStr[0] == '"' || litStr[0] == '\'') && litStr.back() == litStr[0]) {
            std::string str = litStr.substr(1, litStr.size() - 2);
            // Verifica se já foi coletada
            if (stringMap.find(str) == stringMap.end()) {
                std::string strName = getStringName();
                std::ostringstream strDef;
                strDef << "[" << (str.size() + 1) << " x i8] c\"" << escapeString(str) << "\\00\"";
                globalStrings.push_back(std::make_pair(strName, strDef.str()));
                stringMap[str] = strName;
            }
        }
    } else if (auto bin = std::dynamic_pointer_cast<BinaryOpNode>(expr)) {
        collectStringsFromExpr(bin->left);
        collectStringsFromExpr(bin->right);
    } else if (auto un = std::dynamic_pointer_cast<UnaryOpNode>(expr)) {
        collectStringsFromExpr(un->operand);
    } else if (auto arr = std::dynamic_pointer_cast<ArrayLiteralNode>(expr)) {
        for (auto& el : arr->elements) {
            collectStringsFromExpr(el);
        }
    }
}

void CodeGenerator::generateProgram(const std::shared_ptr<ProgramNode>& node) {
    // Declara funções auxiliares de printf
    declarePrintfFunctions();
    
    // Gera strings globais coletadas (ANTES de qualquer função)
    for (const auto& strPair : globalStrings) {
        output << strPair.first << " = private unnamed_addr constant " << strPair.second << "\n";
    }
    if (!globalStrings.empty()) {
        output << "\n";
    }
    
    // Gera declarações globais (variáveis globais)
    for (auto& decl : node->declarations) {
        if (auto varDecl = std::dynamic_pointer_cast<VarDeclNode>(decl)) {
            if (varDecl->isStatic) {
                generateVarDecl(varDecl);
            }
        } else if (auto funcDecl = std::dynamic_pointer_cast<FuncDeclNode>(decl)) {
            generateFuncDecl(funcDecl);
        } else if (auto classDecl = std::dynamic_pointer_cast<ClassDeclNode>(decl)) {
            generateClassDecl(classDecl);
        } else if (auto contractDecl = std::dynamic_pointer_cast<ContractDeclNode>(decl)) {
            generateContractDecl(contractDecl);
        }
    }
    
    // Gera função main se houver statements no nível global
    if (!node->statements.empty()) {
        output << "\ndefine i32 @main() {\n";
        output << "entry:\n";
        
        currentFunction = "main";
        currentReturnType = "i32";
        pushScope();
        
        // Primeiro, gera todas as declarações de variáveis locais
        for (auto& decl : node->declarations) {
            if (auto varDecl = std::dynamic_pointer_cast<VarDeclNode>(decl)) {
                if (!varDecl->isStatic) {
                    generateVarDecl(varDecl);
                }
            }
        }
        
        // Depois, gera os statements
        for (auto& stmt : node->statements) {
            generateStatement(stmt);
        }
        
        output << "  ret i32 0\n";
        output << "}\n";
        
        popScope();
        currentFunction = "";
    }
}

void CodeGenerator::generateVarDecl(const std::shared_ptr<VarDeclNode>& node) {
    std::string type = getLLVMType(node->typeName);
    std::string varName = node->ident;
    std::string llvmVarName;
    
    if (currentFunction.empty()) {
        // Variável global
        llvmVarName = "@" + varName;
        output << "@" << varName << " = ";
        
        std::string initValue;
        if (node->init) {
            auto exprInit = std::dynamic_pointer_cast<ExprNode>(node->init);
            if (exprInit) {
                initValue = generateExpr(exprInit);
            }
            // Para globais, precisa ser constante
            if (!initValue.empty()) {
                if (type == "i32") {
                    output << "global i32 " << initValue << "\n";
                } else if (type == "double") {
                    output << "global double " << initValue << "\n";
                } else if (type == "i1") {
                    output << "global i1 " << initValue << "\n";
                } else {
                    output << "global " << type << " null\n";
                }
            } else {
                // Se não conseguiu gerar valor, inicializa com zero
                if (type == "i32") {
                    output << "global i32 0\n";
                } else if (type == "double") {
                    output << "global double 0.000000e+00\n";
                } else if (type == "i1") {
                    output << "global i1 false\n";
                } else {
                    output << "global " << type << " null\n";
                }
            }
        } else {
            if (type == "i32") {
                output << "global i32 0\n";
            } else if (type == "double") {
                output << "global double 0.000000e+00\n";
            } else if (type == "i1") {
                output << "global i1 false\n";
            } else {
                output << "global " << type << " null\n";
            }
        }
        declareVariable(varName, llvmVarName, node->typeName);
    } else {
        // Variável local
        llvmVarName = getTempName();
        output << "  " << llvmVarName << " = alloca " << type << "\n";
        
        if (node->init) {
            auto exprInit = std::dynamic_pointer_cast<ExprNode>(node->init);
            std::string initValue;
            if (exprInit) {
                initValue = generateExpr(exprInit);
            }
            
            if (!initValue.empty()) {
                std::string initType = type;
                
                // Verifica se initValue é "null" e o tipo não é ponteiro
                if (initValue == "null") {
                    if (type.find("*") != std::string::npos) {
                        // Tipo é ponteiro, pode usar null
                        output << "  store " << initType << " null, " << initType << "* " << llvmVarName << "\n";
                    } else {
                        // Não pode usar null com tipos não-ponteiro, inicializa com zero
                        if (type == "i32") {
                            output << "  store i32 0, i32* " << llvmVarName << "\n";
                        } else if (type == "double") {
                            output << "  store double 0.000000e+00, double* " << llvmVarName << "\n";
                        } else if (type == "i1") {
                            output << "  store i1 false, i1* " << llvmVarName << "\n";
                        }
                    }
                } else {
                    // Conversão de tipo se necessário
                    if (type == "double" && initValue.find("i32") != std::string::npos) {
                        std::string temp = getTempName();
                        output << "  " << temp << " = sitofp i32 " << initValue << " to double\n";
                        initValue = temp;
                    } else if (type == "i32" && initValue.find("double") != std::string::npos) {
                        std::string temp = getTempName();
                        output << "  " << temp << " = fptosi double " << initValue << " to i32\n";
                        initValue = temp;
                    }
                    output << "  store " << initType << " " << initValue << ", " << initType << "* " << llvmVarName << "\n";
                }
            } else {
                // Se não conseguiu gerar valor, inicializa com zero
                if (type == "i32") {
                    output << "  store i32 0, i32* " << llvmVarName << "\n";
                } else if (type == "double") {
                    output << "  store double 0.000000e+00, double* " << llvmVarName << "\n";
                } else if (type == "i1") {
                    output << "  store i1 false, i1* " << llvmVarName << "\n";
                }
            }
        } else {
            // Inicializa com zero
            if (type == "i32") {
                output << "  store i32 0, i32* " << llvmVarName << "\n";
            } else if (type == "double") {
                output << "  store double 0.000000e+00, double* " << llvmVarName << "\n";
            } else if (type == "i1") {
                output << "  store i1 false, i1* " << llvmVarName << "\n";
            }
        }
        
        declareVariable(varName, llvmVarName, node->typeName);
    }
}

void CodeGenerator::generateFuncDecl(const std::shared_ptr<FuncDeclNode>& node) {
    std::string retType = getLLVMType(node->retType);
    std::string funcName = "@" + node->name;
    
    output << "\ndefine " << retType << " " << funcName << "(";
    
    std::vector<std::string> paramNames;
    for (size_t i = 0; i < node->params.size(); i++) {
        if (i > 0) output << ", ";
        std::string paramType = getLLVMType(node->params[i].first);
        std::string paramName = getTempName();
        output << paramType << " %" << (i + 1);
        paramNames.push_back(paramName);
    }
    
    output << ") {\n";
    output << "entry:\n";
    
    std::string prevFunction = currentFunction;
    std::string prevReturnType = currentReturnType;
    currentFunction = node->name;
    currentReturnType = retType;
    
    pushScope();
    
    // Aloca e armazena parâmetros
    for (size_t i = 0; i < node->params.size(); i++) {
        std::string paramType = getLLVMType(node->params[i].first);
        std::string allocaName = getTempName();
        std::string paramVar = "%" + std::to_string(i + 1);
        
        output << "  " << allocaName << " = alloca " << paramType << "\n";
        output << "  store " << paramType << " " << paramVar << ", " << paramType << "* " << allocaName << "\n";
        declareVariable(node->params[i].second, allocaName, node->params[i].first);
    }
    
    // Gera o corpo da função
    for (auto& stmt : node->body) {
        generateStatement(stmt);
    }
    
    // Gera o retorno
    if (node->regressExpr) {
        auto exprRegress = std::dynamic_pointer_cast<ExprNode>(node->regressExpr);
        std::string retVal;
        if (exprRegress) {
            retVal = generateExpr(exprRegress);
        }
        std::string retTypeStr = retType;
        
        if (!retVal.empty()) {
            // Conversão de tipo se necessário
            if (retType == "double" && retVal.find("i32") != std::string::npos) {
                std::string temp = getTempName();
                output << "  " << temp << " = sitofp i32 " << retVal << " to double\n";
                retVal = temp;
            } else if (retType == "i32" && retVal.find("double") != std::string::npos) {
                std::string temp = getTempName();
                output << "  " << temp << " = fptosi double " << retVal << " to i32\n";
                retVal = temp;
            }
            
            output << "  ret " << retTypeStr << " " << retVal << "\n";
        } else {
            // Se não conseguiu gerar valor, retorna valor padrão
            if (retType == "i32") {
                output << "  ret i32 0\n";
            } else if (retType == "double") {
                output << "  ret double 0.000000e+00\n";
            } else {
                output << "  ret " << retTypeStr << " 0\n";
            }
        }
    } else if (retType == "void") {
        output << "  ret void\n";
    } else {
        // Retorna valor padrão
        if (retType == "i32") {
            output << "  ret i32 0\n";
        } else if (retType == "double") {
            output << "  ret double 0.000000e+00\n";
        } else {
            output << "  ret " << retType << " 0\n";
        }
    }
    
    output << "}\n";
    
    popScope();
    currentFunction = prevFunction;
    currentReturnType = prevReturnType;
}

void CodeGenerator::generateClassDecl(const std::shared_ptr<ClassDeclNode>& node) {
    // Classes podem ser implementadas como structs LLVM
    // Por enquanto, apenas declara (implementação simplificada)
}

void CodeGenerator::generateContractDecl(const std::shared_ptr<ContractDeclNode>& node) {
    // Contracts podem ser implementados como interfaces
    // Por enquanto, apenas declara (implementação simplificada)
}

std::string CodeGenerator::generateExpr(const std::shared_ptr<ExprNode>& expr) {
    if (auto id = std::dynamic_pointer_cast<IdentifierNode>(expr)) {
        return generateIdentifier(id);
    } else if (auto lit = std::dynamic_pointer_cast<LiteralNode>(expr)) {
        return generateLiteral(lit);
    } else if (auto arr = std::dynamic_pointer_cast<ArrayLiteralNode>(expr)) {
        return generateArrayLiteral(arr);
    } else if (auto bin = std::dynamic_pointer_cast<BinaryOpNode>(expr)) {
        return generateBinaryOp(bin);
    } else if (auto un = std::dynamic_pointer_cast<UnaryOpNode>(expr)) {
        return generateUnaryOp(un);
    }
    return "0";
}

std::string CodeGenerator::generateIdentifier(const std::shared_ptr<IdentifierNode>& node) {
    auto varInfo = lookupVariable(node->name);
    std::string varName = varInfo.first;
    std::string varType = varInfo.second;
    
    if (varName.empty()) {
        // Variável não encontrada - retorna string vazia para indicar erro
        return "";
    }
    
    // Usa o tipo anotado se disponível, senão usa o tipo da tabela de símbolos
    if (!node->annotatedType.empty()) {
        varType = node->annotatedType;
    }
    
    std::string llvmType = getLLVMType(varType);
    
    // Se for alloca (variável local) ou global, precisa fazer load
    if (varName[0] == '%' || varName[0] == '@') {
        std::string temp = getTempName();
        output << "  " << temp << " = load " << llvmType << ", " << llvmType << "* " << varName << "\n";
        return temp;
    }
    
    return varName;
}

std::string CodeGenerator::generateLiteral(const std::shared_ptr<LiteralNode>& node) {
    std::string lit = node->lit;
    
    // Remove aspas de strings
    if (lit.size() >= 2 && (lit[0] == '"' || lit[0] == '\'') && lit.back() == lit[0]) {
        std::string str = lit.substr(1, lit.size() - 2);
        
        // Busca o nome LLVM da string (DEVE ter sido coletada na fase de coleta)
        std::string strName;
        if (stringMap.find(str) != stringMap.end()) {
            strName = stringMap[str];
        } else {
            // Se não foi coletada, isso é um erro, mas vamos criar agora como fallback
            // (não deveria acontecer se a coleta estiver correta)
            strName = getStringName();
            std::ostringstream strDef;
            strDef << "[" << (str.size() + 1) << " x i8] c\"" << escapeString(str) << "\\00\"";
            // NÃO adiciona a globalStrings aqui - isso causaria o problema!
            // Em vez disso, retorna um valor que indica erro
            return "null"; // ou podemos lançar uma exceção
        }
        
        // Gera o getelementptr dentro da função atual
        std::string temp = getTempName();
        output << "  " << temp << " = getelementptr inbounds [" << (str.size() + 1) 
               << " x i8], [" << (str.size() + 1) << " x i8]* " << strName << ", i64 0, i64 0\n";
        return temp;
    }
    
    // Booleanos (case insensitive)
    std::string litLower = lit;
    std::transform(litLower.begin(), litLower.end(), litLower.begin(), ::tolower);
    if (litLower == "true" || litLower == "verdadeiro") {
        return "1";
    }
    if (litLower == "false" || litLower == "falso") {
        return "0";
    }
    
    // Números - verifica se é um número válido antes de converter
    if (isInteger(lit)) {
        try {
            int val = static_cast<int>(parseNumber(lit));
            return std::to_string(val);
        } catch (...) {
            return "0";
        }
    } else {
        // Tenta converter como float
        try {
            // Verifica se contém apenas dígitos, ponto, sinal de menos/mais, ou 'e'/'E' para notação científica
            bool isValidFloat = true;
            bool hasDot = false;
            bool hasE = false;
            for (size_t i = 0; i < lit.size(); i++) {
                char c = lit[i];
                if (c == '.' && !hasDot) {
                    hasDot = true;
                } else if ((c == 'e' || c == 'E') && !hasE) {
                    hasE = true;
                } else if (c != '-' && c != '+' && (c < '0' || c > '9')) {
                    isValidFloat = false;
                    break;
                }
            }
            
            if (isValidFloat) {
                double val = parseNumber(lit);
                std::ostringstream oss;
                oss << std::scientific << std::setprecision(6) << val;
                return oss.str();
            } else {
                return "0"; // Não é um número válido
            }
        } catch (...) {
            return "0"; // Erro ao converter
        }
    }
}

std::string CodeGenerator::generateArrayLiteral(const std::shared_ptr<ArrayLiteralNode>& node) {
    // Implementação simplificada - arrays podem ser implementados como structs ou ponteiros
    // Por enquanto retorna null (precisa de implementação mais complexa)
    return "null";
}

std::string CodeGenerator::generateBinaryOp(const std::shared_ptr<BinaryOpNode>& node) {
    std::string left = generateExpr(node->left);
    std::string right = generateExpr(node->right);
    std::string op = node->op;
    
    if (left.empty() || right.empty()) {
        return "0";
    }
    
    std::string result = getTempName();
    
    // Determina os tipos usando annotatedType
    std::string leftType = node->left->annotatedType;
    std::string rightType = node->right->annotatedType;
    
    // Se não tiver tipo anotado, tenta inferir
    if (leftType.empty()) {
        // Se for um número literal sem ponto, assume inteiro
        if (left.find(".") == std::string::npos && left.find("e") == std::string::npos && 
            left.find("E") == std::string::npos && left[0] != '%' && left[0] != '@') {
            leftType = "integer";
        } else if (left.find(".") != std::string::npos || left.find("e") != std::string::npos || 
                   left.find("E") != std::string::npos) {
            leftType = "float";
        } else {
            leftType = "integer"; // Assume inteiro por padrão
        }
    }
    
    if (rightType.empty()) {
        // Se for um número literal sem ponto, assume inteiro
        if (right.find(".") == std::string::npos && right.find("e") == std::string::npos && 
            right.find("E") == std::string::npos && right[0] != '%' && right[0] != '@') {
            rightType = "integer";
        } else if (right.find(".") != std::string::npos || right.find("e") != std::string::npos || 
                   right.find("E") != std::string::npos) {
            rightType = "float";
        } else {
            rightType = "integer"; // Assume inteiro por padrão
        }
    }
    
    std::string leftLLVMType = getLLVMType(leftType);
    std::string rightLLVMType = getLLVMType(rightType);
    
    bool leftIsInt = (leftLLVMType == "i32");
    bool rightIsInt = (rightLLVMType == "i32");
    
    // Operações aritméticas
    if (op == "+") {
        if (leftIsInt && rightIsInt) {
            output << "  " << result << " = add i32 " << left << ", " << right << "\n";
            // Anota o tipo do resultado
            node->annotatedType = "integer";
        } else {
            // Converte para double se necessário
            if (leftIsInt) {
                std::string temp = getTempName();
                output << "  " << temp << " = sitofp i32 " << left << " to double\n";
                left = temp;
            }
            if (rightIsInt) {
                std::string temp = getTempName();
                output << "  " << temp << " = sitofp i32 " << right << " to double\n";
                right = temp;
            }
            output << "  " << result << " = fadd double " << left << ", " << right << "\n";
            // Anota o tipo do resultado
            node->annotatedType = "float";
        }
    } else if (op == "-") {
        if (leftIsInt && rightIsInt) {
            output << "  " << result << " = sub i32 " << left << ", " << right << "\n";
            node->annotatedType = "integer";
        } else {
            if (leftIsInt) {
                std::string temp = getTempName();
                output << "  " << temp << " = sitofp i32 " << left << " to double\n";
                left = temp;
            }
            if (rightIsInt) {
                std::string temp = getTempName();
                output << "  " << temp << " = sitofp i32 " << right << " to double\n";
                right = temp;
            }
            output << "  " << result << " = fsub double " << left << ", " << right << "\n";
            node->annotatedType = "float";
        }
    } else if (op == "*") {
        if (leftIsInt && rightIsInt) {
            output << "  " << result << " = mul i32 " << left << ", " << right << "\n";
            node->annotatedType = "integer";
        } else {
            if (leftIsInt) {
                std::string temp = getTempName();
                output << "  " << temp << " = sitofp i32 " << left << " to double\n";
                left = temp;
            }
            if (rightIsInt) {
                std::string temp = getTempName();
                output << "  " << temp << " = sitofp i32 " << right << " to double\n";
                right = temp;
            }
            output << "  " << result << " = fmul double " << left << ", " << right << "\n";
            node->annotatedType = "float";
        }
    } else if (op == "/") {
        if (leftIsInt && rightIsInt) {
            output << "  " << result << " = sdiv i32 " << left << ", " << right << "\n";
            node->annotatedType = "integer";
        } else {
            if (leftIsInt) {
                std::string temp = getTempName();
                output << "  " << temp << " = sitofp i32 " << left << " to double\n";
                left = temp;
            }
            if (rightIsInt) {
                std::string temp = getTempName();
                output << "  " << temp << " = sitofp i32 " << right << " to double\n";
                right = temp;
            }
            output << "  " << result << " = fdiv double " << left << ", " << right << "\n";
            node->annotatedType = "float";
        }
    } else if (op == "%") {
        if (leftIsInt && rightIsInt) {
            output << "  " << result << " = srem i32 " << left << ", " << right << "\n";
        }
    }
    // Operações de comparação
    else if (op == "==" || op == "=") {
        if (leftIsInt && rightIsInt) {
            output << "  " << result << " = icmp eq i32 " << left << ", " << right << "\n";
        } else {
            if (leftIsInt) {
                std::string temp = getTempName();
                output << "  " << temp << " = sitofp i32 " << left << " to double\n";
                left = temp;
            }
            if (rightIsInt) {
                std::string temp = getTempName();
                output << "  " << temp << " = sitofp i32 " << right << " to double\n";
                right = temp;
            }
            output << "  " << result << " = fcmp oeq double " << left << ", " << right << "\n";
        }
    } else if (op == "!=" || op == "<>") {
        if (leftIsInt && rightIsInt) {
            output << "  " << result << " = icmp ne i32 " << left << ", " << right << "\n";
        } else {
            if (leftIsInt) {
                std::string temp = getTempName();
                output << "  " << temp << " = sitofp i32 " << left << " to double\n";
                left = temp;
            }
            if (rightIsInt) {
                std::string temp = getTempName();
                output << "  " << temp << " = sitofp i32 " << right << " to double\n";
                right = temp;
            }
            output << "  " << result << " = fcmp one double " << left << ", " << right << "\n";
        }
    } else if (op == "<") {
        if (leftIsInt && rightIsInt) {
            output << "  " << result << " = icmp slt i32 " << left << ", " << right << "\n";
        } else {
            if (leftIsInt) {
                std::string temp = getTempName();
                output << "  " << temp << " = sitofp i32 " << left << " to double\n";
                left = temp;
            }
            if (rightIsInt) {
                std::string temp = getTempName();
                output << "  " << temp << " = sitofp i32 " << right << " to double\n";
                right = temp;
            }
            output << "  " << result << " = fcmp olt double " << left << ", " << right << "\n";
        }
    } else if (op == ">") {
        if (leftIsInt && rightIsInt) {
            output << "  " << result << " = icmp sgt i32 " << left << ", " << right << "\n";
        } else {
            if (leftIsInt) {
                std::string temp = getTempName();
                output << "  " << temp << " = sitofp i32 " << left << " to double\n";
                left = temp;
            }
            if (rightIsInt) {
                std::string temp = getTempName();
                output << "  " << temp << " = sitofp i32 " << right << " to double\n";
                right = temp;
            }
            output << "  " << result << " = fcmp ogt double " << left << ", " << right << "\n";
        }
    } else if (op == "<=") {
        if (leftIsInt && rightIsInt) {
            output << "  " << result << " = icmp sle i32 " << left << ", " << right << "\n";
        } else {
            if (leftIsInt) {
                std::string temp = getTempName();
                output << "  " << temp << " = sitofp i32 " << left << " to double\n";
                left = temp;
            }
            if (rightIsInt) {
                std::string temp = getTempName();
                output << "  " << temp << " = sitofp i32 " << right << " to double\n";
                right = temp;
            }
            output << "  " << result << " = fcmp ole double " << left << ", " << right << "\n";
        }
    } else if (op == ">=") {
        if (leftIsInt && rightIsInt) {
            output << "  " << result << " = icmp sge i32 " << left << ", " << right << "\n";
        } else {
            if (leftIsInt) {
                std::string temp = getTempName();
                output << "  " << temp << " = sitofp i32 " << left << " to double\n";
                left = temp;
            }
            if (rightIsInt) {
                std::string temp = getTempName();
                output << "  " << temp << " = sitofp i32 " << right << " to double\n";
                right = temp;
            }
            output << "  " << result << " = fcmp oge double " << left << ", " << right << "\n";
        }
    }
    // Operações lógicas
    else if (op == "&&" || op == "e") {
        output << "  " << result << " = and i1 " << left << ", " << right << "\n";
    } else if (op == "||" || op == "ou") {
        output << "  " << result << " = or i1 " << left << ", " << right << "\n";
    }
    
    return result;
}

std::string CodeGenerator::generateUnaryOp(const std::shared_ptr<UnaryOpNode>& node) {
    std::string operand = generateExpr(node->operand);
    std::string op = node->op;
    
    if (operand.empty()) {
        return "0";
    }
    
    std::string result = getTempName();
    
    if (op == "-") {
        bool isInt = operand.find("i32") != std::string::npos || 
                     (operand.find("double") == std::string::npos && operand.find("@") == std::string::npos && operand.find("%") == std::string::npos);
        if (isInt) {
            output << "  " << result << " = sub i32 0, " << operand << "\n";
        } else {
            output << "  " << result << " = fsub double 0.000000e+00, " << operand << "\n";
        }
    } else if (op == "!" || op == "nao") {
        output << "  " << result << " = xor i1 " << operand << ", true\n";
    } else if (op == "+") {
        return operand; // Unário + não faz nada
    }
    
    return result;
}

void CodeGenerator::generateIf(const std::shared_ptr<IfNode>& node) {
    std::string cond = generateExpr(node->cond);
    if (cond.empty()) {
        return;
    }
    
    std::string thenLabel = getLabelName();
    std::string elseLabel = getLabelName();
    std::string mergeLabel = getLabelName();
    
    // Converte condição para i1 se necessário
    std::string condBool = cond;
    if (cond.find("i1") == std::string::npos) {
        condBool = getTempName();
        output << "  " << condBool << " = icmp ne i32 " << cond << ", 0\n";
    }
    
    output << "  br i1 " << condBool << ", label %" << thenLabel << ", label %" << elseLabel << "\n";
    
    output << "\n" << thenLabel << ":\n";
    pushScope();
    for (auto& stmt : node->thenBranch) {
        generateStatement(stmt);
    }
    popScope();
    output << "  br label %" << mergeLabel << "\n";
    
    // Elsif
    for (size_t i = 0; i < node->elsi.size(); i++) {
        output << "\n" << elseLabel << ":\n";
        std::string elsifCond = generateExpr(node->elsi[i].first);
        std::string elsifCondBool = elsifCond;
        if (elsifCond.find("i1") == std::string::npos) {
            elsifCondBool = getTempName();
            output << "  " << elsifCondBool << " = icmp ne i32 " << elsifCond << ", 0\n";
        }
        
        std::string elsifBodyLabel = getLabelName();
        elseLabel = getLabelName();
        output << "  br i1 " << elsifCondBool << ", label %" << elsifBodyLabel << ", label %" << elseLabel << "\n";
        
        output << "\n" << elsifBodyLabel << ":\n";
        pushScope();
        for (auto& stmt : node->elsi[i].second) {
            generateStatement(stmt);
        }
        popScope();
        output << "  br label %" << mergeLabel << "\n";
    }
    
    // Else
    if (!node->elseBranch.empty()) {
        output << "\n" << elseLabel << ":\n";
        pushScope();
        for (auto& stmt : node->elseBranch) {
            generateStatement(stmt);
        }
        popScope();
        output << "  br label %" << mergeLabel << "\n";
    } else {
        output << "\n" << elseLabel << ":\n";
        output << "  br label %" << mergeLabel << "\n";
    }
    
    output << "\n" << mergeLabel << ":\n";
}

void CodeGenerator::generateAssign(const std::shared_ptr<AssignNode>& node) {
    auto varInfo = lookupVariable(node->target);
    std::string varName = varInfo.first;
    std::string varType = varInfo.second;
    
    if (varName.empty()) {
        return;
    }
    
    std::string value = generateExpr(node->expr);
    if (value.empty()) {
        return;
    }
    
    std::string llvmType = getLLVMType(varType);
    
    // Conversão de tipo se necessário
    std::string exprType = node->expr->annotatedType;
    if (!exprType.empty() && exprType != varType) {
        std::string llvmExprType = getLLVMType(exprType);
        if (llvmType == "double" && llvmExprType == "i32") {
            std::string temp = getTempName();
            output << "  " << temp << " = sitofp i32 " << value << " to double\n";
            value = temp;
        } else if (llvmType == "i32" && llvmExprType == "double") {
            std::string temp = getTempName();
            output << "  " << temp << " = fptosi double " << value << " to i32\n";
            value = temp;
        }
    }
    
    output << "  store " << llvmType << " " << value << ", " << llvmType << "* " << varName << "\n";
}

void CodeGenerator::generateShow(const std::shared_ptr<ShowNode>& node) {
    for (auto& arg : node->args) {
        std::string val = generateExpr(arg);
        if (val.empty() || val == "0" || val == "null") {
            // Se não conseguiu gerar a expressão ou retornou valor inválido, pula
            continue;
        }
        
        // Determina o tipo usando o annotatedType da expressão
        std::string exprType = arg->annotatedType;
        if (exprType.empty()) {
            // Se não tiver tipo anotado, tenta inferir pelo valor
            if (val.find("@.str") != std::string::npos || val[0] == '%') {
                // Se começa com % e foi gerado por getelementptr, provavelmente é string
                // Verifica se o tipo anotado está vazio mas o valor parece ser ponteiro
                exprType = "string";
            } else {
                exprType = "integer"; // Assume inteiro por padrão
            }
        }
        
        // Converte para tipo LLVM
        std::string llvmType = getLLVMType(exprType);
        
        // Chama printf com o tipo correto
        if (llvmType == "i8*") {
            // String - verifica se val é realmente um ponteiro válido
            if (val == "0" || val == "null") {
                // Valor inválido, não imprime
                continue;
            }
            output << "  call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str_fmt_s, i32 0, i32 0), i8* " << val << ")\n";
        } else if (llvmType == "double") {
            // Float
            output << "  call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str_fmt_f, i32 0, i32 0), double " << val << ")\n";
        } else if (llvmType == "i1") {
            // Boolean - converte para i32 primeiro
            std::string intVal = getTempName();
            output << "  " << intVal << " = zext i1 " << val << " to i32\n";
            output << "  call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str_fmt_d, i32 0, i32 0), i32 " << intVal << ")\n";
        } else {
            // Int (i32)
            output << "  call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str_fmt_d, i32 0, i32 0), i32 " << val << ")\n";
        }
    }
}

void CodeGenerator::generateDuring(const std::shared_ptr<DuringNode>& node) {
    std::string condLabel = getLabelName();
    std::string bodyLabel = getLabelName();
    std::string afterLabel = getLabelName();
    
    output << "  br label %" << condLabel << "\n";
    
    output << "\n" << condLabel << ":\n";
    std::string cond = generateExpr(node->cond);
    std::string condBool = cond;
    if (cond.find("i1") == std::string::npos) {
        condBool = getTempName();
        output << "  " << condBool << " = icmp ne i32 " << cond << ", 0\n";
    }
    output << "  br i1 " << condBool << ", label %" << bodyLabel << ", label %" << afterLabel << "\n";
    
    output << "\n" << bodyLabel << ":\n";
    pushScope();
    for (auto& stmt : node->body) {
        generateStatement(stmt);
    }
    popScope();
    output << "  br label %" << condLabel << "\n";
    
    output << "\n" << afterLabel << ":\n";
}

void CodeGenerator::generateForEach(const std::shared_ptr<ForEachNode>& node) {
    // Implementação simplificada
    pushScope();
    
    std::string iterVar = getTempName();
    output << "  " << iterVar << " = alloca i32\n";
    output << "  store i32 0, i32* " << iterVar << "\n";
    declareVariable(node->ident, iterVar, "int");
    
    std::string condLabel = getLabelName();
    std::string bodyLabel = getLabelName();
    std::string afterLabel = getLabelName();
    
    output << "  br label %" << condLabel << "\n";
    
    output << "\n" << condLabel << ":\n";
    std::string iterVal = getTempName();
    output << "  " << iterVal << " = load i32, i32* " << iterVar << "\n";
    std::string cond = getTempName();
    output << "  " << cond << " = icmp slt i32 " << iterVal << ", 10\n";
    output << "  br i1 " << cond << ", label %" << bodyLabel << ", label %" << afterLabel << "\n";
    
    output << "\n" << bodyLabel << ":\n";
    for (auto& stmt : node->body) {
        generateStatement(stmt);
    }
    std::string nextVal = getTempName();
    output << "  " << nextVal << " = add i32 " << iterVal << ", 1\n";
    output << "  store i32 " << nextVal << ", i32* " << iterVar << "\n";
    output << "  br label %" << condLabel << "\n";
    
    output << "\n" << afterLabel << ":\n";
    popScope();
}

void CodeGenerator::generateRepeat(const std::shared_ptr<RepeatNode>& node) {
    std::string bodyLabel = getLabelName();
    std::string condLabel = getLabelName();
    std::string afterLabel = getLabelName();
    
    output << "  br label %" << bodyLabel << "\n";
    
    output << "\n" << bodyLabel << ":\n";
    pushScope();
    for (auto& stmt : node->body) {
        generateStatement(stmt);
    }
    popScope();
    output << "  br label %" << condLabel << "\n";
    
    output << "\n" << condLabel << ":\n";
    std::string cond = generateExpr(node->untilExpr);
    std::string condBool = cond;
    if (cond.find("i1") == std::string::npos) {
        condBool = getTempName();
        output << "  " << condBool << " = icmp ne i32 " << cond << ", 0\n";
    }
    output << "  br i1 " << condBool << ", label %" << afterLabel << ", label %" << bodyLabel << "\n";
    
    output << "\n" << afterLabel << ":\n";
}

void CodeGenerator::generateStatement(const std::shared_ptr<ASTNode>& node) {
    if (auto ifNode = std::dynamic_pointer_cast<IfNode>(node)) {
        generateIf(ifNode);
    } else if (auto assignNode = std::dynamic_pointer_cast<AssignNode>(node)) {
        generateAssign(assignNode);
    } else if (auto showNode = std::dynamic_pointer_cast<ShowNode>(node)) {
        generateShow(showNode);
    } else if (auto duringNode = std::dynamic_pointer_cast<DuringNode>(node)) {
        generateDuring(duringNode);
    } else if (auto forEachNode = std::dynamic_pointer_cast<ForEachNode>(node)) {
        generateForEach(forEachNode);
    } else if (auto repeatNode = std::dynamic_pointer_cast<RepeatNode>(node)) {
        generateRepeat(repeatNode);
    } else if (auto varDecl = std::dynamic_pointer_cast<VarDeclNode>(node)) {
        generateVarDecl(varDecl);
    }
}

void CodeGenerator::declarePrintfFunctions() {
    output << "\n; Declaração de printf\n";
    output << "declare i32 @printf(i8*, ...)\n\n";
    
    output << "; Strings de formato\n";
    output << "@.str_fmt_d = private unnamed_addr constant [4 x i8] c\"%d\\0A\\00\"\n";
    output << "@.str_fmt_f = private unnamed_addr constant [4 x i8] c\"%f\\0A\\00\"\n";
    output << "@.str_fmt_s = private unnamed_addr constant [4 x i8] c\"%s\\0A\\00\"\n\n";
}

double CodeGenerator::parseNumber(const std::string& str) {
    try {
        return std::stod(str);
    } catch (...) {
        return 0.0;
    }
}

bool CodeGenerator::isInteger(const std::string& str) {
    if (str.empty()) return false;
    size_t start = 0;
    if (str[0] == '-' || str[0] == '+') start = 1;
    if (start >= str.size()) return false;
    for (size_t i = start; i < str.size(); i++) {
        if (str[i] < '0' || str[i] > '9') {
            return false;
        }
    }
    return true;
}

std::string CodeGenerator::escapeString(const std::string& str) {
    std::string result;
    for (char c : str) {
        if (c == '\\') {
            result += "\\\\";
        } else if (c == '"') {
            result += "\\22";
        } else if (c == '\n') {
            result += "\\0A";
        } else if (c == '\t') {
            result += "\\09";
        } else if (c >= 32 && c < 127) {
            result += c;
        } else {
            // Caracteres não-ASCII - codifica como hex
            char buf[5];
            sprintf_s(buf, sizeof(buf), "\\%02X", (unsigned char)c);
            result += buf;
        }
    }
    return result;
}
