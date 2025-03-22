; ModuleID = 'Prova.Optimized.bc'
source_filename = "test/Prova.ll"

define dso_local i32 @prova(i32 noundef %0, i32 noundef %1) {
  %3 = add i32 %0, 4
  %4 = add i32 %1, 6
  %5 = mul nsw i32 %3, %4
  ret i32 %5
}
