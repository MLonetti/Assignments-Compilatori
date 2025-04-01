
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

