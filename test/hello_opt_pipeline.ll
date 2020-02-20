; REQUIRES: opt-pipeline

; RUN: opt -mtriple=x86_64-- -load %shlibdir/libHelloWorld%shlibext -O1 -debug-pass=Structure  %s -o /dev/null 2>&1 | FileCheck --check-prefixes=CHECK-O1 %s
; RUN: opt -mtriple=x86_64-- -load %shlibdir/libHelloWorld%shlibext -O2 -debug-pass=Structure  %s -o /dev/null 2>&1 | FileCheck --check-prefixes=CHECK-O2 %s
; RUN: opt -mtriple=x86_64-- -load %shlibdir/libHelloWorld%shlibext -O3 -debug-pass=Structure  %s -o /dev/null 2>&1 | FileCheck --check-prefixes=CHECK-O3 %s
; RUN: opt -mtriple=x86_64-- -load %shlibdir/libHelloWorld%shlibext -Os -debug-pass=Structure  %s -o /dev/null 2>&1 | FileCheck --check-prefixes=CHECK-Os %s

; This test verifies that HelloWorld is run when an optimisation pipeline is
; requested through -O{0,1,2,3,s}. Note that contrary to other tests, there's no
; explicit request to run HelloWorld (through --legacy-hello-world or
; -passes=hello-world). Instead, HelloWorld is requested implicitly by specifying
; an optimisation level.

; CHECK-O1: FunctionPass Manager
; CHECK-O1: Hello World Pass

; CHECK-O2: FunctionPass Manager
; CHECK-O2: Hello World Pass

; CHECK-O3: FunctionPass Manager
; CHECK-O3: Hello World Pass

; CHECK-Os: FunctionPass Manager
; CHECK-Os: Hello World Pass

define i32 @main(i32, i8** nocapture readnone) {
  ret i32 1
}
