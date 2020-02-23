; REQUIRES: opt-pipeline

; RUN: opt -mtriple=x86_64-- -load %shlibdir/libOpcodeCounter%shlibext -O0 -debug-pass=Structure  %s -o /dev/null 2>&1 | FileCheck --check-prefixes=CHECK-O1 %s
; RUN: opt -mtriple=x86_64-- -load %shlibdir/libOpcodeCounter%shlibext -O1 -debug-pass=Structure  %s -o /dev/null 2>&1 | FileCheck --check-prefixes=CHECK-O1 %s
; RUN: opt -mtriple=x86_64-- -load %shlibdir/libOpcodeCounter%shlibext -O2 -debug-pass=Structure  %s -o /dev/null 2>&1 | FileCheck --check-prefixes=CHECK-O2 %s
; RUN: opt -mtriple=x86_64-- -load %shlibdir/libOpcodeCounter%shlibext -O3 -debug-pass=Structure  %s -o /dev/null 2>&1 | FileCheck --check-prefixes=CHECK-O3 %s
; RUN: opt -mtriple=x86_64-- -load %shlibdir/libOpcodeCounter%shlibext -Os -debug-pass=Structure  %s -o /dev/null 2>&1 | FileCheck --check-prefixes=CHECK-Os %s

; This test verifies that OpcodeCounter is run when an optimisation pipeline is
; requested through -O{0,1,2,3,s}. Note that contrary to other tests, there's
; no explicit request to run OpcodeCounter (through --legacy-opcode-counter or
; -passes=opcode-counter). Instead, OpcodeCounter is requested implicitly by
; specifying an optimisation level.

; CHECK-O1: FunctionPass Manager
; CHECK-O1: OpcodeCounter Pass

; CHECK-O2: FunctionPass Manager
; CHECK-O2: OpcodeCounter Pass

; CHECK-O3: FunctionPass Manager
; CHECK-O3: OpcodeCounter Pass

; CHECK-Os: FunctionPass Manager
; CHECK-Os: OpcodeCounter Pass

define i32 @main(i32, i8** nocapture readnone) {
  ret i32 1
}
