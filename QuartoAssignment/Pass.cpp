//=============================================================================
/*

  Quarto Assignment:
    - ristrutturare il codice seguendo gl appunti su ipad
      - implementa controlli su: adiacenza loop e CFG ...

*/
//=============================================================================
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/Dominators.h"
#include "llvm/Analysis/PostDominators.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/DependenceAnalysis.h"

using namespace llvm;


//-----------------------------------------------------------------------------
// TestPass implementation
//-----------------------------------------------------------------------------
// No need to expose the internals of the pass to the outside world - keep
// everything in an anonymous namespace.
namespace {


// New PM implementation
struct LoopFusionPass0: PassInfoMixin<LoopFusionPass0> {
  // Main entry point, takes IR unit to run the pass on (&F) and the
  // corresponding pass manager (to be queried if need be)

/*
  definiamo la funzione loopFusion dove facciamo il controllo sulla coppia di loop: se tutti i controlli ritornano true, allora la funzione ritorna treu e si
  può fare appunto la loop fusion.

*/ 

/*
  si visitano prima tutti i loop del livello superiore,e si controlla per tutte le coppie se è posibile la loop fusion

  Poi si controllano in maniera ricorsiva eventuali loop figli, se ogni singolo loop ha almeno 2 loop figli.
*/
void visitaLoops(std::vector<Loop*> &Loops, LoopInfo &LI, DominatorTree &DT, PostDominatorTree &PDT, ScalarEvolution &SE, DependenceInfo &DI){
  //visita i loop e controlla se sono adiacenti, se hanno lo stesso numero di iterazioni, se sono equivalenti come CFG e se non hanno dipendenze negative
  for(int i = 0; i < Loops.size(); i++){
    for(int j = 0; j < Loops.size(); j++){
      if(i!=j){
        if(loopFusionPossible(Loops[i], Loops[j], DT, PDT, SE, DI)){
          //se i due loop sono adiacenti, hanno lo stesso numero di iterazioni, sono equivalenti come CFG e non hanno dipendenze negative,
          // allora si può fare la loop fusion
          errs() << "Loop Fusion possibile tra: " << *Loops[i] << " e " << *Loops[j] << "\n";
          loopFusion(Loops[i], Loops[j]);
        }
      }
    }
    std::vector<Loop*> SubLoops = Loops[i]->getSubLoops();
    if(SubLoops.size() > 1){
      //se il loop ha dei figli, allora visitiamo i figli
      visitaLoops(SubLoops, LI, DT, PDT, SE, DI);
    }
  }
}



void loopFusion(Loop *L1, Loop *L2){
  PHINode *L1IndVar = L1->getCanonicalInductionVariable();
  errs() << "Induction variable del primo loop: " << *L1IndVar << "\n";
  PHINode *L2IndVar = L2->getCanonicalInductionVariable();
  errs() << "Induction variable del secondo loop: " << *L2IndVar << "\n";
  
  L2IndVar->replaceAllUsesWith(L1IndVar); //sostituiamo tutti gli usi della variabile di induzione del secondo loop con quella del primo loop

  /*
    MODIFICA 1:
    L'header del loop 1 punterà all'exit del loop 2, così da non avere più il loop 2 come figlio del loop 1.
    In questo modo il loop 2 sarà integrato nel body del loop 1 e non più un figlio del loop 1.
  */

  BasicBlock *L1Header = L1->getHeader();
  BasicBlock *L1Exit = L1->getExitBlock();
  BasicBlock *L2Exit = L2->getExitBlock();

  Instruction* L1HeaderTerm = L1Header->getTerminator(); //prendiamo l'istruzione terminatrice dell'header del loop 1

  if(BranchInst *BI = dyn_cast<BranchInst>(L1HeaderTerm)){
    if(BI->isConditional()){
      //se l'istruzione terminale dell'header del loop 1 è una branch condizionale, allora possiamo andare in due possibili basic block
      if(BI->getSuccessor(0) == L1Exit){
        //se il primo successore è l'header del loop 1, allora dobbiamo cambiare il secondo successore con l'exit del loop 2
        BI->setSuccessor(0, L2Exit); //modifichiamo il secondo successore dell'istruzione branch condizionale dell'header del loop 1
      }else if(BI->getSuccessor(1) == L1Exit){
        //se il secondo successore è l'header del loop 1, allora dobbiamo cambiare il primo successore con l'exit del loop 2
        BI->setSuccessor(1, L2Exit); //modifichiamo il primo successore dell'istruzione branch condizionale dell'header del loop 1
      }
    }
  }
  

  /*
    MODIFICA 2:
    Il Body di L1 punterà al Body di L2.
  */

  //otteniamo il primo basic block del body del loop 2.
  BasicBlock *L2Header = L2->getHeader();
  BasicBlock *L2FirstBodyBlock = nullptr;
  
  for(BasicBlock *BB : successors(L2Header)){
    // cerchiamo il primo basic block del body del loop 2, che non sia il latch
    if(BB != L2Exit){
      L2FirstBodyBlock = BB;
      break; // una volta trovato il primo blocco del body del loop 2, usciamo dal ciclo
    }
  }

  //otteniamo ora l'ultimo basic block del body del loop 1, che punterà al primo basic block del body del loop 2

  BasicBlock *L1lastBodyBlock = L1->getLoopLatch()->getSinglePredecessor(); //prendiamo il predecessore del latch del loop 1, che è l'ultimo blocco del body del loop 1
  Instruction *L1lastBodyTerm = L1lastBodyBlock->getTerminator(); //prendiamo l'istruzione terminatrice dell'ultimo blocco del body del loop 1

  if(BranchInst *BI = dyn_cast<BranchInst>(L1lastBodyTerm)){
    BI->setSuccessor(0, L2FirstBodyBlock);//ora il body di L1 punta al body di L2 e non più al latch di L1
  }

  /*
    MODIFICA 3:
    Il body di L2 punterà al latch di L1.
  */
  
  BasicBlock *L2lastBodyBlock = L2->getLoopLatch()->getSinglePredecessor(); //prendiamo il predecessore del latch del loop 2, che è l'ultimo blocco del body del loop 2
  Instruction *L2lastBodyTerm = L2lastBodyBlock->getTerminator(); //prendiamo l'istruzione terminatrice dell'ultimo blocco del body del loop 2

  if(BranchInst *BI = dyn_cast<BranchInst>(L2lastBodyTerm)){
    BI->setSuccessor(0, L1->getLoopLatch()); //ora il body di L2 punta al latch di L1 e non più al latch di L2
  }
  
  /*
    MODIFICA 4:
    L'header di L2 invece che puntare al blocco exit, punterà al latch di L2, così da staccare il CFG del loop 2 dal loop 1.
    Abbiamo difatto già integrato il corpo del loop 2 nel corpo del loop 1, ora dobbiamo staccare il CFG del loop 2 dal loop 1.
  */

  Instruction *L2HeaderTerm = L2Header->getTerminator(); //prendiamo l'istruzione terminatrice dell'header del loop 2

  if(BranchInst *BI = dyn_cast<BranchInst>(L2HeaderTerm)){
    if(BI->isConditional()){
      //se l'istruzione terminale dell'header del loop 2 è una branch condizionale, allora possiamo andare in due possibili basic block
      BI->setSuccessor(0, L2->getLoopLatch()); //modifichiamo il primo successore dell'istruzione branch condizionale dell'header del loop 2
      BI->setSuccessor(1, L2->getLoopLatch()); //modifichiamo il secondo successore dell'istruzione branch condizionale dell'header del loop 2
      //così scolleghiamo il CFG del loop 2 dal loop 1, in quanto ora l'header del loop 2 punterà al latch del loop 2
    }
  }
}

bool analisiDipendenze(Loop *L1, Loop *L2, DependenceInfo &DI, ScalarEvolution &SE){
  // controlla se ci sono dipendenze negative tra i due loop
  // se ci sono dipendenze negative allora non si può fare la loop fusion

  /*
    Per comprendere questo controllo, dobbiamo prima comprendere come sono rappresentate le store in LLVM
    esempio:

    %1 = getelementptr inbounds [10 x i32], ptr %array, i64 0, i64 %offset
    %2 = store i32 valore, i32* %1, align 4

    Quando si accede alla store:
      il primo operando (valore) è il valore che si sta caricando nell'indirizzo in memoria
      il secondo operando è l'indirizzo in memoria dove si sta caricando il valore

      in questo caso, prendiamo l'operando %1, lo castiamo ad instruction e qui vediamo due operandi:
        - il primo operando è il base register dell'array
        - l'ultimo operando è l'offset

    Quindi, questo è il metodo in cui accederemo alle store, prenderemo base register ed offset per vedere se poi avremo delle dipendenze negative
    nelle load che accederanno allo stesso indirizzo in memoria.

    Per le dipendenze negative: se l'offset della load è maggiore di quello della store, allora avremo una dipendenza negativa

    
  */

  for(BasicBlock *BB : L1->getBlocks()){
    for(Instruction &I : *BB){
      //vediamo se l'istruzione è una store
      if(StoreInst *ST = dyn_cast<StoreInst>(&I)){
        //se l'istruzione è una store dobbiamo ottenere il base address
        Value* DestOperand = ST-> getOperand(1); //ritorna l'operando dell'indirizzo in memoria dove si fa la store
        // non è ancora il base register, è l'indirizzo considerando l'offset+base register, da questo dato dobbiamo ottenere il base register

        //facciamo il cast ad Instruction dell'operando per ottenere un oggetto Instruction, da qui prelevare l'operando che rappresenta il base register
        //preleviamo anche l'offest, che useremo per calcolare se la dipendenza è negativa.
        outs() << "istruzione store: " << ST << "\n";
        outs() << "operando destinazione: " << *DestOperand << "\n";


        if(GetElementPtrInst *GEP = dyn_cast<GetElementPtrInst>(DestOperand)){
          //se l'istruzione è una getelementptr, allora possiamo fare il cast ad Instruction
          //e prelevare gli operandi che ci interessano
        
          if(Instruction *Inst = dyn_cast<Instruction>(DestOperand)){
            Value *BaseReg = Inst->getOperand(0); //ritorna il base register
            Value *Offset = Inst->getOperand(Inst->getNumOperands() - 1); //ritorna l'offset

            outs() << "Base register: " << *BaseReg << "\n";
            outs() << "Offset: " << *Offset << "\n";
          
        
            //iteriamo tutte le load del secondo loop, poi controlliamo se il base register è uguale, altrimenti già non ci sarà dipendenza negativa
            //se base register è uguale, ci concentreremo sull'offset, forse sarà da sfruttare la scalar evolution
            // in quanto i registri di offset saranno sicuramente diversi, per l'SSA.

            for(BasicBlock *BB2 : L2->getBlocks()){
              for(Instruction &I2 : *BB2){
                //vediamo se l'istruzione è una load
                if(LoadInst *LO = dyn_cast<LoadInst>(&I2)){
                  //se l'istruzione è una load dobbiamo ottenere il base address
                  Value* SrcOperand = LO-> getOperand(0); //ritorna l'operando dell'indirizzo in memoria dove si fa la load
                  outs() << "istruzione load: " << LO << "\n";
                  outs() << "operando sorgente: " << SrcOperand << "\n";

                  if(GetElementPtrInst *GEP2 = dyn_cast<GetElementPtrInst>(SrcOperand)){
                    if(Instruction *Inst2 = dyn_cast<Instruction>(GEP2)){
                      Value *BaseReg2 = Inst2->getOperand(0); //ritorna il base register
                      Value *Offset2 = Inst2->getOperand(Inst2->getNumOperands() - 1); //ritorna l'offset

                      outs() << "Base register: " << BaseReg2 << "\n";
                      outs() << "Offset: " << Offset2 << "\n";

                      if(BaseReg == BaseReg2){
                        outs() << "I due loop hanno lo stesso base register\n";

                        const SCEV *StoreOffset = SE.getSCEV(Offset);
                        const SCEV *LoadOffset = SE.getSCEV(Offset2);

                        outs () << "StoreOffset: " << *StoreOffset << "\n";
                        outs () << "LoadOffset: " << *LoadOffset << "\n";

                        const SCEVAddRecExpr *StoreAR = dyn_cast<SCEVAddRecExpr>(StoreOffset);
                        const SCEVAddRecExpr *LoadAR = dyn_cast<SCEVAddRecExpr>(LoadOffset);

                        if(StoreAR && LoadAR){
                          //se entrambi sono SCEVAddRecExpr, allora possiamo fare il controllo sulla dipendenza negativa
                          // controlliamo se il passo della store è maggiore del passo della load
                          const SCEV *StoreStart = StoreAR->getStart();
                          const SCEV *LoadStart = LoadAR->getStart();

                          //ora controlliamo gli incrementi
                          const SCEV *StoreStep = StoreAR->getStepRecurrence(SE);
                          const SCEV *LoadStep = LoadAR->getStepRecurrence(SE);

                          outs() << "Store start: " << *StoreStart << ", step: " << *StoreStep << "\n";
                          outs() << "Load start: " << *LoadStart << ", step: " << *LoadStep << "\n";

                          //se gli step sono uguali, allora controlliamo gli start
                          if(StoreStep == LoadStep){
                            const SCEV *diffStart = SE.getMinusSCEV(StoreStart, LoadStart);

                            if(const SCEVConstant *DiffConst = dyn_cast<SCEVConstant>(diffStart)){
                              int64_t diffValue = DiffConst->getAPInt().getSExtValue();
                              outs() << "Differenza tra gli start: " << diffValue << "\n";
                              if(diffValue < 0){
                                //se la differenza è negativa, allora abbiamo una dipendenza negativa
                                outs() << "Abbiamo una dipendenza negativa tra i due loop\n";
                                return false; //se abbiamo una dipendenza negativa, allora non possiamo fare la loop fusion
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  return true; //se non ci sono dipendenze negative, allora si può fare la loop fusion
}

bool sameNumIterations(Loop *L1, Loop *L2, ScalarEvolution &SE){
  // controlla se i due loop hanno lo stesso numero di iterazioni
  // si può fare in questo modo: si calcola il numero di iterazioni del primo loop e si confronta con il secondo loop

  // se sono uguali allora ritorna true, altrimenti false
  unsigned L1Num = SE.getSmallConstantTripCount(L1);
  unsigned L2Num = SE.getSmallConstantTripCount(L2);

  outs() << "Iterazioni L1: " << L1Num << "\n";
  outs() << "Iterazioni L2: " << L2Num << "\n";
  
  if(L1Num == 0 || L2Num == 0){
    outs() << "impossibile definire il numero di iterazioni\n";
    //può avere 0 iterazioni se non si riesce a determinare il numero effettivo di iterazioni
    return false;
  }
  else if(L1Num == L2Num){
    outs() << "I due loop hanno lo stesso numero di iterazioni\n";
    return true;
  }
  outs() << "I due loop non hanno lo stesso numero di iterazioni\n";
  return false;
}

bool cfgEquivalenti(Loop *L1, Loop *L2, DominatorTree &DT, PostDominatorTree &PDT){
  // guarda README.md per i controlli effettuati in tale funzione

  if(L1->isGuarded() && L2->isGuarded()){
    outs () << "Entrambi i loop sono Guard\n";
    //se entrambi i loop sono Guard, la condizione nelle istruzioni guard deve essere uguale
    if(L1->getLoopGuardBranch()->isSameOperationAs(L2->getLoopGuardBranch())){
      //se le condizioni sono uguali, allora si può continuare con i controlli
      //si controlla se l'entry di L1 domina l'entry di L2 e viceversa, così che se entriamo nel primo Loop, sicuramente si entra nel secondo
      // e se si è eseguito il secondo loop, allora sicuramente si è eseguito anche il primo
      outs() << "I due loop hanno stessa condizione\n";

      BasicBlock *L1Guard = L1->getLoopGuardBranch()->getParent(); //ritorna il BB del guard del primo loop
      BasicBlock *L2Guard = L2->getLoopGuardBranch()->getParent(); //ritorna il BB del guard del secondo loop
      if(DT.dominates(L1Guard, L2Guard) && PDT.dominates(L2Guard, L1Guard)){
        //se l'entry di L1 domina l'entry di L2 e viceversa, allora la CFG è equivalente
        outs() << "CFG equivalenti, i due loop sono giardini e hanno stessa condizione\n";
        return true; 
      }

    }
    // se entrambi i loop non sono Guard ma normali, si guarda che la entry di L1 domina la entry di L2
  }else if(!L1->isGuarded() && !L2->isGuarded()){
    outs () << "Entrambi i loop sono normali\n";
    BasicBlock *L1Entry = L1->getHeader(); //entry del primo loop
    BasicBlock *L2Entry = L2->getHeader(); //entry del secondo loop
    if(DT.dominates(L1Entry, L2Entry) && PDT.dominates(L2Entry, L1Entry)){
      //se l'entry di L1 domina l'entry di L2 e viceversa, allora la CFG è equivalente
      outs() << "CFG equivalenti\n";
      return true; 
    }
    
  }
  outs() << "CFG non equivalenti \n";
  return false;
  //ovvero se uno dei due è guard oppure se sono entrambi guard ma la condizione è diversa.
}

bool sonoAdiacenti(Loop *L1, Loop *L2) {
  // facciamo in questa funzione il controllo sull'adiacenza dei loop. se sono adiacenti, allora ritorna true
  // nella documentazione sono spiegati i controlli che ora effettueremo:

  BasicBlock *L1Exit = L1->getExitBlock();//ritorna null se ci sono più exit blocks.

  if(L2->isGuarded()){
    //si controlla se il BB di uscita di L1, corrisponde al BB Guard di L2.
    if(L1Exit == L2->getLoopGuardBranch() -> getParent()->getPrevNode()){ // si prende il BB predecessore della guardia, in quanto nei Guarded 
                                                                         // prima della guardia c'è sempre un blocco.
      // getLoopGuardBranch ritorna l'istruzone branch del Guard, in cui si analizza se entrare nel loop o andare verso l'uscita
      // da questa istruzione branch risaliamo al basic block

      outs() << "I due loop sono adiacenti\n";
      return true; // dunque se il secondo loop è guard e si verifica questa condizione, allora sono adiacenti
    }
  }else{
    // se il secondo loop non è Guard allora la exit del primo loop deve corrispondere con il preheader 
    if(L1Exit == L2->getLoopPreheader()){
      //se il secondo loop è normale, allora si controla che il BB di uscita di L1 corrisponda al BB preheader di L2
      outs() << "I due loop sono adiacenti\n";
      return true; 
    } 
  }
  outs() << "I due loop non sono adiacenti\n";
  return false; //se non si verifica nessuna delle due condizioni, allora i loop non sono adiacentu, ma è presente qualcosa tra i due loop
}

bool loopFusionPossible(Loop *L1, Loop *L2, DominatorTree &DT, PostDominatorTree &PDT, ScalarEvolution &SE, DependenceInfo &DI){
  /*
    PRIMO CONTROLLO:
    controllo se i loop sono adiacenti
  */
  if(sonoAdiacenti(L1, L2)){
    if(cfgEquivalenti(L1, L2, DT, PDT)){
      if(sameNumIterations(L1, L2, SE)){
        if(!analisiDipendenze(L1, L2, DI, SE)){
          return true;
        }
      }
    }
  }

 
  return false; //se anche solo un controllo non è soddisfatto, non si può effettuare la loop fusion
}

PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM) {
  LoopInfo &LI = AM.getResult<LoopAnalysis>(F);
  DominatorTree &DT = AM.getResult<DominatorTreeAnalysis>(F);
  PostDominatorTree &PDT = AM.getResult<PostDominatorTreeAnalysis>(F);
  ScalarEvolution &SE = AM.getResult<ScalarEvolutionAnalysis>(F);
  DependenceInfo &DI = AM.getResult<DependenceAnalysis>(F);

  //prendiamo il vettore dei loop -> abbiamo solamente quelli del livello superiore
  std::vector<Loop*> Loops = LI.getTopLevelLoops();

  visitaLoops(Loops, LI, DT, PDT, SE, DI);

  /*for (int i = 0; i < Loops.size(); i++){
    errs() << "Loop " << i << ": " << *Loops[i] << "\n";

    // dal loop corrente, prendiamo il vettore dei loop figli
    std::vector<Loop*> SubLoops = Loops[i]->getSubLoops();
    for(int j = 0; j < SubLoops.size(); j++){
      errs() << "subLoop " << j << ": " << *SubLoops[j] << "\n";
    }
  }
  

  if(loopFusionPossible(L1, L2, DT, PDT, SE, DI)){
    //si fa la loop fusion

    //modifichiamo gli usi della induction variable del loop 2 con quelli del loop 1

    loopFusion(L1, L2);

  }
  */

  return PreservedAnalyses::all();
}


  // Without isRequired returning true, this pass will be skipped for functions
  // decorated with the optnone LLVM attribute. Note that clang -O0 decorates
  // all functions with optnone.
  static bool isRequired() { return true; }
};
} // namespace


//-----------------------------------------------------------------------------
// New PM Registration
//-----------------------------------------------------------------------------
llvm::PassPluginLibraryInfo getTestPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "LoopFusionPass", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "loop-fusion-pass") {
                    FPM.addPass(LoopFusionPass0());
                    return true;
                  }
                  return false;
                });
          }};
}

// This is the core interface for pass plugins. It guarantees that 'opt' will
// be able to recognize TestPass when added to the pass pipeline on the
// command line, i.e. via '-passes=test-pass'
extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getTestPassPluginInfo();
}
