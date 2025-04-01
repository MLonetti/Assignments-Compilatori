; Modulo Test.ll

; All'interno di questo codice, abbiamo i 3 scenari da ottimizzare:

; 1) Algebraic Identity:
define dso_local i32 @Algebraic_Identity(i32 noundef %0, i32 noundef %1) #0 {
  %3 = add nsw i32 %0, 3

  %4 = add nsw i32 %3, 0
  %5 = mul nsw i32 %4, 3

  %6 = mul i32 %5, 1
  %7 = add i32 %5, 6

  %8 = mul nsw i32 %5, %6
 
  %9 = sdiv i32 %4, 1 
  %10 = sub i32 %5, 0

  ret i32 %1
}


; 2) Strenght Reduction

define i32 @Strenght_Reduction(i32 noundef %0, i32 noundef %1) #0 {
  
  ;caso shift normale:
  %3 = mul nsw i32 %0, 16 ; mi aspetto venga creata una shift al posto di questa mul, e chi userà questa mul utilizzerà la shift!
  %4 = add nsw i32 %3, 10 ; mi aspetto questa istruzione utilizzi la shift creata precedentemente al posto della mul

  ; caso con shift del tipo x * 15
  %5 = mul i32 %1, 15 ; mi aspetto venga creata una shift left di 4 al posto di questa mul e subito dopo mi aspetto una sottrazione del tipo -> %.. = shift - x

  ; caso con shift del tipo x * 17
  %6 = mul i32 %0 , 17 ; mi aspetto venga creata una shift left di 4 al posto di questa mul e subito dopo mi aspetto una somma del tipo -> %.. = shift + x 

  ; caso con divisione del tipo x / 2
  %7 = sdiv i32 %0, 2 ; mi aspetto venga creata una shift right al posto di questa div

  ; caso con divisione del tipo x / 4
  %8 = sdiv i32 %1, 4 ; mi aspetto venga creata una shift right di 2 al posto di questa div

  ret i32 %6
}



; 3) Multi Instruction Operation:

define i32 @Multi_Instruction_Operation() {
  ; Value to operate on
  %val = add i32 10, 5    ; Initialize %val = 15

  ; Addizione seguita da sottrazione
  %add = add i32 %val, 3    ; Addition
  %sub = sub i32 %add, 3    ; Subtraction, mi aspetto che venga eliminata
  
  %sub2 = sub i32 %add, 3; deve essere eliminata anche questa, dato ch3e rispecchia la prima add trovata
  
  ; usi della subtraction cge mi aspetto vengano rimpiazati con %val
  %mul = mul i32 %sub, 15   ; lei che usa la sub, mi aspetto che utilizzerà direttamente %val
  
  ; sottrazione seguita da addizione
  %var1 = sub i32 %val, 3    ; Subtraction
  %var2 = add i32 3, %var1  ; Addition, mi aspetto che venga eliminata
  %var3 = add i32 %var1, 3 ;addition che deve essere anche questa rimossa, sia perché gestiamo la commutatività
                          ; sia perché controlliamo tutte le add dopo la suib che abbiamo catturato inizialmente

  ; usi della addition che mi aspetto vengano rimpiazati con %val
  %var4 = mul i32 %var2, 15   ; lei che usa la prima add, mi aspetto che utilizzi direttamente %val
  %var5 = mul i32 %var2, 4   ; idem questa qui
  
  ; Combine results
  %result = add i32 %mul, %var5
  
  ret i32 %result
}