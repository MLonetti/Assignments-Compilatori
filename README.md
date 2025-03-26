# Compilatori Assignement
## 1. Primo Esercizio
### - Identità Algebrica
Consiste nel riconoscere operazioni ridondanti che mostrano elemento neutro.

```python
#Esempio
x = y + 0 -> y
c = z * 1 -> z
```
Una volta riconosciuta l'operazione è necessario:
- Rimpiazzare gli usi di queste istruzioni, direttamente con l'operando non neutro
- Eliminare queste istruzioni

Per fare ciò necessitiamo del metodo ```ReplaceAllUsesWith(Instruction)```.
Come possiamo notare, necessita di un ```Istruction``` come parametro, mentre nel contesto in esame è necessario operare con un ```Value``` (Classe di cui gli operandi sono <b>Oggetti</b>)<br>
Visto che ```Istruction``` è una classe che eredita da ```Value``` possiamo comunque utilizzare questo metodo con un oggetto della classe ```Istruction```.<br>
Pertanto è necessario sostitire tutte le istanze dell'istruzione ridondante, per poi eliminare quanto rimosso.

Per fare ciò 
### - Strength Reduction
### - Ottimizzazione multi-istruzione

# Note ed Osservazioni 
- Per lo svolgimento di un passo è possibile operare anche su una sola directory, lavorando sui passi singolarmente, in quanto ogni passo è una scruct isolata, per poi richiamare i metodi ```runOnFunction``` e ```runOnBasicBlock``` direttamente dentro alla struct del singolo passo.
- Per applicare le singole ottimizzazioni andiamo a manipolare la parte in basso del codice, dove abbiamo ```if (Name == Algebrai-identity)``` che chiama la struct ```AlgIde```, per poi fare un ```else if``` in cui andiamo a vedere se ```Name```, ovvero il flag che diamo in ingresso ad opt, è uguale ad una delle tre keyword associate alle tre struct che abbiamo nel sorgente. Nel caso in cui la condizione fosse vera, si chiama la struct associata, e quindi viene eseguito il passo chiamato. Pertanto possiamo quindi creare un file .ll in cui possimao fare le tre ottimizzazioni, così che quando chiamiamo opt applichiamo i flag uno per volta, e andiamo ad ottimizzare tutto il sorgente LLVM con le tre ottimizzazioni.