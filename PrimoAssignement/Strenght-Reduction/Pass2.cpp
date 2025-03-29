//=============================================================================
// FILE:
//    aPass1.cpp
//
// DESCRIPTION:
//    Passo in cui andremo ad applicare la strenght reduction
//    questa spiegazione andrà aggiunta al file sorgente che conterrà tutti i passi, come spiegato enll'aggiornamento del ReadMe.md

//    fare quindi una spiegazione del opasso di ottimizzazione, molto breve, in quanto la spiegaazione dettagliata andrà nel readme finale
//     che conterrà tutte le spiegazioni dei passi e del progetto in generale.

//
//
// License: MIT
//=============================================================================
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

//-----------------------------------------------------------------------------
// TestPass implementation
//-----------------------------------------------------------------------------
// No need to expose the internals of the pass to the outside world - keep
// everything in an anonymous namespace.

bool is_mul(Instruction &Inst)
{
  if (BinaryOperator *BO = dyn_cast<BinaryOperator>(&Inst))
  { // si fa il cast per capire se l'istruzione è una binary operator, se lo è si controlla
    // l'opcode dell'istruzione.

    // se l'opcode è Mul, allora diciamo che abbiamo trovato la moltiplicazione,
    // indicando l'istruzione, e ritorna true
    if (BO->getOpcode() == Instruction::Mul)
    {
      outs() << "Trovata una moltiplicazione: " << *BO << "\n";
      return true;
    }
  }
  return false;
}

bool is_div(Instruction &I)
{
  if (auto *BO = dyn_cast<BinaryOperator>(&I))
  {
    if (BO->getOpcode() == Instruction::SDiv || BO->getOpcode() == Instruction::UDiv)
    {
      return true;
    }
  }
  return false;
}

bool runOnBasicBlock(BasicBlock &B)
{
  bool transformed = false;
  std::vector<Instruction *> ToErase; // vettore che conterrà le istruzioni da eliminare

  for (Instruction &I : B)
  {
    int i = 0; // contatore che useremo per prendere l'altro operando dell'istruzione

    outs() << "Analizzo l'istruzione: " << I << "\n";

    if (is_mul(I))
    { // in futuro si può direttamente implementare il cast di tipo if(MulOperator *MU = din_cast<MulOperator>(&I))
      // esiste direttamente questo cast che ritorna true se l'istruzione è una moltiplicazione, e false altrimenti

      for (auto *Iter = I.op_begin(); Iter != I.op_end(); ++Iter)
      { // Iteriamo gli operandi della mul fino a trovare la costante int
        Value *Operand = *Iter;

        if (ConstantInt *CI = dyn_cast<ConstantInt>(Operand))
        {
          APInt Val = CI->getValue();

          if (Val.isPowerOf2())
          {
            outs() << "Trovata MUL con costante intera potenza di 2 ";

            // facciamo la classica roba di sempre
            int shift_val = Val.logBase2(); // valore che utilizzeremo per l'istruzione shift che andiamo a creare

            Value *Other_operand = I.getOperand(1 - i); // prendiamo l'altro operando della moltiplicazione che applicheremo alla shift

            BinaryOperator *NewShift = BinaryOperator::Create(Instruction::Shl, Other_operand, ConstantInt::get(Other_operand->getType(), shift_val));
            outs() << "creo nuova istruzione shift " << *NewShift << " da rimpiazzare con la Mul precedente detectata\n";

            NewShift->insertAfter(&I); // inseriamo l'istruzione dopo la moltiplicazione

            I.replaceAllUsesWith(NewShift); // sostituiamo la moltiplicazione con la shift
            ToErase.push_back(&I);          // aggiungiamo l'istruzione da eliminare al vettore
            transformed = true;

            break; // usciamo dal ciclo, e iteriamo la prossima istruzione
          }
          else if ((Val + 1).isPowerOf2())
          {
            outs() << "Trovata MUL con costante intera che sommata di 1 è potenza di 2: ";
            // siamo nel caso in cui x * 15  -> x * 16 - x -> perchè 16-1 = 15

            int shift_val = (Val + 1).logBase2();
            Value *Other_operand = I.getOperand(1 - i);
            BinaryOperator *NewShift = BinaryOperator::Create(Instruction::Shl, Other_operand, ConstantInt::get(Other_operand->getType(), shift_val));
            NewShift->insertAfter(&I);

            I.replaceAllUsesWith(NewShift);
            ToErase.push_back(&I);

            BinaryOperator *NewSub = BinaryOperator::Create(Instruction::Sub, NewShift, Other_operand);
            NewSub->insertAfter(NewShift);

            outs() << "creo nuova istruzione shift " << *NewShift << " da rimpiazzare con la Mul precedente detectata\n";
            outs() << "ed in seguito creo istruzione sub " << *NewSub;

            transformed = true;

            break; // usciamo dal ciclo, e iteriamo la prossima istruzione
          }
          else if ((Val - 1).isPowerOf2())
          {
            outs() << "Trovata MUL con costante intera che sottratta di 1 è potenza di 2: ";
            // siamo nel caso in cui x * 17  -> x * 16 + x -> perchè 16+1 = 17

            int shift_val = (Val - 1).logBase2();
            Value *Other_operand = I.getOperand(1 - i);
            BinaryOperator *NewShift = BinaryOperator::Create(Instruction::Shl, Other_operand, ConstantInt::get(Other_operand->getType(), shift_val));
            NewShift->insertAfter(&I);

            I.replaceAllUsesWith(NewShift);
            ToErase.push_back(&I);

            BinaryOperator *NewAdd = BinaryOperator::Create(Instruction::Add, NewShift, Other_operand);
            NewAdd->insertAfter(NewShift);

            outs() << "creo nuova istruzione shift " << *NewShift << " da rimpiazzare con la Mul precedente detectata\n";
            outs() << "ed in seguito creo istruzione add " << *NewAdd;

            transformed = true;

            break; // usciamo dal ciclo, e iteriamo la prossima istruzione
          }
        }

        i += 1;
      }
    }
    else if (is_div(I))
    {                                   // Controlliamo se l'istruzione è una divisione
      Value *Operand = I.getOperand(1); // Prendiamo il secondo operando della divisione
      if (ConstantInt *CI = dyn_cast<ConstantInt>(Operand))
      { // Verifichiamo se l'operando è una costante intera
        APInt Val = CI->getValue();

        if (Val.isPowerOf2())
        { // Controlliamo se la costante è una potenza di 2
          outs() << "Trovata DIV con costante intera potenza di 2: ";

          int shift_val = Val.logBase2();         // Calcoliamo il valore dello shift
          Value *Other_operand = I.getOperand(0); // Otteniamo l'altro operando della divisione (Il primo)

          BinaryOperator *NewShift = BinaryOperator::Create(Instruction::LShr, Other_operand, ConstantInt::get(Other_operand->getType(), shift_val));
          NewShift->insertAfter(&I); // Inseriamo l'istruzione di shift dopo la divisione

          I.replaceAllUsesWith(NewShift); // Sostituiamo la divisione con lo shift
          ToErase.push_back(&I);          // Aggiungiamo la divisione alla lista delle istruzioni da eliminare

          outs() << "creo nuova istruzione shift-right " << *NewShift << " da rimpiazzare con la Div precedente detectata\n";

          transformed = true;
        }
      }
    }
  }

  // eliminiamo le istruzioni di cui abbiamo sostituito gli usi con le shift
  for (Instruction *I : ToErase)
  {
    I->eraseFromParent();
  }

  return transformed;
}

bool runOnFunction(Function &F)
{
  bool transformed = false;
  for (auto Iter = F.begin(); Iter != F.end(); ++Iter)
  {
    if (runOnBasicBlock(*Iter))
    {
      transformed = true;
    }
  }
  return transformed;
}

namespace
{
  // dentro namespace mettiamo le altre struct, quindi implmentiamo gli altri passi
  // Discorso per il futuro! quando i passi funzionano singolarmente e vogliamo metterli su singolarmente

  // tutte le funzioni, come detto nel readme, come runOn..., andranno dentro la struct del relativo passo! ora le mettiamo addirittura
  // fuori dal namespace, ma in futuro andranno dentro la struct del passo

  // New PM implementation
  struct StrRed : PassInfoMixin<StrRed>
  {
    // Main entry point, takes IR unit to run the pass on (&F) and the
    // corresponding pass manager (to be queried if need be)
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &)
    {

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
llvm::PassPluginLibraryInfo getTestPassPluginInfo()
{
  return {LLVM_PLUGIN_API_VERSION, "TestPass", LLVM_VERSION_STRING,
          [](PassBuilder &PB)
          {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>)
                {
                  if (Name == "Strenght-Reduction")
                  { // nome del passo che passeremo ad opt
                    FPM.addPass(StrRed());
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
llvmGetPassPluginInfo()
{
  return getTestPassPluginInfo();
}

/*
  in run che prende le funzioni che incontra nel codice LLVM, chiamiamo RunOnBasickBlock, che è la funzione che preso un oggetto Functions itera i basick block al suo interno

  in queta funzione, per ogni oggetto basic block iterato, si chiama RunOnBasicBlock, che passa il basic block corrente ed itera tutte le istruzioni al suo interno,
  facendo un'analisi di ogni istruzione grazie al casting
      - recupera per bene queste cose sugli appunti, e ovviamente integra con il codice LLVM e mostra il perche la situazione non funziona nel caso di
        Loop.ll
*/