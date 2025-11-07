# Gramática livre de contexto

Program = "abyss" ident "{" { VarDecl | FuncDecl | ClassDecl | ContractDecl | Statement } "}" "end" ;

VarDecl = "keep" [ "static" ] Type ident "as" Value ;

FuncDecl = "define" Type ident "(" [ FormPars ] ")" "{" { Statement } "regress" Expr "end" "}" ;

ClassDecl = "class" ident "{" { VarDecl } "}" "end" ;

ContractDecl = "component" ident "{" { VarDecl | Statement } "}" "end" ;

FormPars = Type ident { "," Type ident } ;

Type = "integer" | "floatingPoint" | "string" | "boolean" | "null" ;

Statement = AssignStmt
| IfStmt
| LoopStmt
| PrintStmt
| "regress" Expr ";"
| Block ;

AssignStmt = ident "=" Expr ";" ;

IfStmt = "if" Expr "then" { Statement }
[ "elsif" Expr "then" { Statement } ]
[ "else" { Statement } ]
"end" ;

LoopStmt = "during" Expr "{" { Statement } "}" "end"
| "foreach" ident "to" Expr "{" { Statement } "}" "end"
| "repeat" "{" { Statement } "}" "until" Expr "end" ;

PrintStmt = "show" [ Expr { "," Expr } ] ";" ;

Block = "{" { Statement } "}" ;

Expr = LogicExpr ;

LogicExpr = RelExpr { ( "and" | "or" ) RelExpr } ;

RelExpr = ArithExpr [ RelOp ArithExpr ] ;

RelOp = "==" | "!=" | ">" | "<" | ">=" | "<=" ;

ArithExpr = Term { AddOp Term } ;

AddOp = "+" | "-" ;

Term = Factor { MulOp Factor } ;

MulOp = "\*" | "/" | "%" ;

Factor = ident
| literal_integer
| literal_floatingPoint
| literal_string
| literal_boolean
| "null"
| "(" Expr ")" ;
