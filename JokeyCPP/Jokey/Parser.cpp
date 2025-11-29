#include "Parser.h"
#include <stdexcept>
#include <iostream>
#include <sstream>

Parser::Parser(const std::vector<Token> &tokens) : tokens(tokens), pos(0) {}

bool Parser::eof() const { return pos >= tokens.size(); }

const Token &Parser::peek(int k) const
{
    size_t idx = pos + k;
    static Token eofTok{"", "EOF", -1, -1};
    if (idx >= tokens.size())
        return eofTok;
    return tokens[idx];
}

Token Parser::advance()
{
    if (!eof())
        return tokens[pos++];
    return Token{"", "EOF", -1, -1};
}
bool Parser::check(const std::string &t) const
{
    if (eof())
        return false;
    const Token &tk = peek();
    return tk.type == t || tk.lexeme == t;
}
Token Parser::consume(const std::string &expected, const std::string &errMsg)
{
    if (check(expected))
        return advance();
    const Token &tk = peek();
    std::ostringstream oss;
    oss << errMsg << " (found '" << tk.lexeme << "' type=" << tk.type << " at " << tk.line << ":" << tk.col << ")";
    throw std::runtime_error(oss.str());
}

bool Parser::isTypeKeyword(const std::string &lex) const
{
    return lex == "integer" || lex == "floatingPoint" || lex == "string" || lex == "boolean" || lex == "null";
}
bool Parser::isReservedWord(const std::string &lex) const
{
    static std::vector<std::string> res = {
        "Keep", "keep", "as", "static", "show", "if", "elsif", "then", "end", "during", "ForEach", "foreach", "to", "repeat", "until", "define", "regress", "return"};
    for (auto &r : res)
        if (r == lex)
            return true;
    return false;
}

std::shared_ptr<ProgramNode> Parser::parseProgram()
{
    auto prog = std::make_shared<ProgramNode>();
    while (!eof())
    {
        if (peek().type == "COMENT�RIO")
        {
            advance();
            continue;
        }
        if (peek().lexeme == "end" || peek().type == "EOF")
            break;
        auto node = parseDeclarationOrStatement();

        if (!node)
            break;

        if (dynamic_cast<VarDeclNode *>(node.get()) || dynamic_cast<FuncDeclNode *>(node.get()))
        {
            prog->declarations.push_back(node);
        }
        else
        {
            prog->statements.push_back(node);
        }
    }
    return prog;
}

std::shared_ptr<ASTNode> Parser::parseDeclarationOrStatement()
{
    if (eof())
        return nullptr;
    Token t = peek();

    if (t.lexeme == "Keep" || t.lexeme == "keep")
        return parseVarDecl();
    if (t.lexeme == "define")
        return parseFuncDecl();
    return parseStatement();
}

std::shared_ptr<VarDeclNode> Parser::parseVarDecl()
{
    if (check("keep"))
        advance();

    bool isStatic = false;
    if (check("static"))
    {
        advance();
        isStatic = true;
    }

    const Token &ttype = peek();
    if (!isTypeKeyword(ttype.lexeme))
    {
        std::ostringstream os;
        os << "Expected type in var decl (found '" << ttype.lexeme << "' type=" << ttype.type
           << " at " << ttype.line << ":" << ttype.col << ")";
        throw std::runtime_error(os.str());
    }

    std::string typeName = ttype.lexeme;
    advance();

    if (!eof() && peek().lexeme == "[")
    {
        advance();
        if (peek().lexeme != "]")
        {
            const Token &tk = peek();
            std::ostringstream os;
            os << "Expected ']' after '[' in type (found '" << tk.lexeme << "' type=" << tk.type
               << " at " << tk.line << ":" << tk.col << ")";
            throw std::runtime_error(os.str());
        }
        advance();
        typeName += "[]";
    }

    const Token &idt = peek();
    if (idt.type != "IDENTIFICADOR" && idt.type != "IDENT" && !isReservedWord(idt.lexeme))
    {
        std::ostringstream os;
        os << "Expected identifier after type in var decl (found '" << idt.lexeme << "' type=" << idt.type
           << " at " << idt.line << ":" << idt.col << ")";
        throw std::runtime_error(os.str());
    }
    std::string ident = idt.lexeme;
    advance();

    consume("as", "Expected 'as' after variable name in var decl");

    ExprNodePtr initExpr = nullptr;
    if (!check(";") && !check("EOF"))
    {
        initExpr = parseExpression();
    }

    if (check(";"))
        advance();

    auto node = std::make_shared<VarDeclNode>();
    node->typeName = typeName;
    node->ident = ident;
    node->init = initExpr;
    node->isStatic = isStatic;
    return node;
}

std::shared_ptr<ReturnNode> Parser::parseReturn()
{
    if (check("regress"))
        advance();
    else if (check("return"))
        advance();

    auto node = std::make_shared<ReturnNode>();

    if (check("end") || check(";"))
    {
    }
    else
    {
        node->expr = parseExpression();
    }

    if (check("end"))
        advance();
    else if (check(";"))
        advance();

    return node;
}

std::shared_ptr<FuncDeclNode> Parser::parseFuncDecl()
{
    auto node = std::make_shared<FuncDeclNode>();
    consume("define", "Expected 'define' for func decl");

    Token t = advance();
    node->retType = t.lexeme;

    Token name = consume("IDENTIFICADOR", "Expected function name");
    node->name = name.lexeme;

    consume("(", "Expected '(' after function name");

    if (!check(")"))
    {
        while (true)
        {
            Token ptype = advance();
            Token pname = consume("IDENTIFICADOR", "Expected param name");
            node->params.push_back({ptype.lexeme, pname.lexeme});
            if (check(")"))
                break;
            consume(",", "Expected ',' between params");
        }
    }
    consume(")", "Expected ')' after params");
    consume("{", "Expected '{' for function body");

    while (!eof() && !check("}"))
    {
        node->body.push_back(parseDeclarationOrStatement());
    }

    consume("}", "Expected '}' closing function");
    return node;
}

std::vector<std::shared_ptr<ASTNode>> Parser::parseBlockDeclarations()
{
    std::vector<std::shared_ptr<ASTNode>> out;
    while (!eof() && (peek().lexeme == "Keep" || peek().lexeme == "keep"))
    {
        out.push_back(parseVarDecl());
    }
    return out;
}

std::vector<std::shared_ptr<ASTNode>> Parser::parseBlockCommands()
{
    std::vector<std::shared_ptr<ASTNode>> out;
    while (!eof() && !check("end") && !check("}") && !check("elsif") && !check("else"))
    {
        if (peek().lexeme == "keep" || peek().lexeme == "Keep")
        {
            out.push_back(parseVarDecl());
        }
        else if (peek().lexeme == "regress")
        {
            out.push_back(parseReturn());
        }
        else
        {
            out.push_back(parseStatement());
        }
    }
    return out;
}

std::shared_ptr<ASTNode> Parser::parseStatement()
{
    if (peek().lexeme == "regress" || peek().lexeme == "return")
    {
        return parseReturn();
    }

    if (peek().lexeme == "if")
        return parseIf();

    if (peek().lexeme == "during" || peek().lexeme == "ForEach" || peek().lexeme == "foreach" || peek().lexeme == "repeat")
        return parseLoop();

    if (peek().lexeme == "show")
        return parseShow();

    if (peek().type == "IDENTIFICADOR")
    {
        if (peek(1).lexeme == "=")
            return parseAssign();
    }

    ExprNodePtr e = parseExpression();
    if (check(";"))
        advance();
    return e;
}

std::shared_ptr<IfNode> Parser::parseIf()
{
    consume("if", "Expected 'if'");
    auto node = std::make_shared<IfNode>();

    node->cond = parseExpression();

    consume("then", "Expected 'then'");
    consume("{", "Expected '{' after 'then'");

    node->thenBranch = parseBlockCommands();

    consume("}", "Expected '}' after if body");

    while (check("elsif"))
    {
        advance();
        ExprNodePtr c = parseExpression();
        consume("then", "Expected 'then' after 'elsif'");
        consume("{", "Expected '{' after 'then' in elsif");

        auto body = parseBlockCommands();

        consume("}", "Expected '}' after elsif body");

        node->elsi.push_back({c, body});
    }

    if (check("else"))
    {
        advance();
        consume("{", "Expected '{' after 'else'");
        node->elseBranch = parseBlockCommands();
        consume("}", "Expected '}' after else body");
    }

    consume("end", "Expected 'end' after if statement");
    return node;
}

std::shared_ptr<ASTNode> Parser::parseLoop()
{
    if (peek().lexeme == "during")
    {
        advance();
        auto node = std::make_shared<DuringNode>();

        consume("(", "Expected '(' after during");
        node->cond = parseExpression();
        consume(")", "Expected ')' after condition");

        consume("{", "Expected '{' after during");

        node->body = parseBlockCommands();

        consume("}", "Expected '}' after during body");
        consume("end", "Expected 'end' after during");

        return node;
    }

    if (peek().lexeme == "ForEach" || peek().lexeme == "foreach")
    {
        advance();
        auto node = std::make_shared<ForEachNode>();

        consume("(", "Expected '(' after foreach");

        Token var = consume("IDENTIFICADOR", "Expected identifier in foreach");
        node->ident = var.lexeme;

        consume("in", "Expected 'in' in foreach");

        node->toExpr = parseExpression();

        consume(")", "Expected ')' after foreach expression");
        consume("{", "Expected '{' after foreach");

        node->body = parseBlockCommands();

        consume("}", "Expected '}' after foreach body");
        consume("end", "Expected 'end' after foreach");

        return node;
    }

    if (peek().lexeme == "repeat")
    {
        advance();
        auto node = std::make_shared<RepeatNode>();
        consume("{", "Expected '{' after repeat");
        node->body = parseBlockCommands();
        consume("}", "Expected '}' after repeat body");
        consume("until", "Expected 'until' after repeat body");
        node->untilExpr = parseExpression();
        consume("end", "Expected 'end' after repeat");
        return node;
    }
    throw std::runtime_error("Unknown loop construct");
}

std::shared_ptr<AssignNode> Parser::parseAssign()
{
    Token id = consume("IDENTIFICADOR", "Expected identifier in assignment");
    consume("=", "Expected '=' in assignment");
    ExprNodePtr e = parseExpression();
    if (check(";"))
        advance();
    auto node = std::make_shared<AssignNode>();
    node->target = id.lexeme;
    node->expr = e;
    return node;
}

std::shared_ptr<ShowNode> Parser::parseShow()
{
    consume("show", "Expected 'show'");
    auto node = std::make_shared<ShowNode>();

    bool hasParens = false;
    if (check("("))
    {
        advance();
        hasParens = true;
    }

    bool isEmpty = (hasParens && check(")")) || (!hasParens && check(";"));

    if (!isEmpty)
    {
        while (true)
        {
            node->args.push_back(parseExpression());

            if (check(","))
            {
                advance();
                continue;
            }
            break;
        }
    }

    if (hasParens)
    {
        consume(")", "Expected ')' after show arguments");
    }

    if (!check(";"))
    {
        while (true)
        {
            node->args.push_back(parseExpression());
            if (check(","))
            {
                advance();
                continue;
            }
            break;
        }
    }
    consume(";", "Expected ';' after show");
    return node;
}

ExprNodePtr Parser::parseExpression() { return parseLogic(); }

ExprNodePtr Parser::parseLogic()
{
    ExprNodePtr left = parseRelational();
    while (check("and") || check("or"))
    {
        Token op = advance();
        ExprNodePtr right = parseRelational();
        auto bin = std::make_shared<BinaryOpNode>();
        bin->op = op.lexeme;
        bin->left = left;
        bin->right = right;
        left = bin;
    }
    return left;
}

ExprNodePtr Parser::parseRelational()
{
    ExprNodePtr left = parseAdd();
    if (check("==") || check("!=") || check(">=") || check("<=") || check(">") || check("<"))
    {
        Token op = advance();
        ExprNodePtr right = parseAdd();
        auto bin = std::make_shared<BinaryOpNode>();
        bin->op = op.lexeme;
        bin->left = left;
        bin->right = right;
        return bin;
    }
    return left;
}

ExprNodePtr Parser::parseAdd()
{
    ExprNodePtr left = parseTerm();
    while (check("+") || check("-"))
    {
        Token op = advance();
        ExprNodePtr right = parseTerm();
        auto bin = std::make_shared<BinaryOpNode>();
        bin->op = op.lexeme;
        bin->left = left;
        bin->right = right;
        left = bin;
    }
    return left;
}

ExprNodePtr Parser::parseTerm()
{
    ExprNodePtr left = parseFactor();
    while (check("*") || check("/") || check("%"))
    {
        Token op = advance();
        ExprNodePtr right = parseFactor();
        auto bin = std::make_shared<BinaryOpNode>();
        bin->op = op.lexeme;
        bin->left = left;
        bin->right = right;
        left = bin;
    }
    return left;
}

std::shared_ptr<ExprNode> Parser::parseFactor()
{
    Token t = peek();

    // 1. Tratamento de Unários (menos ou not)
    if (t.lexeme == "-" || t.lexeme == "not" || t.lexeme == "!" || t.lexeme == "nao")
    {
        advance(); // Consome o operador
        auto node = std::make_shared<UnaryOpNode>();
        node->op = t.lexeme;
        node->operand = parseFactor(); // Recursão para pegar o valor
        return node;
    }

    // 2. Parênteses
    if (t.lexeme == "(")
    {
        advance();
        ExprNodePtr e = parseExpression();
        consume(")", "Expected ')' after expression");
        return e;
    }

    // 3. Array Literal [1, 2]
    if (t.lexeme == "[")
    {
        advance();
        auto arr = std::make_shared<ArrayLiteralNode>();
        if (check("]"))
        {
            advance();
            return arr;
        }
        while (true)
        {
            arr->elements.push_back(parseExpression());
            if (check(","))
            {
                advance();
                continue;
            }
            break;
        }
        consume("]", "Expected ']' after array literal");
        return arr;
    }

    // 4. Identificador ou Chamada de Função
    if (t.type == "IDENTIFICADOR")
    {
        Token id = advance();

        // Chamada de função: fib(10)
        if (check("("))
        {
            advance();
            auto callNode = std::make_shared<CallNode>();
            callNode->funcName = id.lexeme;
            // O tipo será preenchido pelo Semantic/CodeGen

            if (!check(")"))
            {
                while (true)
                {
                    callNode->args.push_back(parseExpression());
                    if (check(")"))
                        break;
                    consume(",", "Expected ',' between func args");
                }
            }
            consume(")", "Expected ')' after func args");
            return callNode;
        }

        // Variável normal
        auto node = std::make_shared<IdentifierNode>();
        node->name = id.lexeme;
        return node;
    }

    // 5. Literais
    if (t.type == "INTEIRO" || t.type == "PONTO_FLUTUANTE" || t.type == "BOOLEANO" || t.type == "STRING")
    {
        advance();
        auto lit = std::make_shared<LiteralNode>();
        lit->lit = t.lexeme;
        return lit;
    }

    // 6. Null
    if (t.lexeme == "null")
    {
        advance();
        auto lit = std::make_shared<LiteralNode>();
        lit->lit = "null";
        return lit;
    }

    throw std::runtime_error("Unexpected token in factor: '" + t.lexeme + "' type=" + t.type);
}