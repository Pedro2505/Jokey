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



Além disso, temos **tokens formados dinamicamente**:
- **identificador**
- **literal_inteiro**
- **literal_floatingPoint**
- **literal_string**
- **literal_boolean**

---

## 🔹 2. Possíveis Ambiguidades e Regras de Resolução

| Categoria                          | Exemplo                           | Ambiguidade                                                                 | Regra de Resolução                                                                 |
|-----------------------------------|-----------------------------------|-----------------------------------------------------------------------------|------------------------------------------------------------------------------------|
| Identificadores vs. Palavras-chave | `if`, `end`, `Class`, `abc`       | Palavras-chave também obedecem ao padrão de identificadores                  | Palavras-chave têm **precedência** sobre identificadores                           |
| Literais Numéricos                 | `123`, `45.67`                    | `123` pode ser inteiro ou início de float                                    | Se houver ponto decimal → `floatingPoint`, caso contrário → `integer`              |
| Operadores Relacionais e Atribuição| `=`, `==`, `<=`, `>=`             | `==` pode ser reconhecido como `=` seguido de `=`                            | Usar **longest match rule** (máximo consumo)                                       |
| Strings e Caracteres               | `"texto"`, `'a'`                  | Aspas simples podem gerar dúvida: string curta ou token isolado              | `"..."` é string, `'...'` pode ser string curta ou rejeitada (decisão da linguagem) |
| Booleanos e Null                   | `true`, `false`, `null`           | Podem ser confundidos com identificadores                                    | Reservados têm **precedência** sobre identificadores                               |
| Espaços e Comentários              | espaço, tab, `// comentário`      | Espaços podem ser confundidos com separador ou erro                          | Ignorar **whitespace** e comentários no léxico                                     |

---

## 🔹 3. Estratégias de Tratamento de Erros Léxicos

- **Caractere inválido**:  
  `Erro léxico na linha X: caractere inesperado '@'`  
  → Ignorar caractere e seguir.

- **Literal não fechado**:  
  `Erro léxico na linha X: string não terminada`  
  → Descartar até próximo `"`.  

- **Número mal formado**:  
  `Erro léxico na linha X: número mal formado (ex.: 12.3.4)`  
  → Descartar token e seguir.  

---

## 🔹 4. Expressões Regulares (Resumo)

| Token               | Expressão Regular                    |
|---------------------|--------------------------------------|
| identificador       | `[a-zA-Z_][a-zA-Z0-9_]*`             |
| literal_inteiro     | `[0-9]+`                             |
| literal_floatingPoint | `[0-9]+\.[0-9]+`                   |
| literal_string      | `"([^"\\]|\\.)*"`                   |
| literal_boolean     | `true\|false`                        |
| operadores          | `==\|!=\|<=\|>=\|=\|<\|>\|%\|\+\|-\|\*\|/` |
| delimitadores       | `\(|\)|\[|\]|\{|\}|;|,|\.`           |
| palavras-chave      | `Keep\|as\|static\|show\|if\|elsif\|then\|end\|during\|ForEach\|to\|repeat\|until\|define\|regress\|Class\|property\|component\|integer\|floatingPoint\|string\|boolean\|null\|and\|or\|not` |

---

## 🔹 5. Conclusão
- A principal ambiguidade é **identificador vs. palavra-chave**, resolvida por precedência.  
- Outras ambiguidades (números, operadores compostos, strings) são resolvidas com **longest match** e regras contextuais.  
- O tratamento de erros léxicos é essencial para fornecer **feedback útil** ao programador.  


# Tipos de erros léxicos comuns
### A principio analisamos e classificamos possiveis erros: 

| Tipo de erro                 | Exemplo       | Descrição  |
| -------------                |:-------------:|:-----------:
| Caractere inválido           |   @, ?   | Símbolo que não pertence a Σ (alfabeto da linguagem). |
| Identificador mal formado    |   1var   | Identificadores devem começar com letra ou _.
| Número mal formado           | 12.34.56 | Mais de um ponto em um número decimal.
| String não terminada         |  "texto  | Falta de fechamento do delimitador ' ou ".
| Comentário não fechado       | /* comentário | Comentário de bloco não finalizado com */.
| Operador inválido/incompleto |     &    | Esperado &&, mas veio apenas um &.
| Sequência inesperada         |    !=!   | Sequência que não corresponde a nenhum token válido.

### Estrategias de tratamento
O tratamento desses erros léxicos pode ser feito em três etapas principais: detecção, relato e recuperação.

##### Detecção:
* A detecção ocorre durante a leitura de caracteres pelo analisador léxico.
* Sempre que uma sequência não corresponder a nenhum padrão de token, isso é identificado como erro.
Exemplo:
```
Entrada: 1var
Durante a leitura, o primeiro caractere é 1.
Como identificadores devem começar com letra ou _, ocorre erro imediatamente.
```
##### Relato:
* Quando um erro é detectado, gere uma mensagem clara para o usuário, informando:
    * Tipo do erro

    * Caractere ou sequência que causou o erro

    * Linha e coluna onde ocorreu

Exemplo de mensagem:
```
[Erro Léxico] Linha 4, Coluna 10: Identificador inválido '1var'. Identificadores devem começar com letra ou '_'.
```
##### Recuperação:
Depois de detectar o erro, precisaremos decidir como continuar analisando o código. Existem algumas estratégias clássicas:

| Estratégia           | Como Funciona       | Quando usar  |
| -------------|:-------------:|:-----------:
| Pânico (Panic Mode)|Descarta caracteres até encontrar um delimitador seguro, como ; ou { | Boa para erros graves, como strings não fechadas |
| Ignorar caractere inválido   |   Remove apenas o caractere que causou o erro e segue o fluxo   | Ideal para caracteres isolados como @ ou ?|
| Substituição | Substitui automaticamente por algo válido | Evitar, pois pode esconder erros reais.

No nosso projeto usaremos somente as duas primeiras estratégias.

### Como aplicar para cada caso
##### Caractere inválido
* Se for encontrado um caractere que não está em Σ, o analisador gera erro e ignora esse caractere.
```
Código: a = 5 @ 3
Saída: [Erro Léxico] Linha 1, Coluna 7: Caractere inválido '@'
```

##### Identificador mal formado
* Identificadores devem seguir:
```
(letra | _)(letra | dígito | _)*
```
* Se começar com número, gera erro, mas consideramos tudo depois do número como parte de um identificador válido.

Exemplo:
```
Código: 1var = 10
Saída: [Erro Léxico] Linha 1, Coluna 1: Identificador não pode começar com número.
```

##### Número mal formado
* Se o número decimal tiver mais de um ponto (.), é inválido.

Exemplo:
```
Código: valor = 12.34.56
Saída: [Erro Léxico] Linha 1, Coluna 11: Número decimal mal formado '12.34.56'.
```
##### String não terminada
* Se ao ler uma string não for encontrado o fechamento até o fim da linha ou do arquivo, o analisador gera erro e descarta tudo até encontrar uma aspa.

Exemplo:
```
Código: print("Texto sem fechar)
Saída: [Erro Léxico] Linha 1, Coluna 7: String não terminada.
```
##### Comentário de bloco não fechado
* Se encontrar /* mas nunca encontrar */, o analisador deve:
    * Gerar erro.
    * Ignorar tudo até o fim do arquivo.

Exemplo:
```
Código:
main() {
    /* Comentário sem fechar
    int x = 10;
}

Saída: [Erro Léxico] Linha 2: Comentário de bloco não fechado.
```
### Fluxo geral no analisador
Pseudocódigo simplificado:
```
enquanto (não fim do arquivo):
    char = próximo_caractere()

    se char pertence a Σ:
        tentar formar token
        se token válido:
            adicionar à lista de tokens
        senão:
            gerar erro
            aplicar recuperação
    senão:
        gerar erro "Caractere inválido"
        ignorar caractere
```
### Exemplo prático com Jokey
Código de entrada:
```
main() {
    int numero = input("Digite um valor: )
    print(numero @ 3)
}
```
Saída esperada:
```
[Erro Léxico] Linha 2, Coluna 32: String não terminada.
[Erro Léxico] Linha 3, Coluna 15: Caractere inválido '@'.
```