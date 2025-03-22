
define dso_local i32 @prova(i32 noundef %0, i32 noundef %1) #0 {
  
  %3 = add nsw i32 %0, 0

  %4 = mul nsw i32 %1, 1

  %5 = add i32 %3, 4

  %6 = add i32 %4, 6

  %7 = mul nsw i32 %5, %6

  ret i32 %7
}

