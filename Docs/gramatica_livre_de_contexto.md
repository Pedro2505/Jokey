# Gramática Livre de Contexto

## Estrutura Principal

Program = { Declaration | Statement } ;

## Declarações

Declaration = VarDecl | FuncDecl ;

## Variáveis (Globais ou Locais)

VarDecl = "keep" [ "static" ] Type ident "as" Expr [ ";" ] ;

## Funções

FuncDecl = "define" Type ident "(" [ FormPars ] ")" "{" { Declaration | Statement } "}" ;
FormPars = Type ident { "," Type ident } ;

## Tipos

Type = SimpleType [ "[" "]" ] ;
SimpleType = "integer" | "floatingPoint" | "string" | "boolean" | "null" | "void" ;

## Comandos (Statements)

Statement = AssignStmt
| CallStmt
| IfStmt
| LoopStmt
| PrintStmt
| ReturnStmt
| Block ;

AssignStmt = ident "=" Expr [ ";" ] ;
CallStmt = ident "(" [ Expr { "," Expr } ] ")" [ ";" ] ;
PrintStmt = "show" [ Expr { "," Expr } ] [ ";" ] ;
ReturnStmt = ("regress" | "return") [ Expr ] ( "end" | ";" ) ;
Block = "{" { Declaration | Statement } "}" ;

## Fluxo de Controle

IfStmt = "if" Expr "then" "{" { Statement } "}"
{ "elsif" Expr "then" "{" { Statement } "}" }
[ "else" "{" { Statement } "}" ]
"end" ;

LoopStmt = "during" "(" Expr ")" "{" { Statement } "}" "end"
| "foreach" "(" ident "in" Expr ")" "{" { Statement } "}" "end"
| "repeat" "{" { Statement } "}" "until" Expr "end" ;

## Expressões (Precedência: Lógica < Relacional < Soma < Multiplicação)

Expr = LogicExpr ;
LogicExpr = RelExpr { ( "and" | "or" | "&&" | "||" ) RelExpr } ;
RelExpr = AddExpr [ RelOp AddExpr ] ;
AddExpr = Term { ( "+" | "-" ) Term } ;
Term = Factor { ( "\*" | "/" | "%" ) Factor } ;

Factor = UnaryOp Factor
| ident [ "(" [ Expr { "," Expr } ] ")" ]
| literal_integer
| literal_floatingPoint
| literal_string
| literal_boolean
| ArrayLiteral
| "null"
| "(" Expr ")" ;

RelOp = "==" | "!=" | ">" | "<" | ">=" | "<=" ;
UnaryOp = "-" | "not" | "!" ;
ArrayLiteral = "[" [ Expr { "," Expr } ] "]" ;
