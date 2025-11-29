# Especificações Léxicas

- **Identificadores**

```
[_a-zA-Z][_a-zA-Z0-9]*
```

- **String literal**

```
"([^"\n])*" | '([^'\n])*'
```

- **Inteiros**

```
\d+
```

- **Decimais**

```
\d+\.\d+
```

- **Palavras reservadas**

```
keep, static, as, define, regress, return, show,
if, elsif, else, then, end,
during, foreach, in, repeat, until,
integer, floatingPoint, string, boolean, null, void
and, or, not
```

- **Operadores e símbolos**

```
==|<=|>=|!=|&&|\|\||[+\-*/=<>!{}()\[\];,\.]
```

- **Booleanos**

```
True | False
```

- **Comentários**

```
 //.* (linha)
/\*[\s\S]*?\*/ (bloco)
```

---

# Mensagens de Erro

## 1. Erros de Sintaxe

- **Faltou ponto e vírgula**

```
Erro: Esperava ';' no final do comando.
```

- **Parênteses ou chaves não fechados**

```
Erro: Parêntese '(' aberto, mas não foi fechado.
Erro: Esperava '}' closing function.
```

- **Palavra reservada mal escrita**

```
Erro: Unexpected token in factor: 'regress'.
```

---

## 2. Erros de Tipos

- **Atribuição incorreta**

```
Erro: Não é possível atribuir um valor do tipo 'string' a uma variável do tipo 'integer'.
```

- **Retorno Inválido**

```
Erro: Tipo de retorno inválido. A função espera 'integer' mas retornou 'boolean'.
```

- **Comparação inválida**

```
Erro [linha 8]: Não é possível comparar um número com um texto.
Dica: Converta os valores para o mesmo tipo antes de comparar.
```

---

## 3. Erros de Funções

- **Número errado de argumentos**

```
Erro [linha 10]: A função 'soma' espera 2 argumentos, mas você forneceu 3.
```

- **Função não definida**

```
Erro [linha 6]: A função 'multiplica' não foi encontrada.
Dica: Verifique se você a declarou antes de usá-la.
```

---

## 4. Erros de Variáveis

- **Variável não declarada**

```
Erro: Identificador 'x' não declarado.
```

- **Variável já declarada**

```
Erro: Variável 'x' já declarada neste escopo.
```

---

## 5. Erros de Execução

- **Divisão por zero**

```
Erro em tempo de execução [linha 12]: Tentativa de dividir por zero.
Dica: Verifique o valor do divisor antes da operação.
```

---

## Estilo das Mensagens

- Sempre indicar **linha e tipo de erro**.
- Usar uma linguagem **amigável e didática**.
- Fornecer **dicas de correção** sempre que possível.
