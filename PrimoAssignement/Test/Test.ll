; Modulo Test.ll

; All'interno di questo codice, abbiamo i 3 scenari da ottimizzare:

; 1) Algebraic Identity:

define dso_local i32 @Algebraic_Identity(i32 noundef %0, i32 noundef %1) #0 {
  
  %3 = add nsw i32 %0, 0 ; dovrà essere cancellata! e chi usa %3, dovrà utilizzare direttamente %0
  %4 = mul nsw i32 %1, 1 ; dovrà essere cancellata! e chi usa %4, dovrà utilizzare direttamente %1

; usi delle istruzioni precedenti
  %5 = add i32 %3, 4
  %6 = add i32 %4, 6


  %8 = sdiv i32 %5, 1 ; dovrà essere cancellata e chi usa %8, dovrà utilizzare direttamente %4 (quindi %1)
  %9 = sub i32 %6, 0 ; dovrà essere cancellata, e chi usa %9, dovrà utilizzare direttamente %5

; usi delle istruzioni precedenti
  %10 = add i32 %8, 10
  %11 = add i32 %9, 3

  %ret = add i32 1, 1
  ret i32 %ret
}





; 2) Strenght Reduction





; 3) Multi Instruction Operation: