; ModuleID = 'test/Prova.optimized.bc'
source_filename = "test/Prova.ll"

define dso_local i32 @prova(i32 noundef %0, i32 noundef %1) {
  %3 = add i32 %1, 6
  %4 = mul nsw i32 4, %3
  ret i32 4
}
