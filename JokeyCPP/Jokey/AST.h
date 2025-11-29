#pragma once
#include <string>
#include <vector>
#include <memory>
#include <iostream>

struct ASTNode
{
    virtual ~ASTNode() = default;
    virtual void dump(int indent = 0) const = 0;

protected:
    static void printIndent(int n)
    {
        for (int i = 0; i < n; i++)
            std::cout << "  ";
    }
};

using ASTNodePtr = std::shared_ptr<ASTNode>;

struct ExprNode : ASTNode
{
    std::string annotatedType;
};

using ExprNodePtr = std::shared_ptr<ExprNode>;

struct IdentifierNode : ExprNode
{
    std::string name;
    void dump(int indent = 0) const override
    {
        printIndent(indent);
        std::cout << "Identifier: " << name << "\n";
    }
};

struct LiteralNode : ExprNode
{
    std::string lit;
    void dump(int indent = 0) const override
    {
        printIndent(indent);
        std::cout << "Literal: " << lit << "\n";
    }
};

struct ArrayLiteralNode : ExprNode
{
    std::vector<ExprNodePtr> elements;

    void dump(int indent = 0) const override
    {
        printIndent(indent);
        std::cout << "ArrayLiteral[" << elements.size() << "]\n";
        for (const auto &el : elements)
        {
            if (el)
                el->dump(indent + 1);
        }
    }
};

struct BinaryOpNode : ExprNode
{
    std::string op;
    ExprNodePtr left, right;
    void dump(int indent = 0) const override
    {
        printIndent(indent);
        std::cout << "BinaryOp: " << op << "\n";
        if (left)
            left->dump(indent + 1);
        if (right)
            right->dump(indent + 1);
    }
};

struct UnaryOpNode : ExprNode
{
    std::string op;
    ExprNodePtr operand;
    void dump(int indent = 0) const override
    {
        printIndent(indent);
        std::cout << "UnaryOp: " << op << "\n";
        if (operand)
            operand->dump(indent + 1);
    }
};

struct CallNode : public ExprNode
{
    std::string funcName;
    std::vector<std::shared_ptr<ExprNode>> args;

    void dump(int indent = 0) const override
    {
        printIndent(indent);
        std::cout << "Call: " << funcName << "\n";
        for (auto &arg : args)
        {
            if (arg)
                arg->dump(indent + 1);
        }
    }
};

struct ProgramNode : ASTNode
{
    std::vector<ASTNodePtr> declarations;
    std::vector<ASTNodePtr> statements;
    void dump(int indent = 0) const override
    {
        printIndent(indent);
        std::cout << "Program\n";
        printIndent(indent + 1);
        std::cout << "Declarations:\n";
        for (auto &d : declarations)
            d->dump(indent + 2);
        printIndent(indent + 1);
        std::cout << "Statements:\n";
        for (auto &s : statements)
            s->dump(indent + 2);
    }
};

struct VarDeclNode : ASTNode
{
    std::string typeName;
    std::string ident;
    ASTNodePtr init;
    bool isStatic = false;
    void dump(int indent = 0) const override
    {
        printIndent(indent);
        std::cout << "VarDecl (";
        if (isStatic)
            std::cout << "static ";
        std::cout << typeName << " " << ident << ")\n";
        if (init)
        {
            printIndent(indent + 1);
            std::cout << "Init:\n";
            init->dump(indent + 2);
        }
    }
};

struct ReturnNode : ASTNode
{
    ExprNodePtr expr;
    void dump(int indent = 0) const override
    {
        printIndent(indent);
        std::cout << "Return (Regress)\n";
        if (expr)
            expr->dump(indent + 1);
    }
};

struct FuncDeclNode : ASTNode
{
    std::string retType;
    std::string name;
    std::vector<std::pair<std::string, std::string>> params;
    std::vector<ASTNodePtr> body;

    void dump(int indent = 0) const override
    {
        printIndent(indent);
        std::cout << "FuncDecl " << name << " : " << retType << "\n";
        printIndent(indent + 1);
        std::cout << "Params:\n";
        for (auto &p : params)
        {
            printIndent(indent + 2);
            std::cout << p.first << " " << p.second << "\n";
        }
        printIndent(indent + 1);
        std::cout << "Body:\n";
        for (auto &b : body)
            b->dump(indent + 2);
    }
};

struct IfNode : ASTNode
{
    ExprNodePtr cond;
    std::vector<ASTNodePtr> thenBranch;
    std::vector<std::pair<ExprNodePtr, std::vector<ASTNodePtr>>> elsi;
    std::vector<ASTNodePtr> elseBranch;
    void dump(int indent = 0) const override
    {
        printIndent(indent);
        std::cout << "If\n";
        printIndent(indent + 1);
        std::cout << "Cond:\n";
        if (cond)
            cond->dump(indent + 2);
        printIndent(indent + 1);
        std::cout << "Then:\n";
        for (auto &s : thenBranch)
            s->dump(indent + 2);
        for (auto &e : elsi)
        {
            printIndent(indent + 1);
            std::cout << "Elsif Cond:\n";
            e.first->dump(indent + 2);
            printIndent(indent + 1);
            std::cout << "Elsif Body:\n";
            for (auto &s : e.second)
                s->dump(indent + 2);
        }
        if (!elseBranch.empty())
        {
            printIndent(indent + 1);
            std::cout << "Else:\n";
            for (auto &s : elseBranch)
                s->dump(indent + 2);
        }
    }
};

struct AssignNode : ASTNode
{
    std::string target;
    ExprNodePtr expr;
    void dump(int indent = 0) const override
    {
        printIndent(indent);
        std::cout << "Assign: " << target << "\n";
        if (expr)
            expr->dump(indent + 1);
    }
};

struct ShowNode : ASTNode
{
    std::vector<ExprNodePtr> args;
    void dump(int indent = 0) const override
    {
        printIndent(indent);
        std::cout << "Show\n";
        for (auto &a : args)
            a->dump(indent + 1);
    }
};

struct DuringNode : ASTNode
{
    ExprNodePtr cond;
    std::vector<ASTNodePtr> body;
    void dump(int indent = 0) const override
    {
        printIndent(indent);
        std::cout << "During\n";
        if (cond)
            cond->dump(indent + 1);
        for (auto &s : body)
            s->dump(indent + 1);
    }
};

struct ForEachNode : ASTNode
{
    std::string ident;
    ExprNodePtr toExpr;
    std::vector<ASTNodePtr> body;
    void dump(int indent = 0) const override
    {
        printIndent(indent);
        std::cout << "ForEach " << ident << "\n";
        if (toExpr)
            toExpr->dump(indent + 1);
        for (auto &s : body)
            s->dump(indent + 1);
    }
};

struct RepeatNode : ASTNode
{
    std::vector<ASTNodePtr> body;
    ExprNodePtr untilExpr;
    void dump(int indent = 0) const override
    {
        printIndent(indent);
        std::cout << "Repeat\n";
        for (auto &s : body)
            s->dump(indent + 1);
        printIndent(indent);
        std::cout << "Until:\n";
        if (untilExpr)
            untilExpr->dump(indent + 1);
    }
};