#include "CodeGenerator.h"
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <cctype>

CodeGenerator::CodeGenerator()
{
    pushScope();
    output << "; ModuleID = 'jokey_module'\n";
    output << "target datalayout = \"e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128\"\n";
    output << "target triple = \"x86_64-pc-windows-msvc\"\n\n";
}

std::string CodeGenerator::generate(const std::shared_ptr<ProgramNode> &program)
{
    collectStrings(program);

    generateProgram(program);
    irCode = output.str();
    return irCode;
}

std::string CodeGenerator::getLLVMType(const std::string &typeName)
{
    if (typeName == "integer" || typeName == "int")
        return "i32";

    if (typeName == "floatingPoint" || typeName == "float" || typeName == "double")
        return "double";

    if (typeName == "boolean" || typeName == "bool")
        return "i1";
    if (typeName == "string" || typeName == "str")
        return "i8*";

    if (typeName.find("[]") != std::string::npos)
    {
        if (typeName.find("string") != std::string::npos)
            return "i8*";
        return "i32*";
    }

    if (typeName == "void")
        return "void";

    return "i32";
}

std::string unescapeRawString(const std::string &input)
{
    std::string res;
    for (size_t i = 0; i < input.size(); i++)
    {
        if (input[i] == '\\' && i + 1 < input.size())
        {
            char next = input[i + 1];
            switch (next)
            {
            case 'n':
                res += '\n';
                break;
            case 't':
                res += '\t';
                break;
            case 'r':
                res += '\r';
                break;
            case '\\':
                res += '\\';
                break;
            case '"':
                res += '"';
                break;
            case '\'':
                res += '\'';
                break;
            default:
                res += '\\';
                res += next;
                break;
            }
            i++;
        }
        else
        {
            res += input[i];
        }
    }
    return res;
}

static bool isBooleanResult(const std::shared_ptr<ExprNode> &node)
{
    if (!node)
        return false;

    if (node->annotatedType == "boolean")
        return true;

    if (auto bin = std::dynamic_pointer_cast<BinaryOpNode>(node))
    {
        std::string op = bin->op;
        if (op == "==" || op == "!=" || op == ">" || op == "<" || op == ">=" || op == "<=" ||
            op == "and" || op == "or" || op == "&&" || op == "||")
        {
            return true;
        }
    }

    if (auto un = std::dynamic_pointer_cast<UnaryOpNode>(node))
    {
        if (un->op == "not" || un->op == "!" || un->op == "nao")
            return true;
    }

    if (auto lit = std::dynamic_pointer_cast<LiteralNode>(node))
    {
        std::string s = lit->lit;
        if (s == "true" || s == "True" || s == "false" || s == "False")
            return true;
    }

    return false;
}

void CodeGenerator::pushScope()
{
    symbolTable.emplace_back();
}

void CodeGenerator::popScope()
{
    if (!symbolTable.empty())
    {
        symbolTable.pop_back();
    }
}

void CodeGenerator::declareVariable(const std::string &name, const std::string &llvmName, const std::string &type)
{
    if (!symbolTable.empty())
    {
        symbolTable.back()[name] = std::make_pair(llvmName, type);
    }
}

std::pair<std::string, std::string> CodeGenerator::lookupVariable(const std::string &name)
{
    for (auto it = symbolTable.rbegin(); it != symbolTable.rend(); ++it)
    {
        auto found = it->find(name);
        if (found != it->end())
        {
            return found->second;
        }
    }
    return std::make_pair("", "");
}

std::string CodeGenerator::getTempName()
{
    std::string name = "%" + std::to_string(tempCounter++);
    return name;
}

std::string CodeGenerator::getLabelName()
{
    return "label" + std::to_string(labelCounter++);
}

std::string CodeGenerator::getStringName()
{
    return "@.str" + std::to_string(stringCounter++);
}

void CodeGenerator::collectStrings(const std::shared_ptr<ProgramNode> &node)
{
    for (auto &decl : node->declarations)
    {
        if (auto varDecl = std::dynamic_pointer_cast<VarDeclNode>(decl))
        {
            if (varDecl->init)
            {
                if (auto exprInit = std::dynamic_pointer_cast<ExprNode>(varDecl->init))
                {
                    collectStringsFromExpr(exprInit);
                }
            }
        }
        else if (auto funcDecl = std::dynamic_pointer_cast<FuncDeclNode>(decl))
        {
            for (auto &stmt : funcDecl->body)
            {
                collectStringsFromStatement(stmt);
            }
        }
    }

    for (auto &stmt : node->statements)
    {
        collectStringsFromStatement(stmt);
    }
}

void CodeGenerator::collectStringsFromStatement(const std::shared_ptr<ASTNode> &stmt)
{
    if (auto showNode = std::dynamic_pointer_cast<ShowNode>(stmt))
    {
        for (auto &arg : showNode->args)
        {
            collectStringsFromExpr(arg);
        }
    }
    else if (auto retNode = std::dynamic_pointer_cast<ReturnNode>(stmt))
    {
        if (retNode->expr)
        {
            collectStringsFromExpr(retNode->expr);
        }
    }
    else if (auto varDecl = std::dynamic_pointer_cast<VarDeclNode>(stmt))
    {
        if (varDecl->init)
        {
            if (auto expr = std::dynamic_pointer_cast<ExprNode>(varDecl->init))
            {
                collectStringsFromExpr(expr);
            }
        }
    }
    else if (auto ifNode = std::dynamic_pointer_cast<IfNode>(stmt))
    {
        collectStringsFromExpr(ifNode->cond);
        for (auto &elsif : ifNode->elsi)
        {
            collectStringsFromExpr(elsif.first);
            for (auto &st : elsif.second)
                collectStringsFromStatement(st);
        }
        for (auto &st : ifNode->thenBranch)
            collectStringsFromStatement(st);
        for (auto &st : ifNode->elseBranch)
            collectStringsFromStatement(st);
    }
    else if (auto duringNode = std::dynamic_pointer_cast<DuringNode>(stmt))
    {
        collectStringsFromExpr(duringNode->cond);
        for (auto &st : duringNode->body)
            collectStringsFromStatement(st);
    }
    else if (auto repeatNode = std::dynamic_pointer_cast<RepeatNode>(stmt))
    {
        collectStringsFromExpr(repeatNode->untilExpr);
        for (auto &st : repeatNode->body)
            collectStringsFromStatement(st);
    }
    else if (auto forEachNode = std::dynamic_pointer_cast<ForEachNode>(stmt))
    {
        collectStringsFromExpr(forEachNode->toExpr);
        for (auto &st : forEachNode->body)
            collectStringsFromStatement(st);
    }
    else if (auto assignNode = std::dynamic_pointer_cast<AssignNode>(stmt))
    {
        collectStringsFromExpr(assignNode->expr);
    }
    else if (auto expr = std::dynamic_pointer_cast<ExprNode>(stmt))
    {
        collectStringsFromExpr(expr);
    }
}

void CodeGenerator::collectStringsFromExpr(const std::shared_ptr<ExprNode> &expr)
{
    if (auto lit = std::dynamic_pointer_cast<LiteralNode>(expr))
    {
        std::string litStr = lit->lit;
        if (litStr.size() >= 2 && (litStr[0] == '"' || litStr[0] == '\'') && litStr.back() == litStr[0])
        {
            std::string rawContent = litStr.substr(1, litStr.size() - 2);

            std::string str = unescapeRawString(rawContent);
            if (stringMap.find(str) == stringMap.end())
            {
                std::string strName = getStringName();
                std::ostringstream strDef;
                strDef << "[" << (str.size() + 1) << " x i8] c\"" << escapeString(str) << "\\00\"";
                globalStrings.push_back(std::make_pair(strName, strDef.str()));
                stringMap[str] = strName;
            }
        }
    }
    else if (auto bin = std::dynamic_pointer_cast<BinaryOpNode>(expr))
    {
        collectStringsFromExpr(bin->left);
        collectStringsFromExpr(bin->right);
    }
    else if (auto un = std::dynamic_pointer_cast<UnaryOpNode>(expr))
    {
        collectStringsFromExpr(un->operand);
    }
    else if (auto arr = std::dynamic_pointer_cast<ArrayLiteralNode>(expr))
    {
        for (auto &el : arr->elements)
        {
            collectStringsFromExpr(el);
        }
    }
    else if (auto call = std::dynamic_pointer_cast<CallNode>(expr))
    {
        for (auto &arg : call->args)
        {
            collectStringsFromExpr(arg);
        }
    }
}

void CodeGenerator::generateProgram(const std::shared_ptr<ProgramNode> &node)
{
    declarePrintfFunctions();

    for (const auto &strPair : globalStrings)
    {
        output << strPair.first << " = private unnamed_addr constant " << strPair.second << "\n";
    }
    if (!globalStrings.empty())
        output << "\n";

    for (auto &decl : node->declarations)
    {
        if (auto varDecl = std::dynamic_pointer_cast<VarDeclNode>(decl))
        {
            generateVarDecl(varDecl);
        }
        else if (auto funcDecl = std::dynamic_pointer_cast<FuncDeclNode>(decl))
        {
            generateFuncDecl(funcDecl);
        }
    }

    if (!node->statements.empty() || !node->declarations.empty())
    {
        output << "\ndefine i32 @main() {\n";
        output << "entry:\n";

        currentFunction = "main";
        currentReturnType = "i32";
        pushScope();

        for (auto &decl : node->declarations)
        {
            if (auto varDecl = std::dynamic_pointer_cast<VarDeclNode>(decl))
            {
                if (varDecl->init)
                {
                    bool isStringLiteral = false;
                    if (auto lit = std::dynamic_pointer_cast<LiteralNode>(varDecl->init))
                    {
                        if (lit->lit.find('"') != std::string::npos)
                            isStringLiteral = true;
                    }

                    if (!std::dynamic_pointer_cast<LiteralNode>(varDecl->init) || isStringLiteral)
                    {
                        std::string val = generateExpr(std::dynamic_pointer_cast<ExprNode>(varDecl->init));
                        auto varInfo = lookupVariable(varDecl->ident);
                        std::string llvmName = varInfo.first;
                        std::string type = getLLVMType(varDecl->typeName);

                        std::string exprType = std::dynamic_pointer_cast<ExprNode>(varDecl->init)->annotatedType;
                        if (exprType.empty())
                            exprType = (val.find("@") != std::string::npos) ? "string" : "integer";

                        std::string valType = getLLVMType(exprType);

                        if (type == "double" && valType == "i32")
                        {
                            std::string temp = getTempName();
                            output << "  " << temp << " = sitofp i32 " << val << " to double\n";
                            val = temp;
                        }

                        output << "  store " << type << " " << val << ", " << type << "* " << llvmName << "\n";
                    }
                }
            }
        }

        for (auto &stmt : node->statements)
        {
            generateStatement(stmt);
        }

        output << "  ret i32 0\n";
        output << "}\n";

        popScope();
        currentFunction = "";
    }
}

void CodeGenerator::generateVarDecl(const std::shared_ptr<VarDeclNode> &node)
{
    std::string type = getLLVMType(node->typeName);
    std::string varName = node->ident;
    std::string llvmVarName;

    if (currentFunction.empty())
    {
        llvmVarName = "@" + varName;

        std::string initValue;
        if (type == "i32")
            initValue = "0";
        else if (type == "double")
            initValue = "0.000000e+00";
        else if (type == "i1")
            initValue = "false";
        else
            initValue = "null";

        if (node->init)
        {
            if (auto arrLit = std::dynamic_pointer_cast<ArrayLiteralNode>(node->init))
            {
                arraySizes[varName] = arrLit->elements.size();
            }
            if (auto lit = std::dynamic_pointer_cast<LiteralNode>(node->init))
            {
                if (lit->lit.find('"') != std::string::npos)
                {
                    initValue = "null";
                }
                else
                {
                    std::string val = generateExpr(std::dynamic_pointer_cast<ExprNode>(node->init));
                    if (val.find("%") == std::string::npos)
                    {
                        initValue = val;
                    }
                }
            }
        }

        output << llvmVarName << " = global " << type << " " << initValue << "\n";
        declareVariable(varName, llvmVarName, node->typeName);
    }
    else
    {
        llvmVarName = getTempName();
        output << "  " << llvmVarName << " = alloca " << type << "\n";

        if (node->init)
        {
            if (auto arrLit = std::dynamic_pointer_cast<ArrayLiteralNode>(node->init))
            {
                arraySizes[varName] = arrLit->elements.size();
            }

            std::string initValue = generateExpr(std::dynamic_pointer_cast<ExprNode>(node->init));

            if (!initValue.empty())
            {
                output << "  store " << type << " " << initValue << ", " << type << "* " << llvmVarName << "\n";
            }
        }
        else
        {
            if (type == "i32")
                output << "  store i32 0, i32* " << llvmVarName << "\n";
            else if (type == "double")
                output << "  store double 0.0, double* " << llvmVarName << "\n";
            else if (type == "i1")
                output << "  store i1 false, i1* " << llvmVarName << "\n";
        }

        declareVariable(varName, llvmVarName, node->typeName);
    }
}

void CodeGenerator::generateReturn(const std::shared_ptr<ReturnNode> &node)
{
    if (node->expr)
    {
        std::string val = generateExpr(node->expr);
        std::string llvmType = getLLVMType(currentReturnType);

        std::string exprType = node->expr->annotatedType;
        if (exprType.empty())
            exprType = "integer";

        std::string valType = getLLVMType(exprType);

        if (llvmType == "double" && valType == "i32")
        {
            std::string temp = getTempName();
            output << "  " << temp << " = sitofp i32 " << val << " to double\n";
            val = temp;
        }
        else if (llvmType == "i32" && valType == "double")
        {
            std::string temp = getTempName();
            output << "  " << temp << " = fptosi double " << val << " to i32\n";
            val = temp;
        }

        output << "  ret " << llvmType << " " << val << "\n";
    }
    else
    {
        output << "  ret void\n";
    }
}

void CodeGenerator::generateFuncDecl(const std::shared_ptr<FuncDeclNode> &node)
{
    int savedTempCounter = tempCounter;

    tempCounter = 0;

    std::string retType = getLLVMType(node->retType);
    std::string funcName = "@" + node->name;

    output << "\ndefine " << retType << " " << funcName << "(";

    std::vector<std::string> paramNames;

    for (size_t i = 0; i < node->params.size(); i++)
    {
        if (i > 0)
            output << ", ";
        std::string paramType = getLLVMType(node->params[i].first);

        std::string paramName = getTempName();

        output << paramType << " " << paramName;
        paramNames.push_back(paramName);
    }

    output << ") {\n";
    output << "entry:\n";

    std::string prevFunction = currentFunction;
    std::string prevReturnType = currentReturnType;

    currentFunction = node->name;

    currentReturnType = node->retType;

    pushScope();

    for (size_t i = 0; i < node->params.size(); i++)
    {
        std::string paramType = getLLVMType(node->params[i].first);
        std::string allocaName = getTempName();
        std::string paramVar = paramNames[i];

        output << "  " << allocaName << " = alloca " << paramType << "\n";
        output << "  store " << paramType << " " << paramVar << ", " << paramType << "* " << allocaName << "\n";

        declareVariable(node->params[i].second, allocaName, node->params[i].first);
    }

    for (auto &stmt : node->body)
    {
        generateStatement(stmt);
    }

    std::string finalLLVMType = getLLVMType(currentReturnType);
    if (finalLLVMType == "void")
    {
        output << "  ret void\n";
    }
    else if (finalLLVMType == "i32")
    {
        output << "  ret i32 0\n";
    }
    else if (finalLLVMType == "double")
    {
        output << "  ret double 0.0\n";
    }
    else if (finalLLVMType == "i1")
    {
        output << "  ret i1 0\n";
    }
    else
    {
        output << "  ret " << finalLLVMType << " null\n";
    }

    output << "}\n";

    popScope();

    currentFunction = prevFunction;
    currentReturnType = prevReturnType;
    tempCounter = savedTempCounter;
}

std::string CodeGenerator::generateExpr(const std::shared_ptr<ExprNode> &expr)
{
    if (auto id = std::dynamic_pointer_cast<IdentifierNode>(expr))
    {
        return generateIdentifier(id);
    }
    else if (auto lit = std::dynamic_pointer_cast<LiteralNode>(expr))
    {
        return generateLiteral(lit);
    }
    else if (auto arr = std::dynamic_pointer_cast<ArrayLiteralNode>(expr))
    {
        return generateArrayLiteral(arr);
    }
    else if (auto bin = std::dynamic_pointer_cast<BinaryOpNode>(expr))
    {
        return generateBinaryOp(bin);
    }
    else if (auto un = std::dynamic_pointer_cast<UnaryOpNode>(expr))
    {
        return generateUnaryOp(un);
    }
    else if (auto call = std::dynamic_pointer_cast<CallNode>(expr))
    {
        return generateCall(call);
    }

    return "0";
}

std::string CodeGenerator::generateIdentifier(const std::shared_ptr<IdentifierNode> &node)
{
    auto varInfo = lookupVariable(node->name);
    std::string varName = varInfo.first;
    std::string varType = varInfo.second;

    if (varName.empty())
    {
        return "0";
    }

    node->annotatedType = varType;

    if (!node->annotatedType.empty())
    {
        varType = node->annotatedType;
    }

    std::string llvmType = getLLVMType(varType);

    if (varName[0] == '%' || varName[0] == '@')
    {
        std::string temp = getTempName();
        output << "  " << temp << " = load " << llvmType << ", " << llvmType << "* " << varName << "\n";
        return temp;
    }

    return varName;
}

std::string CodeGenerator::generateLiteral(const std::shared_ptr<LiteralNode> &node)
{
    std::string lit = node->lit;

    if (lit.size() >= 2 && (lit[0] == '"' || lit[0] == '\'') && lit.back() == lit[0])
    {
        node->annotatedType = "string";
        std::string rawContent = lit.substr(1, lit.size() - 2);

        std::string str = unescapeRawString(rawContent);

        std::string strName;
        if (stringMap.find(str) != stringMap.end())
        {
            strName = stringMap[str];
        }
        else
        {
            return "null";
        }
        std::string temp = getTempName();
        output << "  " << temp << " = getelementptr inbounds [" << (str.size() + 1)
               << " x i8], [" << (str.size() + 1) << " x i8]* " << strName << ", i64 0, i64 0\n";
        return temp;
    }

    std::string litLower = lit;
    std::transform(litLower.begin(), litLower.end(), litLower.begin(), ::tolower);

    if (litLower == "true" || litLower == "verdadeiro")
    {
        node->annotatedType = "boolean";
        return "1";
    }
    if (litLower == "false" || litLower == "falso")
    {
        node->annotatedType = "boolean";
        return "0";
    }

    if (isInteger(lit))
    {
        node->annotatedType = "integer";
        try
        {
            int val = static_cast<int>(parseNumber(lit));
            return std::to_string(val);
        }
        catch (...)
        {
            return "0";
        }
    }
    else
    {
        node->annotatedType = "float";
        try
        {
            bool isValidFloat = true;
            if (isValidFloat)
            {
                double val = parseNumber(lit);
                std::ostringstream oss;
                oss << std::scientific << std::setprecision(6) << val;
                return oss.str();
            }
        }
        catch (...)
        {
        }
    }
    return "0";
}

std::string CodeGenerator::generateArrayLiteral(const std::shared_ptr<ArrayLiteralNode> &node)
{
    if (node->elements.empty())
        return "null";

    std::string firstVal = generateExpr(node->elements[0]);
    std::string elemType = "i32";
    if (firstVal.find("@") != std::string::npos || node->elements[0]->annotatedType == "string")
    {
        elemType = "i8*";
    }

    int size = node->elements.size();

    std::string arrBase = getTempName();
    output << "  " << arrBase << " = alloca " << elemType << ", i32 " << size << "\n";

    for (int i = 0; i < size; i++)
    {
        std::string val = generateExpr(node->elements[i]);

        std::string ptr = getTempName();
        output << "  " << ptr << " = getelementptr " << elemType << ", " << elemType << "* " << arrBase << ", i32 " << i << "\n";

        output << "  store " << elemType << " " << val << ", " << elemType << "* " << ptr << "\n";
    }

    return arrBase;
}

std::string CodeGenerator::generateBinaryOp(const std::shared_ptr<BinaryOpNode> &node)
{
    std::string left = generateExpr(node->left);
    std::string right = generateExpr(node->right);
    std::string op = node->op;

    if (left.empty() || right.empty())
        return "0";

    std::string leftType = node->left->annotatedType;
    std::string rightType = node->right->annotatedType;

    if (leftType.empty())
        leftType = (left.find(".") != std::string::npos) ? "float" : "integer";
    if (rightType.empty())
        rightType = (right.find(".") != std::string::npos) ? "float" : "integer";

    std::string leftLLVM = getLLVMType(leftType);
    std::string rightLLVM = getLLVMType(rightType);

    bool leftIsInt = (leftLLVM == "i32");
    bool rightIsInt = (rightLLVM == "i32");
    bool leftIsBool = (leftLLVM == "i1");
    bool rightIsBool = (rightLLVM == "i1");

    bool isMixed = (leftIsInt && !rightIsInt) || (!leftIsInt && rightIsInt);
    bool isFloatOp = (leftLLVM == "double" || rightLLVM == "double");

    if (isMixed && isFloatOp)
    {
        if (leftIsInt)
        {
            std::string temp = getTempName();
            output << "  " << temp << " = sitofp i32 " << left << " to double\n";
            left = temp;
        }
        if (rightIsInt)
        {
            std::string temp = getTempName();
            output << "  " << temp << " = sitofp i32 " << right << " to double\n";
            right = temp;
        }
    }

    std::string result = getTempName();

    if (op == "+")
    {
        if (isFloatOp)
        {
            output << "  " << result << " = fadd double " << left << ", " << right << "\n";
            node->annotatedType = "float";
        }
        else
        {
            output << "  " << result << " = add i32 " << left << ", " << right << "\n";
            node->annotatedType = "integer";
        }
    }
    else if (op == "-")
    {
        if (isFloatOp)
        {
            output << "  " << result << " = fsub double " << left << ", " << right << "\n";
            node->annotatedType = "float";
        }
        else
        {
            output << "  " << result << " = sub i32 " << left << ", " << right << "\n";
            node->annotatedType = "integer";
        }
    }
    else if (op == "*")
    {
        if (isFloatOp)
        {
            output << "  " << result << " = fmul double " << left << ", " << right << "\n";
            node->annotatedType = "float";
        }
        else
        {
            output << "  " << result << " = mul i32 " << left << ", " << right << "\n";
            node->annotatedType = "integer";
        }
    }
    else if (op == "/")
    {
        if (isFloatOp)
        {
            output << "  " << result << " = fdiv double " << left << ", " << right << "\n";
            node->annotatedType = "float";
        }
        else
        {
            output << "  " << result << " = sdiv i32 " << left << ", " << right << "\n";
            node->annotatedType = "integer";
        }
    }
    else if (op == "%")
    {
        output << "  " << result << " = srem i32 " << left << ", " << right << "\n";
        node->annotatedType = "integer";
    }
    else if (op == "==" || op == "=")
    {
        if (leftIsBool && rightIsBool)
            output << "  " << result << " = icmp eq i1 " << left << ", " << right << "\n";
        else if (isFloatOp)
            output << "  " << result << " = fcmp oeq double " << left << ", " << right << "\n";
        else
            output << "  " << result << " = icmp eq i32 " << left << ", " << right << "\n";
        node->annotatedType = "boolean";
    }
    else if (op == "!=" || op == "<>")
    {
        if (leftIsBool && rightIsBool)
            output << "  " << result << " = icmp ne i1 " << left << ", " << right << "\n";
        else if (isFloatOp)
            output << "  " << result << " = fcmp one double " << left << ", " << right << "\n";
        else
            output << "  " << result << " = icmp ne i32 " << left << ", " << right << "\n";
        node->annotatedType = "boolean";
    }
    else if (op == ">")
    {
        if (isFloatOp)
            output << "  " << result << " = fcmp ogt double " << left << ", " << right << "\n";
        else
            output << "  " << result << " = icmp sgt i32 " << left << ", " << right << "\n";
        node->annotatedType = "boolean";
    }
    else if (op == "<")
    {
        if (isFloatOp)
            output << "  " << result << " = fcmp olt double " << left << ", " << right << "\n";
        else
            output << "  " << result << " = icmp slt i32 " << left << ", " << right << "\n";
        node->annotatedType = "boolean";
    }
    else if (op == ">=")
    {
        if (isFloatOp)
            output << "  " << result << " = fcmp oge double " << left << ", " << right << "\n";
        else
            output << "  " << result << " = icmp sge i32 " << left << ", " << right << "\n";
        node->annotatedType = "boolean";
    }
    else if (op == "<=")
    {
        if (isFloatOp)
            output << "  " << result << " = fcmp ole double " << left << ", " << right << "\n";
        else
            output << "  " << result << " = icmp sle i32 " << left << ", " << right << "\n";
        node->annotatedType = "boolean";
    }
    else if (op == "and" || op == "&&")
    {
        output << "  " << result << " = and i1 " << left << ", " << right << "\n";
        node->annotatedType = "boolean";
    }
    else if (op == "or" || op == "||")
    {
        output << "  " << result << " = or i1 " << left << ", " << right << "\n";
        node->annotatedType = "boolean";
    }

    return result;
}

std::string CodeGenerator::generateUnaryOp(const std::shared_ptr<UnaryOpNode> &node)
{
    std::string operand = generateExpr(node->operand);
    std::string op = node->op;

    if (operand.empty())
        return "0";

    std::string result = getTempName();

    if (op == "-")
    {
        std::string type = node->operand->annotatedType;

        if (type.empty())
            type = node->annotatedType;
        if (type.empty() && operand.find(".") != std::string::npos)
            type = "floatingPoint";

        bool isFloat = (type == "float" || type == "double" || type == "floatingPoint");

        if (isFloat)
        {
            output << "  " << result << " = fneg double " << operand << "\n";
            node->annotatedType = "floatingPoint";
        }
        else
        {
            output << "  " << result << " = sub i32 0, " << operand << "\n";
            node->annotatedType = "integer";
        }
    }
    else if (op == "!" || op == "nao" || op == "not")
    {
        output << "  " << result << " = xor i1 " << operand << ", true\n";
        node->annotatedType = "boolean";
    }
    else if (op == "+")
    {
        return operand;
    }

    return result;
}

void CodeGenerator::generateIf(const std::shared_ptr<IfNode> &node)
{
    std::string mergeLabel = getLabelName();

    std::string cond = generateExpr(node->cond);
    std::string thenLabel = getLabelName();
    std::string nextCheckLabel = getLabelName();

    std::string condBool = cond;
    if (!isBooleanResult(node->cond))
    {
        condBool = getTempName();
        output << "  " << condBool << " = icmp ne i32 " << cond << ", 0\n";
    }

    output << "  br i1 " << condBool << ", label %" << thenLabel << ", label %" << nextCheckLabel << "\n";

    output << "\n"
           << thenLabel << ":\n";
    pushScope();
    bool thenTerminated = false;
    for (auto &stmt : node->thenBranch)
    {
        generateStatement(stmt);
        if (std::dynamic_pointer_cast<ReturnNode>(stmt))
            thenTerminated = true;
    }
    popScope();
    if (!thenTerminated)
        output << "  br label %" << mergeLabel << "\n";

    std::string currentTestLabel = nextCheckLabel;

    for (auto &elsif : node->elsi)
    {
        output << "\n"
               << currentTestLabel << ":\n";

        std::string elifCond = generateExpr(elsif.first);
        std::string elifBodyLabel = getLabelName();
        std::string nextElifLabel = getLabelName();

        std::string elifCondBool = elifCond;
        if (!isBooleanResult(elsif.first))
        {
            elifCondBool = getTempName();
            output << "  " << elifCondBool << " = icmp ne i32 " << elifCond << ", 0\n";
        }

        output << "  br i1 " << elifCondBool << ", label %" << elifBodyLabel << ", label %" << nextElifLabel << "\n";

        output << "\n"
               << elifBodyLabel << ":\n";
        pushScope();
        bool elifTerminated = false;
        for (auto &stmt : elsif.second)
        {
            generateStatement(stmt);
            if (std::dynamic_pointer_cast<ReturnNode>(stmt))
                elifTerminated = true;
        }
        popScope();
        if (!elifTerminated)
            output << "  br label %" << mergeLabel << "\n";

        currentTestLabel = nextElifLabel;
    }

    output << "\n"
           << currentTestLabel << ":\n";

    if (!node->elseBranch.empty())
    {
        pushScope();
        bool elseTerminated = false;
        for (auto &stmt : node->elseBranch)
        {
            generateStatement(stmt);
            if (std::dynamic_pointer_cast<ReturnNode>(stmt))
                elseTerminated = true;
        }
        popScope();
        if (!elseTerminated)
            output << "  br label %" << mergeLabel << "\n";
    }
    else
    {
        output << "  br label %" << mergeLabel << "\n";
    }

    output << "\n"
           << mergeLabel << ":\n";
}

void CodeGenerator::generateAssign(const std::shared_ptr<AssignNode> &node)
{
    auto varInfo = lookupVariable(node->target);
    std::string varName = varInfo.first;
    std::string varType = varInfo.second;

    if (varName.empty())
    {
        return;
    }

    std::string value = generateExpr(node->expr);
    if (value.empty())
    {
        return;
    }

    std::string llvmType = getLLVMType(varType);

    std::string exprType = node->expr->annotatedType;
    if (!exprType.empty() && exprType != varType)
    {
        std::string llvmExprType = getLLVMType(exprType);
        if (llvmType == "double" && llvmExprType == "i32")
        {
            std::string temp = getTempName();
            output << "  " << temp << " = sitofp i32 " << value << " to double\n";
            value = temp;
        }
        else if (llvmType == "i32" && llvmExprType == "double")
        {
            std::string temp = getTempName();
            output << "  " << temp << " = fptosi double " << value << " to i32\n";
            value = temp;
        }
    }

    output << "  store " << llvmType << " " << value << ", " << llvmType << "* " << varName << "\n";
}

void CodeGenerator::generateShow(const std::shared_ptr<ShowNode> &node)
{
    // 1. Imprime todos os argumentos colados (sem pular linha entre eles)
    for (auto &arg : node->args)
    {
        std::string val = generateExpr(arg);
        if (val.empty() || val == "0" || val == "null")
        {
            continue;
        }

        std::string exprType = arg->annotatedType;
        if (exprType.empty())
        {
            if (val.find("@.str") != std::string::npos || val[0] == '%')
                exprType = "string";
            else
                exprType = "integer";
        }

        // Ignora arrays para não quebrar o terminal
        if (exprType.find("[]") != std::string::npos)
        {
            std::string temp = getTempName(); // Consome contador para manter sincronia
            continue;
        }

        std::string llvmType = getLLVMType(exprType);
        std::string callResult = getTempName();

        if (llvmType == "i8*")
        {
            // String
            if (val == "0" || val == "null")
                continue;
            output << "  " << callResult << " = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str_fmt_s, i32 0, i32 0), i8* " << val << ")\n";
        }
        else if (llvmType == "double")
        {
            // Float
            output << "  " << callResult << " = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str_fmt_f, i32 0, i32 0), double " << val << ")\n";
        }
        else if (llvmType == "i1")
        {
            // Boolean
            std::string intVal = getTempName();
            output << "  " << intVal << " = zext i1 " << val << " to i32\n";
            callResult = getTempName(); // Atualiza para o call
            output << "  " << callResult << " = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str_fmt_d, i32 0, i32 0), i32 " << intVal << ")\n";
        }
        else
        {
            // Integer
            output << "  " << callResult << " = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str_fmt_d, i32 0, i32 0), i32 " << val << ")\n";
        }
    } // <--- FIM DO LOOP AQUI

    // 2. Imprime a quebra de linha UMA VEZ no final
    std::string nlCall = getTempName();
    output << "  " << nlCall << " = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str_nl, i32 0, i32 0))\n";
}

void CodeGenerator::generateDuring(const std::shared_ptr<DuringNode> &node)
{
    std::string condLabel = getLabelName();
    std::string bodyLabel = getLabelName();
    std::string afterLabel = getLabelName();

    output << "  br label %" << condLabel << "\n";

    output << "\n"
           << condLabel << ":\n";
    std::string cond = generateExpr(node->cond);

    std::string condBool = cond;

    if (!isBooleanResult(node->cond))
    {
        condBool = getTempName();
        output << "  " << condBool << " = icmp ne i32 " << cond << ", 0\n";
    }

    output << "  br i1 " << condBool << ", label %" << bodyLabel << ", label %" << afterLabel << "\n";

    output << "\n"
           << bodyLabel << ":\n";
    pushScope();
    for (auto &stmt : node->body)
    {
        generateStatement(stmt);
    }
    popScope();
    output << "  br label %" << condLabel << "\n";

    output << "\n"
           << afterLabel << ":\n";
}

void CodeGenerator::generateForEach(const std::shared_ptr<ForEachNode> &node)
{
    pushScope();

    std::string arrPtr = generateExpr(node->toExpr);

    int size = 10;
    std::string varNameForLookup;

    if (auto idNode = std::dynamic_pointer_cast<IdentifierNode>(node->toExpr))
    {
        varNameForLookup = idNode->name;
        if (arraySizes.find(idNode->name) != arraySizes.end())
        {
            size = arraySizes[idNode->name];
        }
    }

    std::string elemType = "i32"; // Padrão

    if (!varNameForLookup.empty())
    {
        auto varInfo = lookupVariable(varNameForLookup);
        std::string jokeyType = varInfo.second;

        if (jokeyType.find("string") != std::string::npos)
        {
            elemType = "i8*";
        }
        else if (jokeyType.find("float") != std::string::npos)
        {
            elemType = "double";
        }
        else if (jokeyType.find("bool") != std::string::npos)
        {
            elemType = "i1";
        }
    }
    else
    {
        if (arrPtr.find("@") != std::string::npos)
            elemType = "i8*";
    }

    std::string indexVarPtr = getTempName();
    output << "  " << indexVarPtr << " = alloca i32\n";
    output << "  store i32 0, i32* " << indexVarPtr << "\n";

    std::string itemVarPtr = getTempName();
    output << "  " << itemVarPtr << " = alloca " << elemType << "\n";

    declareVariable(node->ident, itemVarPtr, (elemType == "i8*" ? "string" : "integer"));

    std::string labelCond = getLabelName();
    std::string labelBody = getLabelName();
    std::string labelEnd = getLabelName();

    output << "  br label %" << labelCond << "\n";

    output << "\n"
           << labelCond << ":\n";
    std::string currIdx = getTempName();
    output << "  " << currIdx << " = load i32, i32* " << indexVarPtr << "\n";

    std::string cond = getTempName();
    output << "  " << cond << " = icmp slt i32 " << currIdx << ", " << size << "\n";
    output << "  br i1 " << cond << ", label %" << labelBody << ", label %" << labelEnd << "\n";

    output << "\n"
           << labelBody << ":\n";

    std::string elemPtr = getTempName();
    output << "  " << elemPtr << " = getelementptr " << elemType << ", " << elemType << "* " << arrPtr << ", i32 " << currIdx << "\n";

    std::string val = getTempName();
    output << "  " << val << " = load " << elemType << ", " << elemType << "* " << elemPtr << "\n";

    output << "  store " << elemType << " " << val << ", " << elemType << "* " << itemVarPtr << "\n";

    for (auto &stmt : node->body)
    {
        generateStatement(stmt);
    }

    std::string reloadedIdx = getTempName();
    output << "  " << reloadedIdx << " = load i32, i32* " << indexVarPtr << "\n";

    std::string nextIdx = getTempName();
    output << "  " << nextIdx << " = add i32 " << reloadedIdx << ", 1\n";

    output << "  store i32 " << nextIdx << ", i32* " << indexVarPtr << "\n";

    output << "  br label %" << labelCond << "\n";

    output << "\n"
           << labelEnd << ":\n";
    popScope();
}

void CodeGenerator::generateRepeat(const std::shared_ptr<RepeatNode> &node)
{
    std::string bodyLabel = getLabelName();
    std::string condLabel = getLabelName();
    std::string afterLabel = getLabelName();

    output << "  br label %" << bodyLabel << "\n";

    output << "\n"
           << bodyLabel << ":\n";
    pushScope();
    for (auto &stmt : node->body)
    {
        generateStatement(stmt);
    }
    popScope();
    output << "  br label %" << condLabel << "\n";

    output << "\n"
           << condLabel << ":\n";
    std::string cond = generateExpr(node->untilExpr);

    std::string condBool = cond;

    if (!isBooleanResult(node->untilExpr))
    {
        condBool = getTempName();
        output << "  " << condBool << " = icmp ne i32 " << cond << ", 0\n";
    }

    output << "  br i1 " << condBool << ", label %" << afterLabel << ", label %" << bodyLabel << "\n";

    output << "\n"
           << afterLabel << ":\n";
}

void CodeGenerator::generateStatement(const std::shared_ptr<ASTNode> &node)
{
    if (auto ifNode = std::dynamic_pointer_cast<IfNode>(node))
    {
        generateIf(ifNode);
    }
    else if (auto retNode = std::dynamic_pointer_cast<ReturnNode>(node))
    {
        output << "  ; Generating Return\n";
        generateReturn(retNode);
    }
    else if (auto assignNode = std::dynamic_pointer_cast<AssignNode>(node))
    {
        generateAssign(assignNode);
    }
    else if (auto showNode = std::dynamic_pointer_cast<ShowNode>(node))
    {
        generateShow(showNode);
    }
    else if (auto duringNode = std::dynamic_pointer_cast<DuringNode>(node))
    {
        generateDuring(duringNode);
    }
    else if (auto forEachNode = std::dynamic_pointer_cast<ForEachNode>(node))
    {
        generateForEach(forEachNode);
    }
    else if (auto repeatNode = std::dynamic_pointer_cast<RepeatNode>(node))
    {
        generateRepeat(repeatNode);
    }
    else if (auto varDecl = std::dynamic_pointer_cast<VarDeclNode>(node))
    {
        output << "  ; Generating VarDecl: " << varDecl->ident << "\n";
        generateVarDecl(varDecl);
    }
    else if (auto expr = std::dynamic_pointer_cast<ExprNode>(node))
    {
        generateExpr(expr);
    }
}

void CodeGenerator::declarePrintfFunctions()
{
    output << "\n; Declaração de printf\n";
    output << "declare i32 @printf(i8*, ...)\n\n";

    output << "; Strings de formato\n";
    output << "@.str_fmt_d = private unnamed_addr constant [3 x i8] c\"%d\\00\"\n";
    output << "@.str_fmt_f = private unnamed_addr constant [3 x i8] c\"%f\\00\"\n";
    output << "@.str_fmt_s = private unnamed_addr constant [3 x i8] c\"%s\\00\"\n\n";

    output << "@.str_nl = private unnamed_addr constant [2 x i8] c\"\\0A\\00\"\n\n";
}

double CodeGenerator::parseNumber(const std::string &str)
{
    try
    {
        return std::stod(str);
    }
    catch (...)
    {
        return 0.0;
    }
}

bool CodeGenerator::isInteger(const std::string &str)
{
    if (str.empty())
        return false;
    size_t start = 0;
    if (str[0] == '-' || str[0] == '+')
        start = 1;
    if (start >= str.size())
        return false;
    for (size_t i = start; i < str.size(); i++)
    {
        if (str[i] < '0' || str[i] > '9')
        {
            return false;
        }
    }
    return true;
}
std::string CodeGenerator::escapeString(const std::string &str)
{
    std::string result;
    for (char c : str)
    {
        if (c == '\\')
        {
            result += "\\\\";
        }
        else if (c == '"')
        {
            result += "\\22";
        }
        else if (c == '\n')
        {
            result += "\\0A";
        }
        else if (c == '\t')
        {
            result += "\\09";
        }
        else if (c >= 32 && c < 127)
        {
            result += c;
        }
        else
        {
            char buf[5];
            std::snprintf(buf, sizeof(buf), "\\%02X", (unsigned char)c);
            result += buf;
        }
    }
    return result;
}

std::string CodeGenerator::generateCall(const std::shared_ptr<CallNode> &node)
{
    std::vector<std::string> argVals;
    for (auto &arg : node->args)
    {
        argVals.push_back(generateExpr(arg));
    }

    std::string returnType = "i32";
    if (!node->annotatedType.empty())
    {
        returnType = getLLVMType(node->annotatedType);
    }

    std::string callResult = "";
    if (returnType != "void")
    {
        callResult = getTempName();
        output << "  " << callResult << " = ";
    }
    else
    {
        output << "  ";
    }

    output << "call " << returnType << " @" << node->funcName << "(";

    for (size_t i = 0; i < argVals.size(); i++)
    {
        if (i > 0)
            output << ", ";

        std::string argType = "i32";

        if (!node->args[i]->annotatedType.empty())
        {
            argType = getLLVMType(node->args[i]->annotatedType);
        }
        else
        {
            if (argVals[i].find("@") != std::string::npos)
                argType = "i8*";
            else if (argVals[i].find(".") != std::string::npos)
                argType = "double";
        }

        output << argType << " " << argVals[i];
    }

    output << ")\n";

    if (returnType == "double")
        node->annotatedType = "float";
    else if (returnType == "i8*")
        node->annotatedType = "string";
    else if (returnType == "i1")
        node->annotatedType = "boolean";
    else if (returnType == "void")
        node->annotatedType = "void";
    else
        node->annotatedType = "integer";

    return callResult;
}