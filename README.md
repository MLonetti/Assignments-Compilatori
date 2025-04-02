# Introduzione

In questo primo assignment, abbiamo sviluppato tre passi di ottimizzazione. Ognuno di questi, è progettato per risolvere specifiche problematiche presenti nelle rappresentazioni intermedie LLVM. Le IR verranno quindi analizzate e trasformate in versioni più efficienti.

---

## Dettagli dei passi di ottimizzazione

### 1. **Algebraic Identity**

Questo passo elimina operazioni ridondanti o inutili basate su identità algebriche. Ad esempio:
- **Moltiplicazioni per 1**: `x * 1 → x`
- **Divisioni per 1**:  `x : 1 → x`
- **Addizioni con 0**: `x + 0 → x`
- **Sottrazioni di 0**: `x - 0 → x`

#### Esempio:
Input:
```llvm
%res = mul i32 %x, 1
```

Output:
```llvm
%res = %x
```
Il nostro passo, oltre che eliminare le istruzioni di questo tipo, ne rimpiazza anche gli usi, ovvero le istruzioni che utilizzavano %res, una volta che %res viene eliminata, utilizzeranno direttamente %x

---

### 2. **Strength Reduction**

Questo passo sostituisce operazioni costose con equivalenti più efficienti. Ad esempio:
- **Moltiplicazioni per potenze di 2**: `x * 2^n → x << n`
- **Divisioni per potenze di 2**: `x / 2^n → x >> n`

Ovviamente in termini di Instruction Count il costo rimane invariato, ovvero il numero di istruzioni è identico.
- Abbiamo però un'ottimizzazione in termini di cicli per istruzione, in quanto le shift sono molto meno costose delle Mul o delle Div.

#### Esempio:
Input:
```llvm
%res = mul i32 %x, 16
```

Output:
```llvm
%res = shl i32 %x, 4
```
In questo passo andiamo ad ottimizzare anche le moltiplicazioni in cui l'operando costante ha un valore di un'unità maggiore o minore della potenza di 2

#### Esempio:
- `res = x * 15` diventa:
- `res = shl x, 4`
- `res = x - 1`

---

### 3. **Multi-Instruction Operation**

Questo passo combina più istruzioni in una singola operazione per ridurre la complessità del codice. Ad esempio:
- **Addizione seguita da sottrazione**: `a = x + y ; b = a - y → b = x`
- **Sottrazione seguita da addizione**: ``a = x - y ; b = a + y → b = x`

---

## Prerequisiti software

Per eseguire i passi di ottimizzazione, sono necessari i seguenti strumenti:
- **LLVM-19**
- **cmake**
- **opt-19**

## Creazione dell'ambiente di test

Una volta clonato il repository, ed entrati all'interno di esso, avremo la seguente struttura

    ProgramRootFolder
    |_ Test #Conterrà il codice LLVM da ottimizzare
    |_ CMakeList.txt #Contiene le istruzioni di building del passo
    \_ Pass.cpp #L'effettivo passo che servirà poi per l'ottimizzazione del codice LLVM

Dobbiamo ora creare all'interno una directroy Build/
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

## Applicazione dei passi di ottimizzazione
    
1.  **Building del passo LLVM**

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

lanciando il comando ```make```, andiamo a compilare il file sorgente in cui sono inclusi i 3 passi di ottimizzazione (Pass.cpp) 
- Verrà quindi generato un file .so (shared object), che verrà applicato ai file LLVM da ottimizzare
    
2.  **Esecuzione dei passi di ottimizzazione:**

Per eseguire i nostri passi di ottimizzazione utilizzeremo lo strumento OPT, alla quale passeremo il nostro file in formato shared object, precedentemente creato.

Il comando che utilizzeremo è il seguente:

```bash
opt-19 -load-pass-plugin=Build/libAssignement.so -passes='Algebraic-Identity Test/AlgebraicIdentityTest -So Optimized.ll
```
- **`-load-pass-plugin`**: Carichiamo il file `.so` contenente i passi di ottimizzazione.
- **`-passes`**: Specifica quale dei 3 passi di ottimizzazione vogliamo applicare.
- **`-S`**: Genera un file LLVM leggibile.
- **`-o`**: Specifica il file di output ottimizzato.

3.  **Analisi dell'IR ottimizzato:**

Si può analizzare il risultato dell'ottimizzazione all'interno del file che verrà generato "Optimized.ll"

## Conclusioni

Questi passi di ottimizzazione migliorano l'efficienza delle IR con tali problematiche, eliminando istruzioni ridontanti e pesanti.


