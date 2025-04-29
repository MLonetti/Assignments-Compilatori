//=============================================================================
/*
  Terzo Assigment - Loop Invariant Code Motion - Manuel Vincenzo Lonetti

  Divideremo il passo di ottimizzazione in due parti:
  1) Nella prima parte individueremo le istruzioni Loop Invariant, ovvero le istruzioni che non cambiano il loro valore all'interno del loop. 

  2) Nella seconda parte, andremo ad analizzare le istruzioni Loop Invariant e vedremo se possiamo effettuare la code motion, ovvero spostarle nel preheader
     del loop.

*/
//=============================================================================
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Analysis/LoopInfo.h"

using namespace llvm;


//-----------------------------------------------------------------------------
// TestPass implementation
//-----------------------------------------------------------------------------
// No need to expose the internals of the pass to the outside world - keep
// everything in an anonymous namespace.
namespace {


// New PM implementation
struct TestPass: PassInfoMixin<TestPass> {
  // Main entry point, takes IR unit to run the pass on (&F) and the
  // corresponding pass manager (to be queried if need be)

  bool isInstLoopInvariant(Instruction &I, Loop &L, const std::vector<Instruction*> &LoopInvariantInstructions){
    //controlliamo se l'istruzione è loop invariant

    //come primo controllo, verifichiamo se l'istruzione è una Phi Function, in tal caso non è loop invariant
    if(PHINode *PN = dyn_cast<PHINode>(&I)){
      return false;
    }

    //controlliamo che l'istruzione non sia una branch, in tal caso non è loop invariant
    if (BranchInst *BI = dyn_cast<BranchInst>(&I)) {
      return false;
    }

    //contrilliamo che l'istruzione non sia una call, in tal caso non è loop invariant
    if (CallInst *CI = dyn_cast<CallInst>(&I)) {
      return false;
    }

    //controlliamo che l'istruzione non sia una return, in tal caso non è loop invariant
    if (ReturnInst *RI = dyn_cast<ReturnInst>(&I)) {
      return false;
    }

    //Ora iniziamo a fare i controlli sugli operandi dell'istruzione per determinare se è loop invariant
    for(auto *Iter = I.op_begin(); Iter != I.op_end(); ++Iter){
      Value *Operand = *Iter;

      // controlliamo se l'operando è una costante
      // se entrambi gli operandi sono costanti, allora l'istruzione è loop invariant

      if(ConstantInt *C = dyn_cast<ConstantInt>(Operand)){
        continue;
      }

      // controlliamo se l'operando è definito all'interno del loop. In tal caso l'istruzione non è loop invariant
      if (Instruction *Inst = dyn_cast<Instruction>(Operand)){
        // dobbiamo controllare se questa istruzione è definita fuori dal loop, per fare questo controlliamo che il basic block 
        // in cui si trova l'istruzione, sia dentro al loop

        BasicBlock *B = Inst -> getParent();

        if ((L.contains(B))){
          //l'istruzione non è loop invariant dato che un suo operando è definito all'interno del loop
          return false;
        }
        //altrimenti si continua e si guarderà se l'altro operando sarà definito dentro al loop
      }

      //controlliamo ora se l'operando è attribuibile ad un'istruzione considerata loop invariant
      //se uno degli operandi è loop invariant, allora l'istruzione è loop invariant
      for (auto *LoopInvariantInst : LoopInvariantInstructions){
        if (Instruction *Inst = dyn_cast<Instruction>(Operand)) {
          if (Inst == LoopInvariantInst) {
            // L'istruzione è loop invariant dato che uno dei suoi operandi è loop invariant
            continue;
          }
        }
      }
    }
    //se tutti i controlli sono passati, allora sicuramente l'istruzione è loop invariant
    return true;
    errs() << "L'istruzione " << I << " è loop invariant!\n";
  }

  void RunOnLoops(Loop &L) {
    std::vector<Instruction*> LoopInvariantInstructions;

    // Itera su tutti i BasicBlock del loop
    for (BasicBlock *BB : L.getBlocks()) {
        for (Instruction &I : *BB) {
            // Controlla se l'istruzione è loop invariant
            if (isInstLoopInvariant(I, L, LoopInvariantInstructions)) {
                // Aggiungi l'istruzione al vettore
                LoopInvariantInstructions.push_back(&I);
            }
        }
    }

    // Stampa le istruzioni loop invariant per il loop corrente
    errs() << "Istruzioni Loop Invariant nel Loop con header: " << L.getHeader()->getName() << "\n";
    for (auto *Inst : LoopInvariantInstructions) {
        errs() << *Inst << "\n";
    }
    errs() << "\n";
}

void RunOnLoopInfo(LoopInfo &LI) {
    for (Loop *L : LI) {
        RunOnLoops(*L);
    }
}

PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM) {
    LoopInfo &LI = AM.getResult<LoopAnalysis>(F);

    if(LI.empty()) {
        errs() << "Nessun loop trovato in " << F.getName() << "\n";
        return PreservedAnalyses::all();
    }
    RunOnLoopInfo(LI);

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
  return {LLVM_PLUGIN_API_VERSION, "TestPass", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "test-pass") {
                    FPM.addPass(TestPass());
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


/*
  in run che prende le funzioni che incontra nel codice LLVM, chiamiamo RunOnBasickBlock, che è la funzione che preso un oggetto Functions itera i basick block al suo interno

  in queta funzione, per ogni oggetto basic block iterato, si chiama RunOnBasicBlock, che passa il basic block corrente ed itera tutte le istruzioni al suo interno,
  facendo un'analisi di ogni istruzione grazie al casting
      - recupera per bene queste cose sugli appunti, e ovviamente integra con il codice LLVM e mostra il perche la situazione non funziona nel caso di 
        Loop.ll
*/