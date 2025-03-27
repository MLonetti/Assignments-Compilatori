//=============================================================================
// FILE:
//    aPass1.cpp
//
// DESCRIPTION:
//    Multi-instruction optimizaion -> a = b + 1
//  -                                  c = a - 1 -> a = b 

// diventerà a = b + 1
//           c = b

// controllo le istruzioni iterativamente. 

// se becco una istruzione che è una add con un operando costante, controllo l'istruzione successiva.

// se è una sub con operando costante uguale a quello che ho trovato prima, e l'altro operando, corrisponde alla istruzione rpecedente
// allora posso eliminare l'istruzione e rimpiazzare gli usi direttamente con l'operando della prima addizione

//
//
// License: MIT
//=============================================================================
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

bool is_add(Instruction &I){
  if(BinaryOperator *BO = dyn_cast<BinaryOperator>(&I)){
    if(BO->getOpcode() == Instruction::Add){
      return true;
    }
  }
  return false;
}

bool is_sub(Instruction &I){
  if(BinaryOperator *BO = dyn_cast<BinaryOperator>(&I)){
    if(BO->getOpcode() == Instruction::Sub){
      return true;
    }
  }
  return false;
}

//-----------------------------------------------------------------------------
// TestPass implementation
//-----------------------------------------------------------------------------
// No need to expose the internals of the pass to the outside world - keep
// everything in an anonymous namespace.


bool runOnBasicBlock(BasicBlock &B) {
  bool transformed = false;
  std::vector<Instruction *> ToErase; //vettore che conterrà le istruzioni da eliminare

  for(Instruction &I : B){
    outs() << "Instruction: " << I << "\n";
    int i = 0;

    if (is_add(I)){
      for(auto *Iter = I.op_begin(); Iter != I.op_end(); ++Iter){
        Value *Op = *Iter;
        if(ConstantInt *CI = dyn_cast<ConstantInt>(Op)){
          outs() << "Trovata somma con operando costante!\n";
          Value *otherOp = I.getOperand(1-i);
          Instruction*nextInst = I.getNextNode();

          //ora vediamo la prossima istruzione se è una sub con le caratteristiche che ci interessano
          if(nextInst != nullptr && is_sub(*nextInst)){
            Value *operando0 = nextInst->getOperand(0);

            Value *operando1 = nextInst->getOperand(1);

            if(operando0 == &I && operando1 == Op){ //Dove la Op è la costante intera
              outs() << "Trovata sub ridondante!: " << *nextInst << "\n";
              nextInst -> replaceAllUsesWith(otherOp); //sostituisco tutti gli usi della sub con l'add

              ToErase.push_back(nextInst); //aggiungo l'istruzione da eliminare al vettore
              transformed = true;
              outs() << "Istruzione eliminata!\n";

              outs() <<"ora le istruzioni utilizzeranno direttamente l'operando dell'add! " << *otherOp << "\n";
            }
          }
        }
        i=i+1;
      }
    }
  }

  for(auto *I : ToErase){
    I->eraseFromParent(); //elimino le istruzioni
  }
  return transformed;
}

bool runOnFunction(Function &F) {
  bool transformed = false;
  for (auto Iter = F.begin(); Iter != F.end(); ++Iter) {
      if(runOnBasicBlock(*Iter)){
        transformed = true;
      }
    }
    return transformed;
  }


namespace {
// dentro namespace mettiamo le altre struct, quindi implmentiamo gli altri passi
// Discorso per il futuro! quando i passi funzionano singolarmente e vogliamo metterli su singolarmente

//tutte le funzioni, come detto nel readme, come runOn..., andranno dentro la struct del relativo passo! ora le mettiamo addirittura 
//fuori dal namespace, ma in futuro andranno dentro la struct del passo

// New PM implementation
struct MultInstrOpt: PassInfoMixin<MultInstrOpt> {
  // Main entry point, takes IR unit to run the pass on (&F) and the
  // corresponding pass manager (to be queried if need be)
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {

    runOnFunction(F);
  	

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
                  if (Name == "Multi-Instruction-Optimization") { // nome del passo che passeremo ad opt
                    FPM.addPass(MultInstrOpt());
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