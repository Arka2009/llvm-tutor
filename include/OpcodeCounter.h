//========================================================================
// FILE:
//    OpcodeCounter.h
//
// DESCRIPTION:
//    Declares the OpcodeCounter Pass
//
// License: MIT
//========================================================================
#ifndef LLVM_TUTOR_OPCODECOUNTER_H
#define LLVM_TUTOR_OPCODECOUNTER_H

#include "llvm/IR/Function.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

using ResultOpcodeCounter = llvm::StringMap<unsigned>;

//------------------------------------------------------------------------------
// New PM interface
//------------------------------------------------------------------------------
struct OpcodeCounter : public llvm::AnalysisInfoMixin<OpcodeCounter> {
  using Result = ResultOpcodeCounter;
  Result run(llvm::Function &F, llvm::FunctionAnalysisManager &);
  Result runOnFunction(llvm::Function &F);

  // A special type used by analysis passes to provide an address that
  // identifies that particular analysis pass type.
  static llvm::AnalysisKey Key;
};

//------------------------------------------------------------------------------
// Legacy PM interface
//------------------------------------------------------------------------------
struct LegacyOpcodeCounter : public llvm::FunctionPass {
  static char ID;
  LegacyOpcodeCounter() : llvm::FunctionPass(ID) {}
  bool runOnFunction(llvm::Function &F) override;
  // The print method must be implemented by Legacy analys passes in order to
  // print a human readable version of the analysis results:
  //  http://llvm.org/docs/WritingAnLLVMPass.html#the-print-method
  void print(llvm::raw_ostream &OutS, llvm::Module const *M) const override;

  ResultOpcodeCounter OC;
  OpcodeCounter Impl;
};

//------------------------------------------------------------------------------
// Helper functions
//------------------------------------------------------------------------------
// Pretty-prints the result of this analysis
void printOpcodeCounterResult(llvm::raw_ostream &OutS,
                              const ResultOpcodeCounter &OC);

#endif
