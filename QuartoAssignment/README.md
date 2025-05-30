# Assignment 4 – Ottimizzazione: Loop Fusion

## Obiettivo

In questo quarto e ultimo assignment del corso di *Compilatori*, si implementa un passo di ottimizzazione volto a migliorare l’efficienza degli accessi in memoria mediante la tecnica di **Loop Fusion**.

L’ottimizzazione ha come finalità il riutilizzo efficace dei dati in cache, riducendo il numero di *cache misses* e migliorando il **CPI (Cycles Per Instruction)** del programma.

---

## Contesto e Motivazione

Si consideri il seguente frammento di codice C:

```c
for (int i = 0; i < N; i++) {
    A[i] = i;
}

for (int i = 0; i < N; i++) {
    B[i] = A[i];
}
```

In questo esempio, il valore `A[i]` viene prodotto nel primo ciclo e consumato nel secondo. Tuttavia, tra i due loop può trascorrere abbastanza tempo da causare l’espulsione del dato dalla cache, comportando rallentamenti dovuti all’accesso alla memoria principale.

Applicando la trasformazione di *loop fusion*, i due cicli vengono unificati:

```c
for (int i = 0; i < N; i++) {
    A[i] = i;
    B[i] = A[i];
}
```

Con questa fusione, il dato `A[i]` è riutilizzato immediatamente, migliorando l’efficienza della cache.

---

## Condizioni per l’Applicazione della Loop Fusion

Affinché la trasformazione sia semanticamente corretta ed efficace, devono essere soddisfatte le seguenti condizioni. Le analisi vanno condotte su coppie di loop appartenenti allo stesso livello di annidamento.

### 1. Adiacenza

I due loop devono essere **adiacenti**: il basic block di uscita del primo loop deve coincidere con il **pre-header** del secondo. Non devono esistere basic block intermedi tra i due.

È inoltre necessario gestire correttamente varianti strutturali dei loop, come i loop con condizioni di guardia (*guarded loops*).

### 2. Equivalenza del Controllo di Flusso

Occorre garantire che:

- Se viene eseguito il primo loop (`L1`), allora verrà eseguito anche il secondo (`L2`);
- Se viene eseguito il secondo loop (`L2`), allora è stato eseguito anche il primo (`L1`).

Questa proprietà si verifica tramite analisi di **dominanza** e **post-dominanza** sui basic block di ingresso dei due loop.

### 3. Uguale Numero di Iterazioni (Trip Count)

È necessario accertarsi che i due loop abbiano lo stesso numero di iterazioni.

Questa informazione è ricavabile tramite l’analisi **Scalar Evolution (SCEV)**, che descrive l’evoluzione delle variabili di induzione e consente di confrontare i *trip count*.

### 4. Assenza di Dipendenze Negative

La presenza di **dipendenze negative** impedisce l’applicazione sicura della *loop fusion*.

Un esempio di dipendenza negativa è il seguente:

```c
for (int i = 0; i < N; i++) {
    A[i] = i;
}

for (int i = 0; i < N; i++) {
    B[i] = A[i + 1];
}
```

In questo caso, al tempo dell’accesso a `A[i + 1]` il valore potrebbe non essere stato ancora prodotto. Ciò viola la correttezza semantica e rende impossibile la fusione.

---

## Fusione dei loops

Verificate tutte le condizioni descritte, è possibile procedere alla trasformazione:

- Si integra il corpo del secondo loop all’interno del primo;
- Si modifica la struttura del CFG affinché il basic block di uscita del secondo loop venga rimosso o riutilizzato come uscita comune;
- Si mantiene inalterata la semantica del programma.

La trasformazione è implementata nella funzione `loopFusion()`, richiamata in `visitaLoops()`, dove vengono selezionate le coppie di loop candidabili alla fusione e sottoposte all’analisi.

---

## Prerequisiti software

Per eseguire il passo di ottimizzazione, sono necessari i seguenti strumenti:
- **LLVM-19**
- **cmake**
- **opt-19**

## Creazione dell'ambiente di test

Una volta clonato il repository, ed entrati all'interno di esso, avremo la seguente struttura

    ProgramRootFolder
    |_ test #Conterrà il codice LLVM da ottimizzare
    |_ CMakeList.txt #Contiene le istruzioni di building del passo
    \_ Pass.cpp #L'effettivo passo che servirà poi per l'ottimizzazione del codice LLVM

Dobbiamo ora creare all'interno una directory Build/
```bash
  mkdir build/
```
Ora la nostra struttura sarà la seguente:

    ProgramRootFolder
    |
    |_ build #Verrà usata per compilare il passo d'ottimizzazione
    |_ test #Conterrà i sorgenti LLVM da ottimizzare
    |_ CMakeList.txt #Contiene le istruzioni di building del passo
    \_ Pass.cpp #File sorgente che contiene le implementazioni dei 3 passi d'ottimizzazione
---

## Applicazione dei passi di ottimizzazione
### 1. **Building del passo LLVM**

Spostiamoci all'interno della directory build/ appena creata:

```bash
cd build/
```
 Lanciamo all'interno della directory il seguente comando
    
```bash
cmake -DLT_LLVM_INSTALL_DIR=/path/install/llvm-19 ../
```
Stiamo specificando:
- Percorso di LLVM (consigliabile utilizzare una variabile d'ambiente)
- Percorso del CMakeLists.txt e del file sorgente del passo (li abbiamo nella directory sopra di noi)

Con questo comando andiamo a generare i file necessari per la compilazione.

lanciando il comando ```make```, andiamo a compilare il file sorgente in cui è incluso il passo di ottimizzazione Loop Fusion (Pass.cpp).

Verrà quindi generato un file .so (shared object), che verrà applicato ai file LLVM da ottimizzare.

---
    
### 2. **Esecuzione dei passi di ottimizzazione:**

Per eseguire il nostro passo di ottimizzazione utilizzeremo lo strumento opt, alla quale passeremo il nostro file in formato shared object, precedentemente creato.

Il comando che utilizzeremo è il seguente:

```bash
opt -load-pass-plugin=build/libLoopFusionPass0.so -passes=loop-fusion-pass test/test_many_loops.ll -So Optimized.ll
```
- **`-load-pass-plugin`**: Carichiamo il file `.so` contenente i passi di ottimizzazione.
- **`-passes`**: Specifica il passo di ottimizzazione mediante flag di attivazione
- **`-S`**: Genera un file LLVM leggibile.
- **`-o`**: Specifica il file di output ottimizzato.

### 3. **Analisi dell'IR ottimizzato:**

Si può analizzare il risultato dell'ottimizzazione all'interno del file che verrà generato: "Optimized.ll"

---

## Conclusioni

L’ottimizzazione mediante **loop fusion** rappresenta un efficace intervento per migliorare la località spaziale e temporale dei dati nei programmi. Applicarla correttamente, nel rispetto delle condizioni strutturali e semantiche dei loop, consente di ottenere un'esecuzione più efficiente e performante.
