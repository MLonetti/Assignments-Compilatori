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
#include "llvm/IR/Dominators.h"

using namespace llvm;


//-----------------------------------------------------------------------------
// TestPass implementation
//-----------------------------------------------------------------------------
// No need to expose the internals of the pass to the outside world - keep
// everything in an anonymous namespace.
namespace {


// New PM implementation
struct LICMPass: PassInfoMixin<LICMPass> {
  // Main entry point, takes IR unit to run the pass on (&F) and the
  // corresponding pass manager (to be queried if need be)

  void moveInst(Instruction &I, Loop &L) {
    if(!L.contains(I.getParent())){
      //istruzione già spostata e quindi non contenuta nel loop
      return;
    }

    for(auto *op = I.op_begin(); op != I.op_end(); ++op){
      
      if(Instruction *Inst = dyn_cast<Instruction>(*op)){
        if(L.contains(Inst->getParent())){
          //Gli operandi dell'istruzione all'interno del loop, vanno prima spostati
          moveInst(*Inst, L);
        }
      }
    }

    // Usciti dal loop siamo sicuri che possiamo spostare l'istruzione, in quanto gli operandi sono stati altrettanto spostati 
    
    // Ottieni il preheader del loop
    BasicBlock *Preheader = L.getLoopPreheader();

    // Sposta l'istruzione nel preheader
    I.moveBefore(Preheader-> getTerminator());
    errs() << "Istruzione spostata: " << I << "\n";
  }

  bool codeMotionPossible(Instruction *Inst, Loop &L, DominatorTree &DT) {
    // Ottieni il blocco che contiene l'istruzione
    BasicBlock *Parent = Inst->getParent();

    // Ottieni tutte le uscite del loop
    llvm::SmallVector<BasicBlock*, 4> ExitBlocks;
    L.getExitBlocks(ExitBlocks);

    // Controlla se il blocco che contiene l'istruzione domina tutte le uscite del loop
    for (BasicBlock *ExitBlock : ExitBlocks) {
      if (!DT.dominates(Parent, ExitBlock)) {
        // Se il blocco non domina una delle uscite, controlla la lista degli usi
        if (Inst->use_empty()) {
          // Se la lista degli usi è vuota, l'istruzione è "dead" e può essere spostata
          return true;
        }

        // Itera su tutti gli usi dell'istruzione
        for (User *U : Inst->users()) {
          if (Instruction *UserInst = dyn_cast<Instruction>(U)) {
            BasicBlock *UserBB = UserInst->getParent();

            // Controlla se il blocco che usa l'istruzione è un successore del blocco exit non dominato dal BB con l'istruzione loop invariant
            // Se è un successore, non possiamo spostare l'istruzione, in quanto è usata e non è dead.
            for (BasicBlock *Succ : successors(ExitBlock)) {
              if (Succ == UserBB) {
                return false; // Non possiamo spostare l'istruzione, poiché è usata in un successore
              }
            }
          }
        }
        // Se nessun blocco che usa l'istruzione è un successore, la code motion è possibile
        return true;
      }
    }

    // Se il blocco domina tutte le uscite, la code motion è possibile
    return true;
  }

  bool isInstLoopInvariant(Instruction &I, Loop &L, const std::vector<Instruction*> &LoopInvariantInstructions){
    //controlliamo se l'istruzione è loop invariant

    //come primo controllo, verifichiamo se l'istruzione è una Phi Function, in tal caso non è loop invariant
    if(PHINode *PN = dyn_cast<PHINode>(&I)){
      return false;
    }

    //se l'istruzione non è una BinaryOperator, non è loop invariant
    BinaryOperator *BO = dyn_cast<BinaryOperator>(&I);
    if (!BO) {
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
        //altrimenti si continua e si guarderà l'altro operando
      }

      //controlliamo ora se l'operando è attribuibile ad un'istruzione considerata loop invariant
      if (Instruction *Inst = dyn_cast<Instruction>(Operand)) {
        if(!isInstLoopInvariant(*Inst, L, LoopInvariantInstructions)){
          //l'istruzione non è loop invariant
          return false;
        }
      }
    }
    //se tutti i controlli sono passati, allora sicuramente l'istruzione è loop invariant
    return true;
    errs() << "L'istruzione " << I << " è loop invariant!\n";
  }

  void runOnLoops(Loop &L, DominatorTree &DT) {
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


    /*-----------------------------------------------------------------------------------------------------------------------------------------------

      SECONDA PARTE DEL PASS: 
      Dopo aver selezionato le istruzioni loop invariant di ogni loop, dovremo effettuare la code motion, ovvero spostarle nel preheader del loop.

    ------------------------------------------------------------------------------------------------------------------------------------------------*/

    for(Instruction *Inst : LoopInvariantInstructions){
      if(codeMotionPossible(Inst, L, DT)){
        // se le condizioni all'interno della funzione Code motion vanno a buon fine
        
        // controlliamo se gli operandi della istruzione sono già stati spostati, se non lo sono e sono ancora nel loop, non possiamo spostare l'istruzione
        moveInst(*Inst, L);
      }
    }
}

void runOnLoopInfo(LoopInfo &LI, DominatorTree &DT) {
    for (Loop *L : LI) {
        runOnLoops(*L, DT);
    }
}

PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM) {
  LoopInfo &LI = AM.getResult<LoopAnalysis>(F);
  DominatorTree &DT = AM.getResult<DominatorTreeAnalysis>(F);

  if(LI.empty()) {
    errs() << "Nessun loop trovato in " << F.getName() << "\n";
    return PreservedAnalyses::all();
  }
  runOnLoopInfo(LI, DT);

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
  return {LLVM_PLUGIN_API_VERSION, "LICMPass", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "LICM-Pass") {
                    FPM.addPass(LICMPass());
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
