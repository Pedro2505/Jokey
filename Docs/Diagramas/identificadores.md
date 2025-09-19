# Identificadores

```mermaid
stateDiagram-v2
    [*] --> S0
    S0 --> S1: letra
    S1 --> S1: letra ou dígito
    S1 --> [*]: aceitação
```
