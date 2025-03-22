//=============================================================================
// FILE:
//    aPass1.cpp
//
// DESCRIPTION:
//    Questo è il primo passo del primo assignment.
//    Dal nome sel passo, ovvero Algebraic-Identity, si comprende fin da subito che andremo ad applicare un'ottimizzazione di questo tipo.

//    L'ottimizzazione di Algebraic Identity è una tecnica di ottimizzazione del codice che consiste nel semplificare le espressioni matematiche.
//    Questa tecnica si basa sul fatto che alcune operazioni matematiche possono essere semplificate, ad esempio:
//      - x + 0 = x
//      - x * 1 = x

//    Andremo quindi ad analizzare il file prova.ll presente nella directory ./test, nella quale saranno presenti queste problematiche, e andremo ad applicare
//    l'ottimizzazione di Algebraic Identity, rimuovendo anche dal codice le istruzioni che non utilizzeremo più

//    Nel pratico l'idea è quella di andare a rimpiazzare gli usi dell'istruzione ridontante, con l'operando che non è l'elemento neutro nell'operazione
//        ES: x4 = sum x3, 0
//            x5 = sum x4, 0

//    Quello che faremo è che le istruzioni che usano x4 (ad esempio x5), utilizzeranno direttamente x3, e rimuoveremo dal codice l'istruzione morta senza usi.

//    Quando utilizziamo il metodo replaceAllUsesWith, solitamente abbiamo dato come argomento un'istruzione. In realtà ora diamo come argomento un Operando.
//    Non c'è nulla di strano, in quanto un operando è un oggetto di classe Value, mentre un'istruzione è di classe Instruction, che però eredita da Value, 
//    per questo possiamo usarlo come argomento del metodo.

//
//
// License: MIT
//=============================================================================
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

bool is_mul(Instruction &I){
  if (auto *BO = dyn_cast<BinaryOperator>(&I)){
    if(BO->getOpcode() == Instruction::Mul){
      return true;
    }
  }
  return false;
}

bool is_add(Instruction &I){
  if (auto *BO = dyn_cast<BinaryOperator>(&I)){
    if(BO->getOpcode() == Instruction::Add){
      return true;
    }
  }
  return false;
}

bool runOnBasicBlock(BasicBlock &BB){
  bool transformed = false;
  std::vector<Instruction *> toErase; // vettore di istruzioni da eliminare

  for(Instruction &Inst : BB){ //iteriamo le istruzioni all'interno del basicblock passato

    int i = 0; //ogni istruzione iterata, il contatore viene ridefinito a 0.

    outs() << "Itero l'istruzione: " << Inst << "\n";

    if(is_mul(Inst)){
      // L'istruzione è una Mul
      // possiamo procedere ad iterare gli operandi, sfruttiamo la lista op dell'istruzione, che contiene appunto gli operandi 
      // per ogni operando guardiamo se si tratta di un costantInt, e se lo è vediamo che sia l'elemento neutro.

      for(auto *Iter = Inst.op_begin(); Iter !=  Inst.op_end(); ++Iter){
        Value *Operand = *Iter;

        if(ConstantInt *CI = dyn_cast<ConstantInt>(Operand)){
          int Val_Operand = CI -> getSExtValue();

          if(Val_Operand == 1){
            outs() << "\tTrovata istruzione Mul con elemento neutro!" <<"\n";
            Value *operando_non_neutrale = Inst.getOperand(1 - i);
              // la i può essere 0 oppure 1
              // se i = 0, vuol dire che l'elemento neutrale è il primo operando, perciò l'altro operando sarà
              // Inst.getOperand(1-0) che è appunto il secondo

              // se la i fosse 1, invece avremmo che l'elemento neutro sarà il secondo parametro, perciò 
              // andiamo a prendere il primo parametro non neutrale con Inst.getOperand(1-1)

              //sfruttiamo il fatto che sappiamo di avere Operand 0 ed Operand 1 e puntiamo ad uno dei due così.

              outs() << "\tTutte le istruzioni che usavano: " << Inst << " utilizzeranno ora: " << *operando_non_neutrale << "\n\n";
            Inst.replaceAllUsesWith(operando_non_neutrale); // Chi prima usava l'istruzione ora usa l'operando 
                                                            // non neutrale.
                                                   
            toErase.push_back(&Inst); // aggiungiamo l'istruzione alla lista di istruzioni da eliminare
            transformed = true;

            break;
            // usciamo dal ciclo anche se abbiamo iterato un solo operando, in quanto se il primo era 
            // l'elemento neutro, abbimao comunque già fatto tutto e non dobbiamo più iterare.
            
          }
        }
        i+=1;
      }
    }else if (is_add(Inst)){
      // si ripete lo stesso procedimento di tutto ciò che abbiamo visto, se l'istruzione iterata è una add
      // iteriamo quindi gli operandi come prima e guardiamo quale è l'elemento neutro
      for(auto *Iter = Inst.op_begin(); Iter !=  Inst.op_end(); ++Iter){
        Value *Operand = *Iter;

        if(ConstantInt *CI = dyn_cast<ConstantInt>(Operand)){
          int Val_Operand = CI -> getSExtValue();

          if(Val_Operand == 0){
            outs() << "\tTrovata istruzione Add con elemento neutro!" <<"\n";
            Value *operando_non_neutrale = Inst.getOperand(1 - i);

            outs() << "\tTutte le istruzioni che usavano: " << Inst << " utilizzeranno ora: " << *operando_non_neutrale << "\n\n";
            Inst.replaceAllUsesWith(operando_non_neutrale);

            toErase.push_back(&Inst);
            transformed = true;

            break;

          }
        }
        i+=1;
      }
    }

  }

  //cancelliamo le istruzioni dopo averle iterate tutte, per evitare probleimi di iterazione
  outs() << "\nHo eliminato le seguenti istruzioni: \n";
  for(Instruction *I : toErase){
    outs() << *I << "\n";
    I->eraseFromParent();
  }

  return transformed;
}


bool runOnFunction(Function &F) {
  // Iteriamo su tutti i BasicBlock della funzione
bool transformed = false;

  for (BasicBlock &BB : F) {
    if(runOnBasicBlock(BB)) {
      transformed = true;
    }
  }

  return transformed;
}


//-----------------------------------------------------------------------------
// TestPass implementation
//-----------------------------------------------------------------------------
// No need to expose the internals of the pass to the outside world - keep
// everything in an anonymous namespace.

namespace {


// New PM implementation
struct AlgIde: PassInfoMixin<AlgIde> {
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
                  if (Name == "Algebraic-Identity") { // nome del passo che passeremo ad opt
                    FPM.addPass(AlgIde());
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