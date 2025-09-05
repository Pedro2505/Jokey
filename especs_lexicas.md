## Especificações léxicas

- Identificadores:

  ```
  [_a-zA-Z][_a-zA-Z0-9]*
  ```

- String literal:

  ```
  "([^"\n])*" | '([^'\n])*'
  ```

- Inteiros:

  ```
  \d+
  ```

- Decimais:

  ```
  \d+\.\d+
  ```

- Palavras reservadas:

```
Keep|as|static|show|if|elsif|then|end|
during|ForEach|to|repeat|until|define|regress|
Class|property|component|
integer|floatingPoint|string|boolean|null|
and|or|not
```

- Operadores e símbolos:

```
    ==|<=|>=|!=|&&|\|\||[+\-*/=<>!{}()\[\];,\.]
```

- Boleanos:

```
    True | False
```

- Comentários:

```
    //.* | /\*[\s\S]*?\*/
```
