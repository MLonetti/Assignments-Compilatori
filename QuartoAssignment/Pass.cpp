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
#include "llvm/Anqalysis/PostDominators.h"

using namespace llvm;


//-----------------------------------------------------------------------------
// TestPass implementation
//-----------------------------------------------------------------------------
// No need to expose the internals of the pass to the outside world - keep
// everything in an anonymous namespace.
namespace {


// New PM implementation
struct LoopFusionPass: PassInfoMixin<LoopFusionPass> {
  // Main entry point, takes IR unit to run the pass on (&F) and the
  // corresponding pass manager (to be queried if need be)

/*
  definiamo la funzione loopFusion dove facciamo il controllo sulla coppia di loop: se tutti i controlli ritornano true, allora la funzione ritorna treu e si
  può fare appunto la loop fusion.

*/ 

bool cfgEquivalenti(Loop *L1, Loop *L2, DominatorTree &DT, PostDominatorTree &PDT){
  // guarda README.md per i controlli effettuati in tale funzione

  if(L1.isGuarded() && L2.isGuarded()){
    //se entrambi i loop sono Guard, la condizione nelle istruzioni guard deve essere uguale
    if(L1.getLoopGuardBranch()->getCondition() == L2.getLoopGuardBranch()->getCondition()){
      //se le condizioni sono uguali, allora si può continuare con i controlli
      //si controlla se l'entry di L1 domina l'entry di L2 e viceversa, così che se entriamo nel primo Loop, sicuramente si entra nel secondo
      // e se si è eseguito il secondo loop, allora sicuramente si è eseguito anche il primo

      BasicBlock *L1Guard = L1.getLoopGuardBranch()->getParent(); //entry del primo loop
      BasicBlock *L2Guard = L2.getLoopGuardBranch()->getParent(); //entry del secondo loop
      if(DT.dominates(L1Guard, L2Guard) && PDT.dominates(L2Guard, L1Guard)){
        //se l'entry di L1 domina l'entry di L2 e viceversa, allora la CFG è equivalente
        return true; 
      }

    }
    // se entrambi i loop non sono Guard ma normali, si guarda che la entry di L1 domina la entry di L2
  }else if(L1.isnotGuarded() && L2.isNotGuarded()){
    BasicBlock *L1Entry = L1.getLoopPreheader(); //entry del primo loop
    BasicBlock *L2Entry = L2.getLoopPreheader(); //entry del secondo loop
    if(DT.dominates(L1Entry, L2Entry) && PDT.dominates(L2Entry, L1Entry)){
      //se l'entry di L1 domina l'entry di L2 e viceversa, allora la CFG è equivalente
      return true; 
    }
    
  }
  return false
  //ovvero se uno dei due è guard oppure se sono entrambi guard ma la condizione è diversa.
}

bool sonoAdiacenti(Loop *L1, Loop *L2) {
  // facciamo in questa funzione il controllo sull'adiacenza dei loop. se sono adiacenti, allora ritorna true
  // nella documentazione sono spiegati i controlli che ora effettueremo:

  BasicBlock *L1Exit = L1->getExitBlock();//ritorna null se ci sono più exit blocks.

  if(L2.isGuarded()){
    //si controlla se il BB di uscita di L1, corrisponde al BB Guard di L2.
    if(L1Exit == L2.getLoopGuardBranch() -> getParent()){
      // getLoopGuardBranch ritorna l'istruzone branch del Guard, in cui si analizza se entrare nel loop o andare verso l'uscita
      // da questa istruzione branch risaliamo al basic block

      return true; // dunque se il secondo loop è guard e si verifica questa condizione, allora sono adiacenti
    }
  }else{
    // se il secondo loop non è Guard allora la exit del primo loop deve corrispondere con il preheader 
    if(L1Exit == L2->getLoopPreheader()){
      //se il secondo loop è normale, allora si controla che il BB di uscita di L1 corrisponda al BB preheader di L2
      return true; 
    } 
  }
  return false; //se non si verifica nessuna delle due condizioni, allora i loop non sono adiacentu, ma è presente qualcosa tra i due loop
}

bool loopFusion(Loop *L1, Loop *L2, DominatorTree &DT, PostDominatorTree &PDT){
  //controllo se i loop sono adiacenti
  if(sonoAdiacenti(L1, L2)){
    if(cfgEquivalenti(L1, L2, DT, PDT)){
      // e qui continuiamo con i controlli... alla fine della catena se tutti sono soddisfatti ritorniamo true
      // mentre al di fuori ritorniamo false
    }
  }

  return false; //se anche solo un controllo non è soddisfatto, non si può effettuare la loop fusion
}

PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM) {
  LoopInfo &LI = AM.getResult<LoopAnalysis>(F);
  DominatorTree &DT = AM.getResult<DominatorTreeAnalysis>(F);
  PostDominatorTree &PDT = AM.getResult<PostDominatorTreeAnalysis>(F);

  //prendiamo il primo loop ed il secondo
  Loop *L1 = LI.getTopLevelLoop()[0];
  Loop *L2 = LI.getTopLevelLoop()[1];

  if(loopFusion(L1, L2, DT, PDT)) {
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
                    FPM.addPass(LoopFusionPass());
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
