; ModuleID = 'jokey_module'
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"


; Declaração de printf
declare i32 @printf(i8*, ...)

; Strings de formato
@.str_fmt_d = private unnamed_addr constant [4 x i8] c"%d\0A\00"
@.str_fmt_f = private unnamed_addr constant [4 x i8] c"%f\0A\00"
@.str_fmt_s = private unnamed_addr constant [4 x i8] c"%s\0A\00"

@.str0 = private unnamed_addr constant [12 x i8] c"Hello World\00"


define i32 @main() {
entry:
  %0 = getelementptr inbounds [12 x i8], [12 x i8]* @.str0, i64 0, i64 0
  call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str_fmt_s, i32 0, i32 0), i8* %0)
  ret i32 0
}
