
; Main function with simple strength reduction examples
define i32 @main() {
entry:
    ; Value to operate on
    %val = add i32 10, 5    ; Initialize %val = 15

    %add = add i32 %val, 3    ; Addition
    %sub = sub i32 %add, 3    ; Subtraction
    
    
    ; Multiplication by power of 2 (can be replaced with shift left)
    %mul = mul i32 %sub, 15   ; Can be replaced with %val << 3
    
    ; Division by power of 2 (can be replaced with shift right)
    %div = sdiv i32 %val, 4  ; Can be replaced with %val >> 2
    %div2 = udiv i32 %val, 4 ; Can be replaced with %val >> 2
    
    ; Combine results
    %result = add i32 %mul, %div
    
    ret i32 %result
}