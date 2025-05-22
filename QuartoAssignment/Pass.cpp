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
                      /*
                        //se i due base register sono uguali, allora controlliamo se gli offset sono uguali

                        //ora dobbiamo controllare gli offset, per questo utilizzeremo la scalar evolution applicata proprio agli operandi che rappresentano l'offser

                        const SCEV *StoreOffset = SE.getSCEV(Offset);
                        outs () << "StoreOffset: " << *StoreOffset << "\n";
                        const SCEV *LoadOffset = SE.getSCEV(Offset2);
                        outs () << "LoadOffset: " << *LoadOffset << "\n";
                        //se LoadOffset è maggiore di StoreOffset, allora abbiamo una dipendenza negativa
                        

                        const SCEV* diff = SE.getMinusSCEV(StoreOffset, LoadOffset);
                        outs() << "Differenza: " << *diff << "\n";
                        if(SE.isKnownNonPositive(diff)){
                          outs() << "Abbiamo una dipendenza negativa\n";
                          //se abbiamo una dipendenza negativa, allora non possiamo fare la loop fusion
                          return false;
                        }
                      */  

                        //dopo aver fallito con il controllo sugli offset a basso livello, utilizziamo la dependence analysis, analizzando le store e load
                        //con base register uguali.

                        if(auto D = DI.depends(ST, LO, true)){
                          //se la dipendenza è negativa, allora non possiamo fare la loop fusion
                          unsigned Levels = D->getLevels();
                          for(unsigned int i = 0; i < Levels; i++){
                            if(D->isKnownNegative(i)){
                              outs() << "Abbiamo una dipendenza negativa\n";
                              //se abbiamo una dipendenza negativa, allora non possiamo fare la loop fusion
                              return false;
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
    if(L1Exit == L2->getHeader()){
      //se il secondo loop è normale, allora si controla che il BB di uscita di L1 corrisponda al BB preheader di L2
      outs() << "I due loop sono adiacenti\n";
      return true; 
    } 
  }
  outs() << "I due loop non sono adiacenti\n";
  return false; //se non si verifica nessuna delle due condizioni, allora i loop non sono adiacentu, ma è presente qualcosa tra i due loop
}

bool loopFusion(Loop *L1, Loop *L2, DominatorTree &DT, PostDominatorTree &PDT, ScalarEvolution &SE, DependenceInfo &DI){
  /*
    PRIMO CONTROLLO:
    controllo se i loop sono adiacenti
  */
  if(sonoAdiacenti(L1, L2)){
    //if(cfgEquivalenti(L1, L2, DT, PDT)){
      // e qui continuiamo con i controlli... alla fine della catena se tutti sono soddisfatti ritorniamo true
      // mentre al di fuori ritorniamo false
    //}
    
  }

  /*
    SECONDO CONTROLLO:
    controllo se i loop sono equivalenti
  */

  if(cfgEquivalenti(L1, L2, DT, PDT)){
    //se i loop sono adiacenti e la cfg è equivalente, allora si può fare la loop fusion
    //outs() << "I due loop possono essere fusi\n";
    
    
  }

  /*
    TERZO CONTROLLO:
    controllo se i loop HANNO lo stesso numero di iterazioni
  */

  if(sameNumIterations(L1, L2, SE)){
   
  }

  if(analisiDipendenze(L1, L2, DI, SE)){
    //se non ci sono dipendenze tra i due loop, allora si può fare la loop fusion
    //outs() << "I due loop non hanno dipendenze\n";
  }

  return false; //se anche solo un controllo non è soddisfatto, non si può effettuare la loop fusion
}

PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM) {
  LoopInfo &LI = AM.getResult<LoopAnalysis>(F);
  DominatorTree &DT = AM.getResult<DominatorTreeAnalysis>(F);
  PostDominatorTree &PDT = AM.getResult<PostDominatorTreeAnalysis>(F);
  ScalarEvolution &SE = AM.getResult<ScalarEvolutionAnalysis>(F);
  DependenceInfo &DI = AM.getResult<DependenceAnalysis>(F);

  //prendiamo il primo loop ed il secondo
  Loop *L1 = LI.getTopLevelLoops()[1]; //sono al contrario, il secondo indice è il primo in realtà
  Loop *L2 = LI.getTopLevelLoops()[0];

  if(loopFusion(L1, L2, DT, PDT, SE, DI)){
    //si fa la loop fusion
  }


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
