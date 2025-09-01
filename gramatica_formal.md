
# Gramática Formal da Linguagem

### Conjunto de Variáveis (V):

V = {Programa, BlocoDeclarações, Declaração, DeclaraçãoVariável,
DeclaraçãoFuncao, DeclaraçãoClasse, DeclaraçãoContrato,
BlocoComandos, Comando, ComandoCondicional, ComandoLaço,
ComandoAtribuição, ComandoEscrita, Expressão, ExpressãoLógica,
ExpressãoRelacional, ExpressãoAritmética, Termo, Fator,
ListaParametros, ListaArgumentos, Tipo}

### Conjunto de Terminais (T):

T = {Keep, as, static, show, if, elsif, then, end,
during, ForEach, to, repeat, until, define, regress,
Class, property, component, integer, floatingPoint,
string, boolean, null, and, or, not, =, ==, !=, >,
<, <=, >=, +, -, \*, /, %, (, ), [, ], {, }, ", ',
;, ., (, ,), \}

### Simbolo Inicial:

S = Programa

## Principais regras de Produção Implementadas

- Programa ⇒ BlocoDeclarações BlocoComandos
- BlocoDeclarações ⇒ Declaração BlocoDeclarações | ε
- Declaração ⇒ Tipo DeclaraçãoVariavel | Tipo DeclaracaoFuncao | DeclaracaoClasse
- DeclaraçãoVariavel ⇒ Keep Tipo identificador as valor | Keep static Tipo Identificador as valor
- Tipo ⇒ integer | floatingPoint | string | boolean | null
- DeclaraçãoFunção ⇒ define Tipo identificador (ListaParametros) { BlocoComandos regress end }
- DeclaraçãoClasse ⇒ Class identificador BlocoClasse end
- DeclaraçãoContrato ⇒ component identificador { BlocoDeclarações BlocoComandos } end

- BlocoComandos ⇒ Comando BlocoComandos | ε

- Comando ⇒ ComandoAtribuição
  | ComandoCondicional
  | ComandoLaço
  | ComandoEscrita
  | regress Expressão

- ComandoCondicional ⇒ if Expressão then BlocoComandos end
  | if Expressão then BlocoComandos elsif Expressão then BlocoComandos end
  | if Expressão then BlocoComandos else BlocoComandos end

- ComandoLaço ⇒ during Expressão { BlocoComandos } end
  | ForEach identificador to Expressão { BlocoComandos } end
  | repeat { BlocoComandos } until Expressão end

- ComandoAtribuição ⇒ identificador = Expressão ;

- ComandoEscrita ⇒ show ListaArgumentos ;

- ListaParametros ⇒ Tipo identificador , ListaParametros
  | Tipo identificador
  | ε

- ListaArgumentos ⇒ Expressão , ListaArgumentos
  | Expressão
  | ε

- Expressão ⇒ ExpressãoLógica

- ExpressãoLógica ⇒ ExpressãoRelacional
  | ExpressãoLógica and ExpressãoRelacional
  | ExpressãoLógica or ExpressãoRelacional
  | not ExpressãoRelacional

- ExpressãoRelacional -> ExpressãoAritmética
  | ExpressãoAritmética == ExpressãoAritmética
  | ExpressãoAritmética != ExpressãoAritmética
  | ExpressãoAritmética > ExpressãoAritmética
  | ExpressãoAritmética < ExpressãoAritmética
  | ExpressãoAritmética >= ExpressãoAritmética
  | ExpressãoAritmética <= ExpressãoAritmética

- ExpressãoAritmética -> Termo
  | ExpressãoAritmética + Termo
  | ExpressãoAritmética - Termo

- Termo -> Fator
  | Termo \* Fator
  | Termo / Fator
  | Termo % Fator

- Fator -> identificador
  | literal_inteiro
  | literal_floatingPoint
  | literal_string
  | literal_boolean
  | null
  | ( Expressão )

  ## Classificação na Hierarquia de Chomsky

A gramática apresentada pelo nosso grupo é Tipo 2 — (Gramática Livre de Contexto) pois:

* Todas as produções são da forma A -> α, ou seja, o lado esquerdo é um único não-terminal e o lado direito é uma sequência finita de terminais e não-terminais.

* A linguagem exige estruturas aninhadas (blocos, parênteses, laços, funções, classes) e recursão — características típicas de CFGs que não podem ser expressas por gramáticas regulares (Tipo 3).

Implicação prática: um parser baseado em técnicas para CFG (LL, LR, LALR, etc.) é apropriado. Entretanto, dependendo do estilo de gramática (recursão à esquerda, ambiguidade), pode-se preferir LR/LALR/GLR sobre LL(1) ou então transformar a gramática (eliminar recursão à esquerda, fatorar) para compatibilidade com um parser top-down.

## Exemplos de derivações para construtos importantes

#### Programa exemplo para a declaração de variável simples

Keep integer x as 10 ; show x ;

Derivação (passo a passo):
```
Programa
⇒ BlocoDeclarações BlocoComandos
⇒ Declaração BlocoDeclarações BlocoComandos
⇒ DeclaraçãoVariavel BlocoDeclarações BlocoComandos
⇒ Keep Tipo Identificador as Literal ; BlocoDeclarações BlocoComandos
⇒ Keep integer Identificador as literal_inteiro ; BlocoDeclarações BlocoComandos
⇒ Keep integer x as 10 ; BlocoDeclarações BlocoComandos
⇒ Keep integer x as 10 ; ε BlocoComandos
⇒ Keep integer x as 10 ; Comando BlocoComandos
⇒ Keep integer x as 10 ; ComandoEscrita BlocoComandos
⇒ Keep integer x as 10 ; show ListaArgumentos ; BlocoComandos
⇒ Keep integer x as 10 ; show Expressão ; BlocoComandos
⇒ Keep integer x as 10 ; show Identificador ; BlocoComandos
⇒ Keep integer x as 10 ; show x ; ε
```

#### Expressão aritmética com precedência

Expressão: `a + b * c`
Derivação de ExpressãoAritmética mostrando que * tem maior precedênncia:
```
ExpressãoAritmética
⇒ ExpressãoAritmética + Termo
⇒ Termo + Termo
⇒ Fator + Termo
⇒ Identificador + Termo
⇒ Identificador + Termo * Fator
⇒ Identificador + Fator * Fator
⇒ Identificador + Identificador * Identificador
⇒ a + b * c
```

#### Condicional aninhado
Exemplo de Programa:
```
if E1 then
    if E2 then
        S1
    else
        S2
end
```
Derivação: o else é associado ao if interno (mais próximo) porque IfWithElse consome else dentro da regra; nossa gramática garante essa associação por construção de IfWithElse e IfNoElse.

Exemplo de derivação concreta:
```
ComandoCondicional
⇒ IfWithElse
⇒ if Expressão then BlocoComandos else BlocoComandos end
⇒ if E1 then BlocoComandos else BlocoComandos end
⇒ if E1 then (ComandoCondicional) else BlocoComandos end
⇒ if E1 then (IfWithElse) else BlocoComandos end
... (continua até produzir S1 e S2) ...
```
A separação previne a ambiguidade: não há derivação alternativa onde o else fique associado ao if externo.

## Análise de dúvidas e Estratégias de Resolução
#### Dangling else

* __Problema__: em gramáticas simples, `if A then if B then S1 else S2` pode ter duas árvores de derivação (else ligado ao primeiro ou segundo if).

* __Solução adotada__: separar `IfNoElse` e `IfWithElse`. Isso força o `else` a pertencer ao `IfWithElse` mais interno — padrão comum em linguagens (como C, Java). Alternativas: usar regras semânticas/precedência no parser ou exigir `end` explícito para cada `if`/`else` (mas isso muda a sintaxe desejada).

#### Precedência e associatividade de operadores

* __Problema__: se usamos uma única produção vaga para Expressão, diferentes árvores podem representar `a - b - c` (associação à esquerda ou direita).

* __Solução adotada__: distinguir `ExpressãoAritmética`, `Termo` e `Fator` com recursão à esquerda para `+`/`-` e `*`/`/`/`%`. Isso garante associatividade à esquerda (com a gramática como escrita). Se for necessário um parser LL(1), recomenda-se eliminar a recursão à esquerda e fatorar a gramática.

#### Equívocos em elsif encadeados

* __Problema__: encadeamento de `elsif` pode gerar árvores distintas se a gramática não tratar `elsif` recursivamente de forma consistente.

* __Solução adotada__: nas regras de `IfNoElse` e `IfWithElse` tratei `elsif` recursivamente, de modo que `if ... elsif ... elsif ... else ... end` tem única interpretação: cada `elsif` liga-se ao `if` externo até o `else` final.

#### Outras potenciais ambiguidades

* __Declarações vs Expressões__: garantir que Keep inicia uma declaração e não seja confundido com expressão começando com Identificador ajuda o parser (lexicalização clara ajuda).

* __Pontos e vírgulas__: padronizei que comandos de efeito de linha terminam com ;. Blocos (entre {}) não exigem ; ao final de bloco, mas comandos internos sim. Isso reduz ambiguidade de terminação.