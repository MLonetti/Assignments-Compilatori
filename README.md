# PRIMO ASSIGNMENT

In questo repository, sono presenti i 3 passi del Primo Assignment del corso di Compilatori.

Possiamo notare 3 directory diverse (ora solo una)
- 	Algebraic_identity
- 	...
-	...

# aggiornamento !!! 

non dobbiamo avere 3 directory per ogni passo!

dobbiamo fare tutto all'interno di una sola directory:
	abbiamo il nostro FirstAssignmentPass.cpp in cui andiamo a mettere i nostri 3 passi separatamente: ogni passo è una struct

seguendo la struttura del rick, lui chiama i metodi runOnFunction e runOnBasicBlock direttamente dentro la struct di ogni singolo passo.
	- Riassumendo, abbiamo in un modulo cpp i 3 passi, che saranno contraddistinti da una struct diversa.


abbiamo quindi un solo Build, un solo file CMakefile.txt, directory test ecc.. ecc..

come applichaimo le singole ottimizzazioni? dobbiamo andare a manipolare la parte in basso del codice, dove abbiamo:

if (Name == Algebrai-identity) -> chiama la struct AlgIde

bisogna quindi fare un else if in cui andiamo a vedere se Name, ovvero il flag che diamo in ingresso ad opt, è uguale ad una delle 3 keyword associate alle 3 struct che abbiamo
nel sorgente.

nel momento in cui la condizione è valida, si chiama la struct associata, e quindi viene eseguito il passo chiamato.

possiamo creare quindi un file .ll in cui possimao fare le 3 ottimizzazioni, così che quando chiamiamo opt applichiamo i flag uno per volta, e andiamo ad ottimizzare
tutto il sorgente LLVM con le 3 ottimizzazioni.

