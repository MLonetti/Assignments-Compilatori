; ModuleID = 'test/Prova.ll'
source_filename = "test/Prova.ll"

define dso_local i32 @prova(i32 noundef %0, i32 noundef %1) {
  %3 = shl i32 %0, 4
  %4 = add nsw i32 %3, 10
  %5 = shl i32 %1, 4
  %6 = sub i32 %5, %1
  %7 = shl i32 %0, 4
  %8 = add i32 %7, %0
  %9 = lshr i32 %0, 1
  %10 = lshr i32 %1, 2
  ret i32 %7
}
