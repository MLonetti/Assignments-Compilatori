; ModuleID = 'test/definitive_test.ll'
source_filename = "test/test.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  %1 = alloca [10 x i32], align 16
  br label %2

2:                                                ; preds = %8, %0
  %.01 = phi i32 [ 0, %0 ], [ %9, %8 ]
  %3 = icmp slt i32 %.01, 5
  br i1 %3, label %4, label %10

4:                                                ; preds = %2
  %5 = mul nsw i32 %.01, 2
  %6 = sext i32 %.01 to i64
  %7 = getelementptr inbounds [10 x i32], ptr %1, i64 0, i64 %6
  store i32 %5, ptr %7, align 4
  br label %8

8:                                                ; preds = %4
  %9 = add nsw i32 %.01, 1
  br label %2, !llvm.loop !6

10:                                               ; preds = %2
  br label %11

11:                                               ; preds = %18, %10
  %.02 = phi i32 [ 0, %10 ], [ %19, %18 ]
  %12 = icmp slt i32 %.02, 5
  br i1 %12, label %13, label %20

13:                                               ; preds = %11
  %14 = add nsw i32 %.02, 1
  %15 = sext i32 %14 to i64
  %16 = getelementptr inbounds [10 x i32], ptr %1, i64 0, i64 %15
  %17 = load i32, ptr %16, align 4
  br label %18

18:                                               ; preds = %13
  %19 = add nsw i32 %.02, 1
  br label %11, !llvm.loop !8

20:                                               ; preds = %11
  br label %21

21:                                               ; preds = %27, %20
  %.03 = phi i32 [ 0, %20 ], [ %28, %27 ]
  %22 = icmp slt i32 %.03, 5
  br i1 %22, label %23, label %29

23:                                               ; preds = %21
  %24 = mul nsw i32 %.03, 2
  %25 = sext i32 %.03 to i64
  %26 = getelementptr inbounds [10 x i32], ptr %1, i64 0, i64 %25
  store i32 %24, ptr %26, align 4
  br label %27

27:                                               ; preds = %23
  %28 = add nsw i32 %.03, 1
  br label %21, !llvm.loop !9

29:                                               ; preds = %21
  br label %30

30:                                               ; preds = %36, %29
  %.0 = phi i32 [ 0, %29 ], [ %37, %36 ]
  %31 = icmp slt i32 %.0, 5
  br i1 %31, label %32, label %38

32:                                               ; preds = %30
  %33 = sext i32 %.0 to i64
  %34 = getelementptr inbounds [10 x i32], ptr %1, i64 0, i64 %33
  %35 = load i32, ptr %34, align 4
  br label %36

36:                                               ; preds = %32
  %37 = add nsw i32 %.0, 1
  br label %30, !llvm.loop !10

38:                                               ; preds = %30
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
!9 = distinct !{!9, !7}
!10 = distinct !{!10, !7}
