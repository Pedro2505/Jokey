#include "SemanticAnalyzer.h"
#include <sstream>
#include <iostream>
#include <algorithm>

static bool endsWith(const std::string &s, const std::string &suf)
{
    if (s.size() < suf.size())
        return false;
    return s.compare(s.size() - suf.size(), suf.size(), suf) == 0;
}

SemanticAnalyzer::SemanticAnalyzer()
{
    pushScope();
}

void SemanticAnalyzer::pushScope()
{
    scopes.emplace_back();
}
void SemanticAnalyzer::popScope()
{
    if (!scopes.empty())
        scopes.pop_back();
}
bool SemanticAnalyzer::declare(const std::string &name, const std::string &type, bool isStatic, bool isMutable)
{
    if (scopes.empty())
        pushScope();
    auto &cur = scopes.back();
    if (cur.find(name) != cur.end())
    {
        std::ostringstream os;
        os << "Variável '" << name << "' já declarada neste escopo";
        errs.push_back(os.str());
        return false;
    }
    cur[name] = Symbol{type, isStatic, isMutable};
    return true;
}
Symbol *SemanticAnalyzer::lookup(const std::string &name)
{
    for (int i = (int)scopes.size() - 1; i >= 0; --i)
    {
        auto it = scopes[i].find(name);
        if (it != scopes[i].end())
            return &it->second;
    }
    return nullptr;
}

void SemanticAnalyzer::analyze(const std::shared_ptr<ProgramNode> &prog)
{
    for (auto &d : prog->declarations)
        analyzeNode(d);
    for (auto &s : prog->statements)
        analyzeNode(s);
    for (auto &e : errs)
        std::cerr << "Erro semantico: " << e << std::endl;
}

void SemanticAnalyzer::analyzeNode(const std::shared_ptr<ASTNode> &node)
{
    if (!node)
        return;
    if (auto v = std::dynamic_pointer_cast<VarDeclNode>(node))
        analyzeVarDecl(v);
    else if (auto f = std::dynamic_pointer_cast<FuncDeclNode>(node))
        analyzeFuncDecl(f);
    else if (auto c = std::dynamic_pointer_cast<ClassDeclNode>(node))
        analyzeClassDecl(c);
    else if (auto comp = std::dynamic_pointer_cast<ContractDeclNode>(node))
        analyzeContractDecl(comp);
    else if (auto i = std::dynamic_pointer_cast<IfNode>(node))
        analyzeIf(i);
    else if (auto a = std::dynamic_pointer_cast<AssignNode>(node))
        analyzeAssign(a);
    else if (auto s = std::dynamic_pointer_cast<ShowNode>(node))
        analyzeShow(s);
    else if (auto d = std::dynamic_pointer_cast<DuringNode>(node))
        analyzeDuring(d);
    else if (auto fe = std::dynamic_pointer_cast<ForEachNode>(node))
        analyzeForEach(fe);
    else if (auto r = std::dynamic_pointer_cast<RepeatNode>(node))
        analyzeRepeat(r);
    else
    {
        if (auto e = std::dynamic_pointer_cast<ExprNode>(node))
            analyzeExpr(e);
    }
}

void SemanticAnalyzer::analyzeVarDecl(const std::shared_ptr<VarDeclNode> &node)
{
    if (!declare(node->ident, node->typeName, node->isStatic, true))
        return;
    if (node->init)
    {
        std::string t = analyzeExpr(std::dynamic_pointer_cast<ExprNode>(node->init));
        if (!t.empty())
        {
            if (endsWith(node->typeName, "[]"))
            {
                if (t != node->typeName)
                {
                    std::ostringstream os;
                    os << "Não é possível atribuir um valor do tipo '" << t << "' a variável '" << node->ident
                       << "' do tipo '" << node->typeName << "'";
                    errs.push_back(os.str());
                }
            }
            else
            {
                if (!typeEquals(t, node->typeName))
                {
                    std::ostringstream os;
                    os << "Não é possível atribuir um valor do tipo '" << t << "' a variável '" << node->ident << "' do tipo '" << node->typeName << "'";
                    errs.push_back(os.str());
                }
            }
        }
    }
}

void SemanticAnalyzer::analyzeFuncDecl(const std::shared_ptr<FuncDeclNode> &node)
{
    if (!declare(node->name, node->retType, false, false))
        return;
    pushScope();
    for (auto &p : node->params)
    {
        declare(p.second, p.first, false, true);
    }
    for (auto &b : node->body)
        analyzeNode(b);
    if (node->regressExpr)
    {
        std::string rt = analyzeExpr(std::dynamic_pointer_cast<ExprNode>(node->regressExpr));
        if (!typeEquals(rt, node->retType))
        {
            std::ostringstream os;
            os << "Função '" << node->name << "' retorna tipo '" << rt << "' mas foi declarada como '" << node->retType << "'";
            errs.push_back(os.str());
        }
    }
    popScope();
}

void SemanticAnalyzer::analyzeClassDecl(const std::shared_ptr<ClassDeclNode> &node)
{
    declare(node->name, "class", false, false);
    pushScope();
    for (auto &m : node->members)
        analyzeNode(m);
    popScope();
}

void SemanticAnalyzer::analyzeContractDecl(const std::shared_ptr<ContractDeclNode> &node)
{
    declare(node->name, "component", false, false);
    pushScope();
    for (auto &m : node->members)
        analyzeNode(m);
    popScope();
}

void SemanticAnalyzer::analyzeIf(const std::shared_ptr<IfNode> &node)
{
    std::string condt = analyzeExpr(node->cond);
    if (!typeEquals(condt, "boolean"))
    {
        errs.push_back("Condição do 'if' deve ser boolean");
    }
    pushScope();
    for (auto &s : node->thenBranch)
        analyzeNode(s);
    popScope();
    for (auto &e : node->elsi)
    {
        std::string ct = analyzeExpr(e.first);
        if (!typeEquals(ct, "boolean"))
            errs.push_back("Condicao de 'elsif' deve ser boolean");
        pushScope();
        for (auto &s : e.second)
            analyzeNode(s);
        popScope();
    }
    if (!node->elseBranch.empty())
    {
        pushScope();
        for (auto &s : node->elseBranch)
            analyzeNode(s);
        popScope();
    }
}

void SemanticAnalyzer::analyzeAssign(const std::shared_ptr<AssignNode> &node)
{
    Symbol *sym = lookup(node->target);
    if (!sym)
    {
        std::ostringstream os;
        os << "Variável '" << node->target << "' não declarada";
        errs.push_back(os.str());
        return;
    }
    std::string et = analyzeExpr(node->expr);
    if (!typeEquals(et, sym->type))
    {
        std::ostringstream os;
        os << "Não é possível atribuir '" << et << "' a '" << node->target << "' do tipo '" << sym->type << "'";
        errs.push_back(os.str());
    }
}

void SemanticAnalyzer::analyzeShow(const std::shared_ptr<ShowNode> &node)
{
    for (auto &a : node->args)
        analyzeExpr(a);
}

void SemanticAnalyzer::analyzeDuring(const std::shared_ptr<DuringNode> &node)
{
    std::string ct = analyzeExpr(node->cond);
    if (!typeEquals(ct, "boolean"))
        errs.push_back("Condição do 'during' deve ser boolean");
    pushScope();
    for (auto &s : node->body)
        analyzeNode(s);
    popScope();
}

void SemanticAnalyzer::analyzeForEach(const std::shared_ptr<ForEachNode> &node)
{
    std::string seqt = analyzeExpr(node->toExpr);
    std::string elemType;
    bool ok = true;

    if (endsWith(seqt, "[]"))
    {
        elemType = seqt.substr(0, seqt.size() - 2);
    }
    else if (seqt == "string")
    {
        elemType = "string";
    }
    else
    {
        ok = false;
    }

    if (!ok)
    {
        std::ostringstream os;
        os << "Objeto não iterável no foreach (esperado string ou colecao): tipo '" << seqt << "'";
        errs.push_back(os.str());
    }

    pushScope();
    if (elemType.empty())
        elemType = seqt;
    declare(node->ident, elemType, false, true);
    for (auto &s : node->body)
        analyzeNode(s);
    popScope();
}

void SemanticAnalyzer::analyzeRepeat(const std::shared_ptr<RepeatNode> &node)
{
    std::string ut = analyzeExpr(node->untilExpr);
    if (!typeEquals(ut, "boolean"))
        errs.push_back("Expressão 'until' do 'repeat' deve ser boolean");
    pushScope();
    for (auto &s : node->body)
        analyzeNode(s);
    popScope();
}

std::string SemanticAnalyzer::analyzeExpr(const std::shared_ptr<ExprNode> &expr)
{
    if (!expr)
        return "";
    if (auto lit = std::dynamic_pointer_cast<LiteralNode>(expr))
    {
        std::string v = lit->lit;
        if (!v.empty() && (v.front() == '"' || v.front() == '\''))
        {
            lit->annotatedType = "string";
            return "string";
        }
        if (v == "True" || v == "False" || v == "true" || v == "false")
        {
            lit->annotatedType = "boolean";
            return "boolean";
        }
        if (v.find('.') != std::string::npos)
        {
            lit->annotatedType = "floatingPoint";
            return "floatingPoint";
        }
        lit->annotatedType = "integer";
        return "integer";
    }
    if (auto id = std::dynamic_pointer_cast<IdentifierNode>(expr))
    {
        Symbol *s = lookup(id->name);
        if (!s)
        {
            std::ostringstream os;
            os << "Identificador '" << id->name << "' não declarado";
            errs.push_back(os.str());
            id->annotatedType = "";
            return "";
        }
        id->annotatedType = s->type;
        return s->type;
    }
    if (auto bin = std::dynamic_pointer_cast<BinaryOpNode>(expr))
    {
        std::string L = analyzeExpr(bin->left);
        std::string R = analyzeExpr(bin->right);
        std::string op = bin->op;
        if (op == "and" || op == "or")
        {
            if (!typeEquals(L, "boolean") || !typeEquals(R, "boolean"))
            {
                errs.push_back("Operadores lógicos 'and/or' esperam booleanos");
            }
            bin->annotatedType = "boolean";
            return "boolean";
        }
        if (op == "==" || op == "!=" || op == "<" || op == ">" || op == "<=" || op == ">=")
        {
            if (!L.empty() && !R.empty() && !typeEquals(L, R))
            {
                errs.push_back("Comparação entre tipos incompatíveis: '" + L + "' e '" + R + "'");
            }
            bin->annotatedType = "boolean";
            return "boolean";
        }
        if (op == "+" || op == "-" || op == "*" || op == "/" || op == "%")
        {
            if (!isNumericType(L) || !isNumericType(R))
            {
                errs.push_back("Operador aritmético usado em operandos não numéricos");
                bin->annotatedType = "";
                return "";
            }
            if (L == "floatingPoint" || R == "floatingPoint")
            {
                bin->annotatedType = "floatingPoint";
                return "floatingPoint";
            }
            bin->annotatedType = "integer";
            return "integer";
        }
        bin->annotatedType = "";
        return "";
    }
    if (auto un = std::dynamic_pointer_cast<UnaryOpNode>(expr))
    {
        std::string t = analyzeExpr(un->operand);
        if (un->op == "not")
        {
            if (!typeEquals(t, "boolean"))
                errs.push_back("Operador 'not' espera boolean");
            un->annotatedType = "boolean";
            return "boolean";
        }
        un->annotatedType = t;
        return t;
    }
    if (auto arr = std::dynamic_pointer_cast<ArrayLiteralNode>(expr))
    {
        std::string elemType = "";
        for (auto &e : arr->elements)
        {
            std::string et = analyzeExpr(e);
            if (et.empty())
                continue;
            if (elemType.empty())
                elemType = et;
            else if (!typeEquals(elemType, et))
            {
                std::ostringstream os;
                os << "Elementos do array literal têm tipos diferentes: '" << elemType << "' vs '" << et << "'";
                errs.push_back(os.str());
            }
        }
        if (elemType.empty())
        {
            arr->annotatedType = "unknown[]";
            return "unknown[]";
        }
        arr->annotatedType = elemType + "[]";
        return arr->annotatedType;
    }

    return "";
}

bool SemanticAnalyzer::isNumericType(const std::string &t) const
{
    return t == "integer" || t == "floatingPoint";
}
bool SemanticAnalyzer::typeEquals(const std::string &a, const std::string &b) const
{
    if (a.empty() || b.empty())
        return false;
    return a == b;
}

void SemanticAnalyzer::reportError(int line, const std::string &msg)
{
    std::ostringstream os;
    os << "Linha " << line << ": " << msg;
    errs.push_back(os.str());
}