; ModuleID = 'testLoopName.ll'
source_filename = "testLoopName.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local i32 @main() #0 {
  %1 = alloca [20 x i32], align 16
  br label %2

2:                                                ; preds = %8, %0
  %.02 = phi i32 [ 0, %0 ], [ %9, %8 ]
  %3 = icmp slt i32 %.02, 10
  br i1 %3, label %4, label %10

4:                                                ; preds = %2
  %5 = mul nsw i32 %.02, 2
  %6 = sext i32 %.02 to i64
  %7 = getelementptr inbounds [20 x i32], ptr %1, i64 0, i64 %6
  store i32 %5, ptr %7, align 4
  br label %8

8:                                                ; preds = %4
  %9 = add nsw i32 %.02, 1
  br label %2, !llvm.loop !6

10:                                               ; preds = %2
  br label %11

11:                                               ; preds = %27, %10
  %.03 = phi i32 [ 0, %10 ], [ %28, %27 ]
  %12 = icmp slt i32 %.03, 10
  br i1 %12, label %13, label %29

13:                                               ; preds = %11
  %14 = add nsw i32 %.03, 1
  %15 = sext i32 %14 to i64
  %16 = getelementptr inbounds [20 x i32], ptr %1, i64 0, i64 %15
  %17 = load i32, ptr %16, align 4
  br label %18

18:                                               ; preds = %24, %13
  %.04 = phi i32 [ 0, %13 ], [ %25, %24 ]
  %19 = icmp slt i32 %.04, 10
  br i1 %19, label %20, label %26

20:                                               ; preds = %18
  %21 = sext i32 %.04 to i64
  %22 = getelementptr inbounds [20 x i32], ptr %1, i64 0, i64 %21
  %23 = load i32, ptr %22, align 4
  br label %24

24:                                               ; preds = %20
  %25 = add nsw i32 %.04, 1
  br label %18, !llvm.loop !8

26:                                               ; preds = %18
  br label %27

27:                                               ; preds = %26
  %28 = add nsw i32 %.03, 1
  br label %11, !llvm.loop !9

29:                                               ; preds = %11
  br label %30

30:                                               ; preds = %36, %29
  %.01 = phi i32 [ 0, %29 ], [ %37, %36 ]
  %31 = icmp slt i32 %.01, 20
  br i1 %31, label %32, label %38

32:                                               ; preds = %30
  %33 = sext i32 %.01 to i64
  %34 = getelementptr inbounds [20 x i32], ptr %1, i64 0, i64 %33
  %35 = load i32, ptr %34, align 4
  br label %36

36:                                               ; preds = %32
  %37 = add nsw i32 %.01, 1
  br label %30, !llvm.loop !10

38:                                               ; preds = %30
  br label %39

39:                                               ; preds = %48, %38
  %.0 = phi i32 [ 0, %38 ], [ %49, %48 ]
  %40 = icmp slt i32 %.0, 5
  br i1 %40, label %41, label %50

41:                                               ; preds = %39
  %42 = add nsw i32 %.0, 1
  %43 = sext i32 %42 to i64
  %44 = getelementptr inbounds [20 x i32], ptr %1, i64 0, i64 %43
  %45 = load i32, ptr %44, align 4
  %46 = sext i32 %.0 to i64
  %47 = getelementptr inbounds [20 x i32], ptr %1, i64 0, i64 %46
  store i32 %45, ptr %47, align 4
  br label %48

48:                                               ; preds = %41
  %49 = add nsw i32 %.0, 1
  br label %39, !llvm.loop !11

50:                                               ; preds = %39
  ret i32 0
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
!8 = distinct !{!8, !7}
!9 = distinct !{!9, !7}
!10 = distinct !{!10, !7}
!11 = distinct !{!11, !7}