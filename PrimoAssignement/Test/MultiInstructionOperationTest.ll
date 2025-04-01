
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