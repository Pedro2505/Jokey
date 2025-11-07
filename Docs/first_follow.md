# First e Follow

```
FIRST(Program) = { "abyss" }

FIRST(VarDecl) = { "keep" }

FIRST(FuncDecl) = { "define" }

FIRST(ClassDecl) = { "class" }

FIRST(ContractDecl) = { "component" }

FIRST(FormPars) = FIRST(Type) = { "integer", "floatingPoint", "string", "boolean", "null" } ∪ { ε }

FIRST(Type) = { "integer", "floatingPoint", "string", "boolean", "null" }

FIRST(Statement) = { ident, "if", "during", "foreach", "repeat", "show", "regress", "{" }

FIRST(AssignStmt) = { ident }

FIRST(IfStmt) = { "if" }

FIRST(LoopStmt) = { "during", "foreach", "repeat" }

FIRST(PrintStmt) = { "show" }

FIRST(Block) = { "{" }

FIRST(Expr) = FIRST(LogicExpr) = FIRST(RelExpr) = FIRST(ArithExpr) = FIRST(Term) = FIRST(Factor) =
  { ident, literal_integer, literal_floatingPoint, literal_string, literal_boolean, "null", "(" }

FIRST(RelOp) = { "==", "!=", ">", "<", ">=", "<=" }

FIRST(AddOp) = { "+", "-" }

FIRST(MulOp) = { "*", "/", "%" }

FIRST(Factor) = { ident, literal_integer, literal_floatingPoint, literal_string, literal_boolean, "null", "(" }
```

```
FOLLOW(Program) = { $ }

FOLLOW(VarDecl) = { "keep", "define", "class", "component", "if", "during", "foreach", "repeat", "show", "regress", "{", "}", "end", ident }

FOLLOW(FuncDecl) = { "keep", "define", "class", "component", "if", "during", "foreach", "repeat", "show", "regress", "{", "}", "end", ident }

FOLLOW(ClassDecl) = { "keep", "define", "class", "component", "if", "during", "foreach", "repeat", "show", "regress", "{", "}", "end", ident }

FOLLOW(ContractDecl) = { "keep", "define", "class", "component", "if", "during", "foreach", "repeat", "show", "regress", "{", "}", "end", ident }

FOLLOW(FormPars) = { ")" }

FOLLOW(Type) = { ident, ")" , "," }

FOLLOW(Statement) = { "if", "during", "foreach", "repeat", "show", "regress", "{", "}", "end", "keep", "define", "class", "component", ident }

FOLLOW(AssignStmt) = FOLLOW(Statement)

FOLLOW(IfStmt) = FOLLOW(Statement)

FOLLOW(LoopStmt) = FOLLOW(Statement)

FOLLOW(PrintStmt) = FOLLOW(Statement)

FOLLOW(Block) = FOLLOW(Statement) ∪ { "}" }

FOLLOW(Expr) = { ";", ")", ",", "then", "to", "until", "}", "end", "and", "or" }

FOLLOW(RelExpr) = FOLLOW(Expr)

FOLLOW(ArithExpr) = FOLLOW(Expr)

FOLLOW(Term) = { "+", "-", "*", "/", "%", RelOp, ")", ",", ";", "then", "to", "until", "and", "or", "}", "end" }

FOLLOW(Factor) = FOLLOW(Term)
```

## A gramática é LL(1)?

- 1° Condição: FIRST(α) ∩ FIRST(β) = ∅

```
FIRST(VarDecl) ∩ FIRST(FuncDecl) = ∅
FIRST(FuncDecl) ∩ FIRST(ClassDecl) = ∅
FIRST(ClassDecl) ∩ FIRST(ContractDecl) = ∅
FIRST(ContractDecl) ∩ FIRST(Statement) = ∅
FIRST(LoopStmt) ∩ FIRST(IfStmt) = ∅
FIRST(IfStmt) ∩ FIRST(PrintStmt) = ∅
FIRST(PrintStmt) ∩ FIRST(Block) = ∅
FIRST(AddOp) ∩ FIRST(MulOp) = ∅
FIRST(RelOp) ∩ FIRST(AddOp) = ∅
```

- 2° Condição: se ε ∈ FIRST(α), então FIRST(β) ∩ FOLLOW(A) = ∅

```
ε ∈ FIRST(FormPars) → FIRST(Type) ∩ FOLLOW(FormPars) = ∅
ε ∈ FIRST({Statement}) → FIRST(Statement) ∩ FOLLOW(Statement) = ∅
```

- 3° Condição: se ε ∈ FIRST(β), então FIRST(α) ∩ FOLLOW(A) = ∅

```
ε ∈ FIRST({VarDecl | FuncDecl | ClassDecl | ContractDecl | Statement}) → FIRST(VarDecl) ∩ FOLLOW(Program) = ∅
ε ∈ FIRST({Statement}) → FIRST(Statement) ∩ FOLLOW(Block) = ∅
```

A gramática é LL(1), pois todas as condições foram atendidas
