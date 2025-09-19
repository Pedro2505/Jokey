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
Keep|as|static|show|if|elsif|then|end|
during|ForEach|to|repeat|until|define|regress|
Class|property|component|
integer|floatingPoint|string|boolean|null|
and|or|not
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
 //.* | /\*[\s\S]*?\*/
```

---

# Mensagens de Erro

## 1. Erros de Sintaxe

- **Faltou ponto e vírgula**  
```
Erro [linha 4]: Esperava ';' no final do comando.
Dica: Cada instrução deve terminar com ';'.
```

- **Parênteses ou chaves não fechados**  
```
Erro [linha 7]: Parêntese '(' aberto, mas não foi fechado.
Dica: Verifique se todos os parênteses/chaves foram fechados corretamente.
```

- **Palavra reservada mal escrita**  
```
Erro [linha 2]: Palavra 'retun' não reconhecida.
Dica: Você quis dizer 'return'?
```

---

## 2. Erros de Tipos

- **Atribuição incorreta**  
```
Erro [linha 5]: Não é possível atribuir um texto a uma variável do tipo inteiro.
Código: int idade = "vinte";
Dica: Use números sem aspas para inteiros, exemplo: 20
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
Erro [linha 3]: A variável 'x' não foi declarada antes do uso.
Dica: Declare usando 'int x = ...;' ou outro tipo.
```

- **Variável já declarada**  
```
Erro [linha 5]: A variável 'resultado' já foi declarada neste escopo.
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
