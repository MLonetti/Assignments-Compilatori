
# Terzo Assignment 

## Introduzione

In questo terzo Assignment del corso di Compilatori, andremo a creare un passo di ottimizzazione per risolvere il problema della ***Loop Invariant-Code Motion***. 

L'obiettivo sarà quindi individuare le istruzioni *loop invariant* all'interno del programma LLVM alla quale applicheremo il passo, ed in seguito verificare se è possibile effettuarne la *code motion*.

Si definiscono ***Loop Invariant***, le istruzioni che si trovano all'interno di un loop che potrebbero però essere spostate al di fuori in quanto non alterano la semantica del programma
- Migliorando così le performance dato che non eseguiremmo più quelle istruzioni ad ogni iterazione del loop, ma solamente una volta.

Facciamo questa ottimizzazione con lo scopo di individuare quelle istruzioni che originariamente vengono eseguite all'interno di un loop tutte le *n* iterazioni, ma potrebbero essere eseguite una sola volta al di fuori di esso.

La ***Code Motion*** è dunque il processo con la quale individuate tali istruzioni, le spostiamo nel *pre-header* del loop.

---

## Dettagli del passo di ottimizzazione
La realizzazione del passo è avvenuta in due parti distinte:

- La prima parte è stata riservata all'identificazione delle istruzioni ***loop invariant***.

- La seconda parte è stata riservata alla verifica di tali istruzioni Loop Invariant per poter attuare la ***code motion***.

### 1) Individuazione delle istruzioni Loop Invariant

Per identificare un'istruzione come Loop Invariant, nel passo abbiamo dovuto verificare diverse condizioni:
- condizone 1
- condizione 2