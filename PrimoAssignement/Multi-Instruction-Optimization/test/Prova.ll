
; Main function with simple strength reduction examples
define i32 @main() {
entry:
    ; Value to operate on
    %val = add i32 10, 5    ; Initialize %val = 15

    ; Addizione seguita da sottrazione
    %add = add i32 %val, 3    ; Addition
    %sub = sub i32 %add, 3    ; Subtraction, mi aspetto che venga eliminata
    
    
    ; usi della subtraction cge mi aspetto vengano rimpiazati con %val
    %mul = mul i32 %sub, 15   ; lei che usa la sub, mi aspetto che utilizzerà direttamente %val
    
    ; sottrazione seguita da addizione
    %var1 = sub i32 %val, 3    ; Subtraction
    %var2 = add i32 3, %var1  ; Addition, mi aspetto che venga eliminata

    ; usi della addition che mi aspetto vengano rimpiazati con %val
    %var4 = mul i32 %var2, 15   ; lei che usa la prima add, mi aspetto che utilizzi direttamente %val
    %var5 = mul i32 %var2, 4   ; idem questa qui
    
    ; Combine results
    %result = add i32 %mul, %var5
    
    ret i32 %result
}