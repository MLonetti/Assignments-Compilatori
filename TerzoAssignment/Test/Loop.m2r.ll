
; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local i32 @main() #0 {
  br label %1

1:                                                ; preds = %14, %0
  %.01 = phi i32 [ 0, %0 ], [ %9, %14 ]
  %.0 = phi i32 [ 0, %0 ], [ %15, %14 ]
  %2 = icmp slt i32 %.0, 10
  br i1 %2, label %3, label %16

3:                                                ; preds = %1
  %4 = icmp slt i32 %.0, 5
  br i1 %4, label %5, label %8

5:                                                ; preds = %3
  %6 = mul nsw i32 %.0, 2
  %7 = add nsw i32 %.01, %.0
  br label %16

8:                                                ; preds = %3
  %9 = add nsw i32 10, 5
  %10 = icmp sgt i32 %.0, 5
  br i1 %10, label %11, label %13

11:                                               ; preds = %8
  %12 = add nsw i32 %9, 1
  br label %16

13:                                               ; preds = %8
  br label %14

14:                                               ; preds = %13
  %15 = add nsw i32 %.0, 1
  br label %1, !llvm.loop !6

16:                                               ; preds = %11, %5, %1
  %.02 = phi i32 [ %7, %5 ], [ 0, %11 ], [ 0, %1 ]
  %.1 = phi i32 [ %.01, %5 ], [ %9, %11 ], [ %.01, %1 ]
  %17 = add nsw i32 12, %.1
  ret i32 %.02
}

attributes #0 = { noinline nounwind sspstrong uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"clang version 19.1.7"}
!6 = distinct !{!6, !7}
!7 = !{!"llvm.loop.mustprogress"}