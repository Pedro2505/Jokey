# Alfabeto da Linguagem Jokey

- Letras
  Σ(letras) = {A, B, C, D, ..., Z, a, b, c, d, ..., z}
- Números decimais
  Σ(decimais) = {0, 1, 2, 3, 4, 5, 6 ,7 ,8 ,9}
- Caracteres especiais
  Σ(caracteres) = {+, -, \*, /, '.', =, ';', <, >, ", ', (, ), [, ], {, }, :, ;, &, |, %, !, ',' }
- Espaços
  Σ(espacos) = {espaço, tab, enter, \n}

Então, o conjunto final seria:

- Σ = Σ(letras​) u Σ(digitos​) u Σ(simbolos​) u Σ(espac​os​)

## Definição dos tipos de tokens

Identificadores / Linguagem:

```
L = (Σ(letras) u {_})(Σ(letras) u Σ(decimais) u {_})∗
```

String Literal:

```
L = ({", '} U Σ(letras) U Σ(caracteres) U Σ(decimais) U Σ(espacos) U {',"})∗
```

Valores numericos

```
Inteiros: Σ(decimais)+
```

```
Decimais: Σ(decimais)+ u {.} u Σ(decimais)+
```

Palavras reservadas

```
keep, static, as, define, regress, return, show,
if, elsif, else, then, end,
during, foreach, in, repeat, until,
integer, floatingPoint, string, boolean, null, void
and, or, not
```

Operadores e símbolos

```
+, -, *, /, %, =, ==, <, >, <=, >=, !=, &&, ||, !,
{, }, (, ), [, ], ;, , .
```

Boleanos

```
True, False
```

Comentários

```
Linha: // Σ*
Bloco: /* Σ* */
```

## Exemplo de Programa válido

```
// Declaração de função principal
define integer principal() {
    // Declaração de variáveis
    keep integer numero as 5;
    keep integer index as 0;

    // Estrutura de repetição (While)
    during (index < numero) {
        show(index);
        index = index + 1;
    } end

    // Retorno da função
    regress 0 end
}

principal();
```
