; ModuleID = 'test/Prova.ll'
source_filename = "test/Prova.ll"

define i32 @main() {
entry:
  %val = add i32 10, 5
  %add = add i32 %val, 3
  %mul = mul i32 %val, 15
  %div = sdiv i32 %val, 4
  %div2 = udiv i32 %val, 4
  %result = add i32 %mul, %div
  ret i32 %result
}
