; ModuleID = 'jokey_module'
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"


; Declaração de printf
declare i32 @printf(i8*, ...)

; Strings de formato
@.str_fmt_d = private unnamed_addr constant [3 x i8] c"%d\00"
@.str_fmt_f = private unnamed_addr constant [3 x i8] c"%f\00"
@.str_fmt_s = private unnamed_addr constant [3 x i8] c"%s\00"

@.str_nl = private unnamed_addr constant [2 x i8] c"\0A\00"

@.str0 = private unnamed_addr constant [20 x i8] c"Jokey Showcase v1.0\00"
@.str1 = private unnamed_addr constant [41 x i8] c"----------------------------------------\00"
@.str2 = private unnamed_addr constant [22 x i8] c">>> Executando teste:\00"
@.str3 = private unnamed_addr constant [25 x i8] c"Analise de Array e Float\00"
@.str4 = private unnamed_addr constant [21 x i8] c"Valores processados:\00"
@.str5 = private unnamed_addr constant [18 x i8] c"Soma Total (Int):\00"
@.str6 = private unnamed_addr constant [25 x i8] c"Media Calculada (Float):\00"
@.str7 = private unnamed_addr constant [25 x i8] c"Repeat Until (Countdown)\00"
@.str8 = private unnamed_addr constant [11 x i8] c"Decolagem!\00"
@.str9 = private unnamed_addr constant [18 x i8] c"Fibbonaci de 10: \00"
@.str10 = private unnamed_addr constant [22 x i8] c"Verificacao de Primos\00"
@.str11 = private unnamed_addr constant [39 x i8] c"17 \C3\A9 um primo maior que 10. (SUCESSO)\00"
@.str12 = private unnamed_addr constant [26 x i8] c"Falha na logica booleana.\00"
@.str13 = private unnamed_addr constant [17 x i8] c"Sistema Inativo.\00"
@.str14 = private unnamed_addr constant [28 x i8] c"Total de testes realizados:\00"
@.str15 = private unnamed_addr constant [24 x i8] c"=== FIM DA EXECUCAO ===\00"

@appName = global i8* null
@totalTestes = global i32 0
@sistemaAtivo = global i1 1

define void @separador() {
entry:
  %0 = getelementptr inbounds [41 x i8], [41 x i8]* @.str1, i64 0, i64 0
  %1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str_fmt_s, i32 0, i32 0), i8* %0)
  %2 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str_nl, i32 0, i32 0))
  ret void
}

define void @registrarTeste(i8* %0) {
entry:
  %1 = alloca i8*
  store i8* %0, i8** %1
  %2 = load i32, i32* @totalTestes
  %3 = add i32 %2, 1
  store i32 %3, i32* @totalTestes
  %4 = getelementptr inbounds [22 x i8], [22 x i8]* @.str2, i64 0, i64 0
  %5 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str_fmt_s, i32 0, i32 0), i8* %4)
  %6 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str_nl, i32 0, i32 0))
  %7 = load i8*, i8** %1
  %8 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str_fmt_s, i32 0, i32 0), i8* %7)
  %9 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str_nl, i32 0, i32 0))
  ret void
}

define i32 @fibonacci(i32 %0) {
entry:
  %1 = alloca i32
  store i32 %0, i32* %1
  %2 = load i32, i32* %1
  %3 = icmp sle i32 %2, 1
  br i1 %3, label %label1, label %label2

label1:
  ; Generating Return
  %4 = load i32, i32* %1
  ret i32 %4

label2:
  ; Generating Return
  %5 = load i32, i32* %1
  %6 = sub i32 %5, 1
  %7 = call i32 @fibonacci(i32 %6)
  %8 = load i32, i32* %1
  %9 = sub i32 %8, 2
  %10 = call i32 @fibonacci(i32 %9)
  %11 = add i32 %7, %10
  ret i32 %11

label0:
  ret i32 0
}

define i1 @ehPrimo(i32 %0) {
entry:
  %1 = alloca i32
  store i32 %0, i32* %1
  %2 = load i32, i32* %1
  %3 = icmp sle i32 %2, 1
  br i1 %3, label %label4, label %label5

label4:
  ; Generating Return
  ret i1 0

label5:
  br label %label3

label3:
  ; Generating VarDecl: i
  %4 = alloca i32
  store i32 2, i32* %4
  br label %label6

label6:
  %5 = load i32, i32* %4
  %6 = load i32, i32* %1
  %7 = sdiv i32 %6, 2
  %8 = icmp sle i32 %5, %7
  br i1 %8, label %label7, label %label8

label7:
  %9 = load i32, i32* %1
  %10 = load i32, i32* %4
  %11 = srem i32 %9, %10
  %12 = icmp eq i32 %11, 0
  br i1 %12, label %label10, label %label11

label10:
  ; Generating Return
  ret i1 0

label11:
  br label %label9

label9:
  %13 = load i32, i32* %4
  %14 = add i32 %13, 1
  store i32 %14, i32* %4
  br label %label6

label8:
  ; Generating Return
  ret i1 1
  ret i1 0
}

define void @analisarDados() {
entry:
  %0 = getelementptr inbounds [25 x i8], [25 x i8]* @.str3, i64 0, i64 0
  call void @registrarTeste(i8* %0)
  ; Generating VarDecl: valores
  %1 = alloca i32*
  %2 = alloca i32, i32 5
  %3 = getelementptr i32, i32* %2, i32 0
  store i32 10, i32* %3
  %4 = getelementptr i32, i32* %2, i32 1
  store i32 25, i32* %4
  %5 = getelementptr i32, i32* %2, i32 2
  store i32 30, i32* %5
  %6 = getelementptr i32, i32* %2, i32 3
  store i32 45, i32* %6
  %7 = getelementptr i32, i32* %2, i32 4
  store i32 50, i32* %7
  store i32* %2, i32** %1
  ; Generating VarDecl: soma
  %8 = alloca i32
  store i32 0, i32* %8
  ; Generating VarDecl: contador
  %9 = alloca i32
  store i32 0, i32* %9
  %10 = getelementptr inbounds [21 x i8], [21 x i8]* @.str4, i64 0, i64 0
  %11 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str_fmt_s, i32 0, i32 0), i8* %10)
  %12 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str_nl, i32 0, i32 0))
  %13 = load i32*, i32** %1
  %14 = alloca i32
  store i32 0, i32* %14
  %15 = alloca i32
  br label %label12

label12:
  %16 = load i32, i32* %14
  %17 = icmp slt i32 %16, 5
  br i1 %17, label %label13, label %label14

label13:
  %18 = getelementptr i32, i32* %13, i32 %16
  %19 = load i32, i32* %18
  store i32 %19, i32* %15
  %20 = load i32, i32* %15
  %21 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str_fmt_d, i32 0, i32 0), i32 %20)
  %22 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str_nl, i32 0, i32 0))
  %23 = load i32, i32* %8
  %24 = load i32, i32* %15
  %25 = add i32 %23, %24
  store i32 %25, i32* %8
  %26 = load i32, i32* %9
  %27 = add i32 %26, 1
  store i32 %27, i32* %9
  %28 = load i32, i32* %14
  %29 = add i32 %28, 1
  store i32 %29, i32* %14
  br label %label12

label14:
  ; Generating VarDecl: media
  %30 = alloca double
  %31 = load i32, i32* %8
  %32 = sitofp i32 %31 to double
  %33 = fdiv double %32, 5.000000e+00
  store double %33, double* %30
  %34 = getelementptr inbounds [18 x i8], [18 x i8]* @.str5, i64 0, i64 0
  %35 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str_fmt_s, i32 0, i32 0), i8* %34)
  %36 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str_nl, i32 0, i32 0))
  %37 = load i32, i32* %8
  %38 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str_fmt_d, i32 0, i32 0), i32 %37)
  %39 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str_nl, i32 0, i32 0))
  %40 = getelementptr inbounds [25 x i8], [25 x i8]* @.str6, i64 0, i64 0
  %41 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str_fmt_s, i32 0, i32 0), i8* %40)
  %42 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str_nl, i32 0, i32 0))
  %43 = load double, double* %30
  %44 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str_fmt_f, i32 0, i32 0), double %43)
  %45 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str_nl, i32 0, i32 0))
  ret void
}

define void @testeLoopReverso() {
entry:
  %0 = getelementptr inbounds [25 x i8], [25 x i8]* @.str7, i64 0, i64 0
  call void @registrarTeste(i8* %0)
  ; Generating VarDecl: count
  %1 = alloca i32
  store i32 3, i32* %1
  br label %label15

label15:
  %2 = load i32, i32* %1
  %3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str_fmt_d, i32 0, i32 0), i32 %2)
  %4 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str_nl, i32 0, i32 0))
  %5 = load i32, i32* %1
  %6 = sub i32 %5, 1
  store i32 %6, i32* %1
  br label %label16

label16:
  %7 = load i32, i32* %1
  %8 = icmp eq i32 %7, 0
  br i1 %8, label %label17, label %label15

label17:
  %9 = getelementptr inbounds [11 x i8], [11 x i8]* @.str8, i64 0, i64 0
  %10 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str_fmt_s, i32 0, i32 0), i8* %9)
  %11 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str_nl, i32 0, i32 0))
  ret void
}

define i32 @main() {
entry:
  %0 = getelementptr inbounds [20 x i8], [20 x i8]* @.str0, i64 0, i64 0
  store i8* %0, i8** @appName
  %1 = load i8*, i8** @appName
  %2 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str_fmt_s, i32 0, i32 0), i8* %1)
  %3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str_nl, i32 0, i32 0))
  call void @separador()
  %4 = load i1, i1* @sistemaAtivo
  br i1 %4, label %label19, label %label20

label19:
  ; Generating VarDecl: fib10
  %5 = alloca i32
  %6 = call i32 @fibonacci(i32 10)
  store i32 %6, i32* %5
  %7 = getelementptr inbounds [18 x i8], [18 x i8]* @.str9, i64 0, i64 0
  %8 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str_fmt_s, i32 0, i32 0), i8* %7)
  %9 = load i32, i32* %5
  %10 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str_fmt_d, i32 0, i32 0), i32 %9)
  %11 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str_nl, i32 0, i32 0))
  call void @separador()
  %12 = getelementptr inbounds [22 x i8], [22 x i8]* @.str10, i64 0, i64 0
  call void @registrarTeste(i8* %12)
  ; Generating VarDecl: num
  %13 = alloca i32
  store i32 17, i32* %13
  %14 = load i32, i32* %13
  %15 = call i1 @ehPrimo(i32 %14)
  %16 = load i32, i32* %13
  %17 = icmp sgt i32 %16, 10
  %18 = and i1 %15, %17
  br i1 %18, label %label22, label %label23

label22:
  %19 = getelementptr inbounds [39 x i8], [39 x i8]* @.str11, i64 0, i64 0
  %20 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str_fmt_s, i32 0, i32 0), i8* %19)
  %21 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str_nl, i32 0, i32 0))
  br label %label21

label23:
  %22 = getelementptr inbounds [26 x i8], [26 x i8]* @.str12, i64 0, i64 0
  %23 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str_fmt_s, i32 0, i32 0), i8* %22)
  %24 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str_nl, i32 0, i32 0))
  br label %label21

label21:
  call void @separador()
  call void @analisarDados()
  call void @separador()
  call void @testeLoopReverso()
  call void @separador()
  br label %label18

label20:
  %25 = getelementptr inbounds [17 x i8], [17 x i8]* @.str13, i64 0, i64 0
  %26 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str_fmt_s, i32 0, i32 0), i8* %25)
  %27 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str_nl, i32 0, i32 0))
  br label %label18

label18:
  %28 = getelementptr inbounds [28 x i8], [28 x i8]* @.str14, i64 0, i64 0
  %29 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str_fmt_s, i32 0, i32 0), i8* %28)
  %30 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str_nl, i32 0, i32 0))
  %31 = load i32, i32* @totalTestes
  %32 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str_fmt_d, i32 0, i32 0), i32 %31)
  %33 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str_nl, i32 0, i32 0))
  %34 = getelementptr inbounds [24 x i8], [24 x i8]* @.str15, i64 0, i64 0
  %35 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str_fmt_s, i32 0, i32 0), i8* %34)
  %36 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str_nl, i32 0, i32 0))
  ret i32 0
}
