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