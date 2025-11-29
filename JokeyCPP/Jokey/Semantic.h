#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <stdexcept>
#include "AST.h"

struct Symbol
{
    std::string type;
    bool isConst = false;
};

class Scope
{
public:
    std::unordered_map<std::string, Symbol> table;

    void declare(const std::string &name, const Symbol &sym)
    {
        if (table.count(name))
            throw std::runtime_error("Erro: simbolo duplicado '" + name + "'");
        table[name] = sym;
    }

    bool has(const std::string &name)
    {
        return table.count(name);
    }

    Symbol get(const std::string &name)
    {
        return table.at(name);
    }
};

class SemanticAnalyzer
{
public:
    std::vector<Scope> scopes;

    SemanticAnalyzer()
    {
        pushScope();
    }

    void pushScope() { scopes.push_back(Scope()); }
    void popScope() { scopes.pop_back(); }

    Symbol resolve(const std::string &name)
    {
        for (int i = scopes.size() - 1; i >= 0; --i)
            if (scopes[i].has(name))
                return scopes[i].get(name);
        throw std::runtime_error("Erro: variavel '" + name + "' n�o declarada");
    }

    void declare(const std::string &name, const std::string &type)
    {
        scopes.back().declare(name, Symbol{type});
    }

    void analyze(ASTNodePtr root)
    {
        auto *prog = dynamic_cast<ProgramNode *>(root.get());

        for (auto &decl : prog->declarations)
            analyzeDecl(decl);

        for (auto &stmt : prog->statements)
            analyzeStmt(stmt);
    }

    void analyzeDecl(ASTNodePtr node)
    {
        if (auto v = dynamic_cast<VarDeclNode *>(node.get()))
        {
            declare(v->ident, v->typeName);

            if (v->init)
            {
                std::string initType = analyzeExpr(v->init);
                checkAssign(v->typeName, initType,
                            "Inicializacao invalida da variavel '" + v->ident + "'");
            }
        }
        else if (auto f = dynamic_cast<FuncDeclNode *>(node.get()))
        {
            declare(f->name, f->retType);

            pushScope();

            for (auto &p : f->params)
                declare(p.second, p.first);

            for (auto &stmt : f->body)
                analyzeStmt(stmt);

            if (f->regressExpr)
            {
                std::string ret = analyzeExpr(f->regressExpr);
                checkAssign(f->retType, ret,
                            "Retorno incompativel na funcao '" + f->name + "'");
            }

            popScope();
        }
    }

    void analyzeStmt(ASTNodePtr node)
    {
        if (auto a = dynamic_cast<AssignNode *>(node.get()))
        {
            Symbol s = resolve(a->target);
            std::string exprType = analyzeExpr(a->expr);
            checkAssign(s.type, exprType,
                        "Atribuicao invalida para '" + a->target + "'");
        }
        else if (auto s = dynamic_cast<ShowNode *>(node.get()))
        {
            for (auto &e : s->args)
                analyzeExpr(e);
        }
        else if (auto d = dynamic_cast<DuringNode *>(node.get()))
        {
            std::string cond = analyzeExpr(d->cond);
            requireBool(cond, "Condicao do 'during' deve ser booleana");

            pushScope();
            for (auto &st : d->body)
                analyzeStmt(st);
            popScope();
        }
        else if (auto f = dynamic_cast<ForEachNode *>(node.get()))
        {
            std::string listType = analyzeExpr(f->toExpr);

            if (listType.size() < 3 || listType.back() != ']')
                throw std::runtime_error("Expressao em foreach nao e um vetor");

            std::string elementType = listType.substr(0, listType.size() - 2);

            pushScope();
            declare(f->ident, elementType);
            for (auto &st : f->body)
                analyzeStmt(st);
            popScope();
        }
        else if (auto r = dynamic_cast<RepeatNode *>(node.get()))
        {
            pushScope();
            for (auto &st : r->body)
                analyzeStmt(st);
            popScope();

            std::string cond = analyzeExpr(r->untilExpr);
            requireBool(cond, "Condicao do 'repeat' deve ser booleana");
        }
        else if (auto i = dynamic_cast<IfNode *>(node.get()))
        {
            std::string cond = analyzeExpr(i->cond);
            requireBool(cond, "Condicao do 'if' deve ser booleana");

            pushScope();
            for (auto &st : i->thenBranch)
                analyzeStmt(st);
            popScope();

            for (auto &el : i->elsi)
            {
                std::string c = analyzeExpr(el.first);
                requireBool(c, "Condicao do 'elsif' deve ser booleana");

                pushScope();
                for (auto &st : el.second)
                    analyzeStmt(st);
                popScope();
            }

            pushScope();
            for (auto &st : i->elseBranch)
                analyzeStmt(st);
            popScope();
        }
    }

    std::string analyzeExpr(ExprNodePtr expr)
    {
        if (auto id = dynamic_cast<IdentifierNode *>(expr.get()))
        {
            Symbol s = resolve(id->name);
            expr->annotatedType = s.type;
            return s.type;
        }
        if (auto lit = dynamic_cast<LiteralNode *>(expr.get()))
        {
            return inferLiteral(lit->lit);
        }
        if (auto b = dynamic_cast<BinaryOpNode *>(expr.get()))
        {
            std::string L = analyzeExpr(b->left);
            std::string R = analyzeExpr(b->right);

            if (b->op == "+" || b->op == "-" || b->op == "*" || b->op == "/")
            {
                requireNumber(L, "Operador aritmetico incompativel");
                requireNumber(R, "Operador aritmetico incompativel");
                return "integer";
            }
            if (b->op == "==" || b->op == ">" || b->op == "<")
            {
                return "boolean";
            }
        }
        if (auto u = dynamic_cast<UnaryOpNode *>(expr.get()))
        {
            return analyzeExpr(u->operand);
        }

        return "unknown";
    }

    std::string inferLiteral(const std::string &lit)
    {
        if (lit == "True" || lit == "False")
            return "boolean";
        if (isdigit(lit[0]))
            return "integer";
        return "string";
    }

    void requireBool(const std::string &t, const std::string &msg)
    {
        if (t != "boolean")
            throw std::runtime_error(msg);
    }

    void requireNumber(const std::string &t, const std::string &msg)
    {
        if (t != "integer")
            throw std::runtime_error(msg);
    }

    void checkAssign(const std::string &expected,
                     const std::string &got,
                     const std::string &msg)
    {
        if (expected != got)
            throw std::runtime_error(msg + " (esperado " + expected + ", obtido " + got + ")");
    }
};
