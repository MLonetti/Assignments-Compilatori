
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {

  /*
  ====================================================================================================================================================

  PASSO DI OTTIMIZZAZIONE 1 => Algebraic Identity

  ====================================================================================================================================================
  */

  struct AlgIde : PassInfoMixin<AlgIde>
  {
    bool is_mul(Instruction &I)
    {
      if (auto *BO = dyn_cast<BinaryOperator>(&I))
      {
        if (BO->getOpcode() == Instruction::Mul)
        {
          return true;
        }
      }
      return false;
    }

    bool is_add(Instruction &I)
    {
      if (auto *BO = dyn_cast<BinaryOperator>(&I))
      {
        if (BO->getOpcode() == Instruction::Add)
        {
          return true;
        }
      }
      return false;
    }

    bool is_sub(Instruction &I)
    {
      if (auto *BO = dyn_cast<BinaryOperator>(&I))
      {
        if (BO->getOpcode() == Instruction::Sub)
        {
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

    bool runOnBasicBlock(BasicBlock &BB)
    {
      bool transformed = false;
      std::vector<Instruction *> toErase; // vettore di istruzioni da eliminare

      for (Instruction &Inst : BB)
      { // iteriamo le istruzioni all'interno del basicblock passato

        int i = 0; // ogni istruzione iterata, il contatore viene ridefinito a 0.

        outs() << "Itero l'istruzione: " << Inst << "\n";

        if (is_mul(Inst))
        {
          // L'istruzione è una Mul
          // possiamo procedere ad iterare gli operandi, sfruttiamo la lista op dell'istruzione, che contiene appunto gli operandi
          // per ogni operando guardiamo se si tratta di un costantInt, e se lo è vediamo che sia l'elemento neutro.

          for (auto *Iter = Inst.op_begin(); Iter != Inst.op_end(); ++Iter)
          {
            Value *Operand = *Iter;

            if (ConstantInt *CI = dyn_cast<ConstantInt>(Operand))
            {
              int Val_Operand = CI->getSExtValue();

              if (Val_Operand == 1)
              {
                outs() << "\tTrovata istruzione Mul con elemento neutro!" << "\n";
                Value *operando_non_neutrale = Inst.getOperand(1 - i);
                // la i può essere 0 oppure 1
                // se i = 0, vuol dire che l'elemento neutrale è il primo operando, perciò l'altro operando sarà
                // Inst.getOperand(1-0) che è appunto il secondo

                // se la i fosse 1, invece avremmo che l'elemento neutro sarà il secondo parametro, perciò
                // andiamo a prendere il primo parametro non neutrale con Inst.getOperand(1-1)

                // sfruttiamo il fatto che sappiamo di avere Operand 0 ed Operand 1 e puntiamo ad uno dei due così.

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
            i += 1;
          }
        }
        else if (is_div(Inst))
        {
          if (ConstantInt *ROperand = dyn_cast<ConstantInt>(Inst.getOperand(1)))
          {
            int Val_Operand = ROperand->getSExtValue();
            if (Val_Operand == 1)
            {
              outs() << "\tTrovata istruzione Div con elemento neutro!" << "\n";
              Value *operando_non_neutrale = Inst.getOperand(0);

              outs() << "\tTutte le istruzioni che usavano: " << Inst << " utilizzeranno ora: " << *operando_non_neutrale << "\n\n";
              Inst.replaceAllUsesWith(operando_non_neutrale);

              toErase.push_back(&Inst);
              transformed = true;
            }
          }
        }
        else if (is_add(Inst))
        {
          // si ripete lo stesso procedimento di tutto ciò che abbiamo visto, se l'istruzione iterata è una add
          // iteriamo quindi gli operandi come prima e guardiamo quale è l'elemento neutro
          for (auto *Iter = Inst.op_begin(); Iter != Inst.op_end(); ++Iter)
          {
            Value *Operand = *Iter;

            if (ConstantInt *CI = dyn_cast<ConstantInt>(Operand))
            {
              int Val_Operand = CI->getSExtValue();

              if (Val_Operand == 0)
              {
                outs() << "\tTrovata istruzione Add con elemento neutro!" << "\n";
                Value *operando_non_neutrale = Inst.getOperand(1 - i);

                outs() << "\tTutte le istruzioni che usavano: " << Inst << " utilizzeranno ora: " << *operando_non_neutrale << "\n\n";
                Inst.replaceAllUsesWith(operando_non_neutrale);

                toErase.push_back(&Inst);
                transformed = true;

                break;
              }
            }
            i += 1;
          }
        }
        else if (is_sub(Inst))
        {
          if (ConstantInt *ROperand = dyn_cast<ConstantInt>(Inst.getOperand(1)))
          {
            int Val_Operand = ROperand->getSExtValue();
            if (Val_Operand == 0)
            {
              outs() << "\tTrovata istruzione Sub con elemento neutro!" << "\n";
              Value *operando_non_neutrale = Inst.getOperand(0);

              outs() << "\tTutte le istruzioni che usavano: " << Inst << " utilizzeranno ora: " << *operando_non_neutrale << "\n\n";
              Inst.replaceAllUsesWith(operando_non_neutrale);

              toErase.push_back(&Inst); // Aggiungiamo l'istruzione alla lista di eliminazione
              transformed = true;
            }
          }
        }
      }

      // cancelliamo le istruzioni dopo averle iterate tutte, per evitare probleimi di iterazione
      outs() << "\nHo eliminato le seguenti istruzioni: \n";
      for (Instruction *I : toErase)
      {
        outs() << *I << "\n";
        I->eraseFromParent();
      }

      return transformed;
    }

    bool runOnFunction(Function &F)
    {
      // Iteriamo su tutti i BasicBlock della funzione
      bool transformed = false;

      for (BasicBlock &BB : F)
      {
        if (runOnBasicBlock(BB))
        {
          transformed = true;
        }
      }

      return transformed;
    }



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


  /*
  ====================================================================================================================================================

  PASSO DI OTTIMIZZAZIONE 2 => Strenght Reduction

  ====================================================================================================================================================
  */

  struct StrRed : PassInfoMixin<StrRed>
  {
    
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

                BinaryOperator *NewSub = BinaryOperator::Create(Instruction::Sub, NewShift, ConstantInt::get(NewShift->getType(), 1));
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

                BinaryOperator *NewAdd = BinaryOperator::Create(Instruction::Add, NewShift, ConstantInt::get(NewShift->getType(), 1));
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

  /*
  ====================================================================================================================================================

  PASSO DI OTTIMIZZAZIONE 3 => Multi Instruction Operation

  ====================================================================================================================================================
  */

  struct MultInstrOpt: PassInfoMixin<MultInstrOpt> {
    
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
    
    
    bool runOnBasicBlock(BasicBlock &B) {
      bool transformed = false;
      std::vector<Instruction *> ToErase; //vettore che conterrà le istruzioni da eliminare
    
      for(Instruction &I : B){
        outs() << "Instruction: " << I << "\n";
        
    
        if (is_add(I)){
          int i = 0;
          for (auto *Iter = I.op_begin(); Iter != I.op_end(); ++Iter) {
            Value *Op = *Iter;
            if (ConstantInt *CI = dyn_cast<ConstantInt>(Op)) {
              Value *otherOp = I.getOperand(1 - i);
    
              // Itera manualmente su tutte le istruzioni successive a I
              Instruction *NextInst = I.getNextNode();
              while (NextInst != nullptr){
                if (is_sub(*NextInst)){
                  Value *operando0 = NextInst->getOperand(0);
                  Value *operando1 = NextInst->getOperand(1);
    
                  if (operando0 == &I && operando1 == Op) { // Dove la Op è la costante intera
                    outs() << "Trovata sub ridondante!: " << *NextInst << "\n";
                    NextInst->replaceAllUsesWith(otherOp); // Sostituisco tutti gli usi della sub con l'add
    
                    ToErase.push_back(NextInst); // Aggiungo l'istruzione da eliminare al vettore
                    transformed = true;
                    outs() << "Istruzione eliminata!\n";
    
                    outs() << "Ora le istruzioni utilizzeranno direttamente l'operando dell'add! " << *otherOp << "\n";
                  }
        
                }
                NextInst = NextInst->getNextNode();
                // Passa all'istruzione successiva
              }  
            }
            i = i + 1;
          }
            
        }else if(is_sub(I)){
    
          Value *Operand0 = I.getOperand(0);
          Value *Operand1 = I.getOperand(1);
    
          if (ConstantInt *CI = dyn_cast<ConstantInt>(Operand1)) {
            // Itera manualmente su tutte le istruzioni successive a I
            Instruction *NextInst = I.getNextNode();
            while (NextInst != nullptr) {
              int i = 0; //ridefinisco la i ogni volta che trovo una add, per catturare correttamente gli operandi
              if (is_add(*NextInst)) {
                // Itera sugli operandi della add
                for (auto Iter = NextInst->op_begin(); Iter != NextInst->op_end(); ++Iter) {
                  Value *Op = *Iter;
                  if (ConstantInt *CI2 = dyn_cast<ConstantInt>(Op)) {
                    // Se l'operando è una costante intera, vediamo se le due istruzioni verificano le condizioni
                    Value *otherOp = NextInst->getOperand(1 - i);
    
                    if (otherOp == &I && Op == Operand1) { // Controlliamo che la costante della add sia uguale alla costante della sub
                      outs() << "Trovata add ridondante!: " << *NextInst << "\n";
                      NextInst->replaceAllUsesWith(Operand0); // Sostituisco tutti gli usi della add con il primo operando della sub
    
                      ToErase.push_back(NextInst); // Aggiungo l'istruzione da eliminare al vettore
                      transformed = true;
                      outs() << "Istruzione eliminata!\n";
    
                      outs() << "Ora le istruzioni utilizzeranno direttamente l'operando della sub! " << *Operand0 << "\n";
                    }
                  }
                  i = i + 1; // Incrementa l'indice per tracciare gli operandi
                }
              }
    
              // Passa all'istruzione successiva
              NextInst = NextInst->getNextNode();
            }
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
                  else if(Name == "Algebraic-Identity") {
                    FPM.addPass(AlgIde());
                    return true;
                  }
                  else if(Name == "Strenght-Reduction") {
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
llvmGetPassPluginInfo() {
  return getTestPassPluginInfo();
}
