# Gramática Formal da Linguagem Jokey

### Conjunto de Variáveis (V):

V = {Programa, BlocoDeclarações, Declaração, DeclaraçãoVariável,
DeclaraçãoFuncao, BlocoComandos, Comando, ComandoCondicional,
ComandoLaço, ComandoAtribuição, ComandoEscrita, ComandoRetorno,
Expressão, ExpressãoLógica, ExpressãoRelacional, ExpressãoAritmética,
Termo, Fator, ListaParametros, ListaArgumentos, Tipo}

### Conjunto de Terminais (T):

T = {keep, as, static, show, if, elsif, then, end,
during, foreach, to, in, repeat, until, define, regress, return,
integer, floatingPoint, string, boolean, null, void,
and, or, not, =, ==, !=, >, <, <=, >=, +, -, \*, /, %,
(, ), [, ], {, }, ", ', ;, ., ,}

### Simbolo Inicial:

S = Programa

## Principais regras de Produção Implementadas

- Programa ⇒ BlocoDeclarações BlocoComandos
- BlocoDeclarações ⇒ Declaração BlocoDeclarações | ε
- Declaração ⇒ DeclaraçãoVariavel | DeclaraçãoFuncao
- DeclaraçãoVariavel ⇒ keep [static] Tipo identificador as Expressão [;]
- Tipo ⇒ integer | floatingPoint | string | boolean | null | void | Tipo[]
- DeclaraçãoFunção ⇒ define Tipo identificador ( ListaParametros ) { BlocoComandos }

- BlocoComandos ⇒ Comando BlocoComandos | DeclaraçãoVariavel BlocoComandos | ε

- Comando ⇒ ComandoAtribuição
  | ComandoCondicional
  | ComandoLaço
  | ComandoEscrita
  | ComandoRetorno
  | ChamadaFuncao

- ComandoCondicional ⇒ if Expressão then { BlocoComandos } end
  | if Expressão then { BlocoComandos } elsif Expressão then { BlocoComandos } end
  | if Expressão then { BlocoComandos } else { BlocoComandos } end

- ComandoLaço ⇒ during ( Expressão ) { BlocoComandos } end
  | foreach ( identificador in Expressão ) { BlocoComandos } end
  | repeat { BlocoComandos } until Expressão end

- ComandoAtribuição ⇒ identificador = Expressão ;
- ChamadaFuncao ⇒ identificador ( ListaArgumentos ) ;
- ComandoEscrita ⇒ show ListaArgumentos ;
- ComandoRetorno ⇒ (regress | return) [ Expressão ] ( end | ; )

- ListaParametros ⇒ Tipo identificador , ListaParametros
  | Tipo identificador
  | ε

- ListaArgumentos ⇒ Expressão , ListaArgumentos
  | Expressão
  | ε

- Expressão ⇒ ExpressãoLógica

- ExpressãoLógica ⇒ ExpressãoRelacional
  | ExpressãoLógica (and | or | && | ||) ExpressãoRelacional
  | (not | !) ExpressãoRelacional

- ExpressãoRelacional ⇒ ExpressãoAritmética
  | ExpressãoAritmética (== | != | > | < | >= | <=) ExpressãoAritmética

- ExpressãoAritmética ⇒ Termo
  | ExpressãoAritmética (+ | -) Termo

- Termo ⇒ Fator
  | Termo (\* | / | %) Fator

- Fator ⇒ identificador
  | identificador ( ListaArgumentos )
  | literal_inteiro
  | literal_floatingPoint
  | literal_string
  | literal_boolean
  | [ ListaArgumentos ]
  | null
  | ( Expressão )
  | (- | not | !) Fator

## Classificação na Hierarquia de Chomsky

A gramática apresentada pelo nosso grupo é **Tipo 2 — (Gramática Livre de Contexto)** pois:

- Todas as produções são da forma A -> α, ou seja, o lado esquerdo é um único não-terminal.
- A linguagem exige estruturas aninhadas (blocos, parênteses, laços, funções) e recursão.

## Exemplos de derivações

#### Declaração de variável simples

`keep integer x as 10;`

#### Expressão aritmética com precedência

Expressão: `a + b * c`

## Análise de dúvidas e Estratégias de Resolução

#### Dangling else

- **Problema**: Ambiguidade sobre a qual `if` um `else` pertence.
- **Solução adotada**: A gramática exige o fechamento explícito de blocos com `{}` e `end`, eliminando a ambiguidade estruturalmente.

#### Precedência e associatividade

- **Solução adotada**: A gramática é estratificada em `ExpressãoAritmética` (soma) e `Termo` (multiplicação), garantindo que a multiplicação tenha maior precedência na árvore de análise.
