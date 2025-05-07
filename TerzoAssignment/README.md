
# Terzo Assignment – Ottimizzazione Loop Invariant Code Motion

## Introduzione

In questo terzo assignment del corso di Compilatori, si implementa un passo di ottimizzazione volto a risolvere il problema della **Loop Invariant Code Motion (LICM)**.

L’obiettivo è individuare le istruzioni *loop invariant* all’interno di un programma LLVM e verificarne la possibilità di spostamento (code motion) al di fuori del ciclo, al fine di migliorarne le prestazioni.

Una *loop-invariant instruction* è un’istruzione situata all’interno di un ciclo che non dipende da valori modificati nel ciclo stesso e che può quindi essere eseguita una sola volta prima dell’inizio del loop, senza alterare la semantica del programma.  
L’ottimizzazione consente di evitare l’esecuzione ripetuta di queste istruzioni ad ogni iterazione, migliorando così l’efficienza del codice.

## Dettagli del Passo di Ottimizzazione

L’implementazione dell’ottimizzazione è articolata in due fasi principali:

1. Individuazione delle istruzioni *loop invariant*  
2. Applicazione della *code motion*

### 1. Individuazione delle Istruzioni Loop Invariant

Un’istruzione è considerata *loop invariant* solo se soddisfa alcuni requisiti precisi.

#### Requisito principale
L’istruzione deve essere di tipo `BinaryOperator`. Se non lo è, può essere esclusa direttamente.

#### Altri criteri
Un’istruzione `BinaryOperator` è considerata *loop invariant* se entrambi i suoi operandi sono:
- Costanti;
- Oppure definiti al di fuori del loop;
- Oppure essi stessi *loop invariant*.

Le istruzioni che rispettano questi criteri vengono memorizzate in un vettore dedicato per un successivo processamento.

### 2. Code Motion

Dopo aver individuato le istruzioni *loop invariant*, è necessario determinare se possono essere effettivamente spostate al di fuori del ciclo, nel cosiddetto *pre-header* del loop.

#### Condizioni per la Code Motion

Un’istruzione può essere spostata nel *pre-header* se soddisfa almeno una delle seguenti condizioni:
- Domina tutte le uscite del loop;
- Oppure, se non domina tutte le uscite, è *dead* nei basic block successivi all’uscita che non domina (cioè non è più utilizzata in quei blocchi).

#### Ordinamento delle istruzioni

Lo spostamento delle istruzioni deve rispettare l’ordine corretto di dipendenza per preservare la semantica del programma.  
Tale ordinamento è garantito dalla funzione ricorsiva:

```cpp
void moveInst();
```
## Prerequisiti software

Per eseguire il passo di ottimizzazione, sono necessari i seguenti strumenti:
- **LLVM-19**
- **cmake**
- **opt-19**

## Creazione dell'ambiente di test

Una volta clonato il repository, ed entrati all'interno di esso, avremo la seguente struttura

    ProgramRootFolder
    |_ Test #Conterrà il codice LLVM da ottimizzare
    |_ CMakeList.txt #Contiene le istruzioni di building del passo
    \_ Pass.cpp #L'effettivo passo che servirà poi per l'ottimizzazione del codice LLVM

Dobbiamo ora creare all'interno una directory Build/
```bash
  mkdir Build/
```
Ora la nostra struttura sarà la seguente:

    ProgramRootFolder
    |
    |_ Build #Verrà usata per compilare il passo d'ottimizzazione
    |_ Test #Conterrà i sorgenti LLVM da ottimizzare
    |_ CMakeList.txt #Contiene le istruzioni di building del passo
    \_ Pass.cpp #File sorgente che contiene le implementazioni dei 3 passi d'ottimizzazione
---

## Applicazione dei passi di ottimizzazione
### 1. **Building del passo LLVM**

Spostiamoci all'interno della directory Build/ appena creata:

```bash
cd Build/
```
 Lanciamo all'interno della directory il seguente comando
    
```bash
cmake -DLT_LLVM_INSTALL_DIR=/path/install/llvm-19 ../
```
Stiamo specificando:
- Percorso di LLVM (consigliabile utilizzare una variabile d'ambiente)
- Percorso del CMakeLists.txt e del file sorgente del passo (li abbiamo nella directory sopra di noi)

Con questo comando andiamo a generare i file necessari per la compilazione.

lanciando il comando ```make```, andiamo a compilare il file sorgente in cui è incluso il passo di ottimizzazione LICM (Pass.cpp).

Verrà quindi generato un file .so (shared object), che verrà applicato ai file LLVM da ottimizzare.

---
    
### 2. **Esecuzione dei passi di ottimizzazione:**

Per eseguire il nostro passo di ottimizzazione utilizzeremo lo strumento opt, alla quale passeremo il nostro file in formato shared object, precedentemente creato.

Il comando che utilizzeremo è il seguente:

```bash
opt -load-pass-plugin=Build/libLICMPass.so -passes=LICM-Pass Test/Loop.m2r. ll -So Optimized.ll
```
- **`-load-pass-plugin`**: Carichiamo il file `.so` contenente i passi di ottimizzazione.
- **`-passes`**: Specifica il passo di ottimizzazione mediante flag di attivazione
- **`-S`**: Genera un file LLVM leggibile.
- **`-o`**: Specifica il file di output ottimizzato.

### 3. **Analisi dell'IR ottimizzato:**

Si può analizzare il risultato dell'ottimizzazione all'interno del file che verrà generato: "Optimized.ll"

---

## Conclusioni

Con questo passo di ottimizzazione è stata implementata una tecnica fondamentale nel contesto dei compilatori:  
**la riduzione delle istruzioni eseguite all’interno di un ciclo senza alterarne il comportamento semantico**.

Il lavoro svolto ha permesso di:

- **Identificare correttamente le istruzioni loop-invariant**, ovvero quelle istruzioni che non dipendono da valori modificati all'interno del ciclo;
- **Verificare formalmente la possibilità di spostamento** fuori dal loop nel rispetto della correttezza semantica;
- **Applicare la code motion in modo sicuro**, spostando le istruzioni nel *pre-header* del ciclo.
