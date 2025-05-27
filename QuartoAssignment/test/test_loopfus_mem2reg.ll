; ModuleID = 'test/test_loopfus.ll'
source_filename = "test/test.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  %1 = alloca [11 x i32], align 16
  br label %2

2:                                                ; preds = %8, %0
  %.01 = phi i32 [ 0, %0 ], [ %9, %8 ]
  %3 = icmp slt i32 %.01, 10
  br i1 %3, label %4, label %10

4:                                                ; preds = %2
  %5 = mul nsw i32 %.01, 2
  %6 = sext i32 %.01 to i64
  %7 = getelementptr inbounds [11 x i32], ptr %1, i64 0, i64 %6
  store i32 %5, ptr %7, align 4
  br label %8

8:                                                ; preds = %4
  %9 = add nsw i32 %.01, 1
  br label %2, !llvm.loop !6

10:                                               ; preds = %2
  br label %11

11:                                               ; preds = %18, %10
  %.0 = phi i32 [ 0, %10 ], [ %19, %18 ]
  %12 = icmp slt i32 %.0, 10
  br i1 %12, label %13, label %20

13:                                               ; preds = %11
  %14 = add nsw i32 %.0, 1
  %15 = sext i32 %14 to i64
  %16 = getelementptr inbounds [11 x i32], ptr %1, i64 0, i64 %15
  %17 = load i32, ptr %16, align 4
  br label %18

18:                                               ; preds = %13
  %19 = add nsw i32 %.0, 1
  br label %11, !llvm.loop !8

20:                                               ; preds = %11
  ret i32 0
}

attributes #0 = { noinline nounwind uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"clang version 19.1.7 (/home/runner/work/llvm-project/llvm-project/clang cd708029e0b2869e80abe31ddb175f7c35361f90)"}
!6 = distinct !{!6, !7}
!7 = !{!"llvm.loop.mustprogress"}
!8 = distinct !{!8, !7}
