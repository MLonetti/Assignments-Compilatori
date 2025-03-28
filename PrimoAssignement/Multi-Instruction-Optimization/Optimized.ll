; ModuleID = 'test/Prova.ll'
source_filename = "test/Prova.ll"

define i32 @main() {
entry:
  %val = add i32 10, 5
  %add = add i32 %val, 3
  %mul = mul i32 %val, 15
  %var1 = sub i32 %val, 3
  %var4 = mul i32 %val, 15
  %var5 = mul i32 %val, 4
  %result = add i32 %mul, %var5
  ret i32 %result
}
