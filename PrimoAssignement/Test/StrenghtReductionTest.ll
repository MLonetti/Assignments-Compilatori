; 2) Strenght Reduction

define i32 @Strenght_Reduction(i32 noundef %0, i32 noundef %1) #0 {
  
  ;caso shift normale:
  %3 = mul nsw i32 %0, 16 ; mi aspetto venga creata una shift al posto di questa mul, e chi userà questa mul utilizzerà la shift!
  %4 = add nsw i32 %3, 10 ; mi aspetto questa istruzione utilizzi la shift creata precedentemente al posto della mul

  ; caso con shift del tipo x * 15
  %5 = mul i32 %1, 15 ; mi aspetto venga creata una shift left di 4 al posto di questa mul e subito dopo mi aspetto una sottrazione del tipo -> %.. = shift - x

  ; caso con shift del tipo x * 17
  %6 = mul i32 %0 , 17 ; mi aspetto venga creata una shift left di 4 al posto di questa mul e subito dopo mi aspetto una somma del tipo -> %.. = shift + x 

  ; caso con divisione del tipo x / 2
  %7 = sdiv i32 %0, 2 ; mi aspetto venga creata una shift right al posto di questa div

  ; caso con divisione del tipo x / 4
  %8 = sdiv i32 %1, 4 ; mi aspetto venga creata una shift right di 2 al posto di questa div

  ret i32 %6
}