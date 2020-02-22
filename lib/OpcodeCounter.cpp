//=============================================================================
// FILE:
//    OpcodeCounter.cpp
//
// DESCRIPTION:
//    Visits all functions in a module, prints their names and the number of
//    arguments via stderr. Strictly speaking, this is an analysis pass (i.e.
//    the functions are not modified). However, in order to keep things simple
//    there's no 'print' method here, which every analysis pass should
//    implement.
//
// USAGE:
//    1. Legacy PM
//      opt -load libOpcodeCounter.dylib -legacy-opcode-count -disable-output \
//        <input-llvm-file>
//    2. New PM
//      opt -load-pass-plugin=libOpcodeCounter.dylib -passes="opcode-count" \
//        -disable-output <input-llvm-file>
//    3. Through an exisiting optimisation pipeline
//      opt -load libOpcodeCounter.dylib -O{0|1|2|3|s} -disable-output \
//        <input-llvm-file>
//
//
// License: MIT
//=============================================================================
#include "OpcodeCounter.h"

#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

using namespace llvm;

//-----------------------------------------------------------------------------
// OpcodeCounter implementation
//-----------------------------------------------------------------------------
AnalysisKey OpcodeCounter::Key;

OpcodeCounter::Result OpcodeCounter::runOnFunction(Function &F) {
  StringMap<unsigned> OC;
  for (auto &BB : F) {
    for (auto &Inst : BB) {
      StringRef Name = Inst.getOpcodeName();
      if (OC.find(Name) == OC.end()) {
        OC[Inst.getOpcodeName()] = 1;
      } else {
        OC[Inst.getOpcodeName()]++;
      }
    }
  }

  return OC;
}

OpcodeCounter::Result
OpcodeCounter::run(llvm::Function &F, llvm::FunctionAnalysisManager &) {
  return runOnFunction(F);
}

bool LegacyOpcodeCounter::runOnFunction(llvm::Function &F) {
  OC = Impl.runOnFunction(F);
  return false;
}

void LegacyOpcodeCounter::print(raw_ostream &OutS, Module const *) const {
  printOpcodeCounterResult(OutS, OC);
}

//-----------------------------------------------------------------------------
// New PM Registration
//-----------------------------------------------------------------------------
llvm::PassPluginLibraryInfo getOpcodeCounterPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "opcode-counter", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerAnalysisRegistrationCallback(
                [](FunctionAnalysisManager &FAM) {
                  FAM.registerPass([&] { return OpcodeCounter(); });
                });
          }};
};

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getOpcodeCounterPluginInfo();
}

//-----------------------------------------------------------------------------
// Legacy PM Registration
//-----------------------------------------------------------------------------
// The address of this variable is used to identify the pass. The actual value
// doesn't matter.
char LegacyOpcodeCounter::ID = 0;

// Register the pass - with this 'opt' will be able to recognize
// LegacyOpcodeCounter when added to the pass pipeline on the command line, i.e.
// via '--legacy-opcode-count'
static RegisterPass<LegacyOpcodeCounter>
    X("legacy-opcode-counter", "OpcodeCounter Pass",
      true,  // This pass doesn't modify the CFG => true
      false  // This pass is not a pure analysis pass => false
    );

// Register LegacyOpcodeCounter as a step of an existing pipeline. The insertion
// point is set to 'EP_EarlyAsPossible', which means that LegacyOpcodeCounter will
// be run automatically at '-O{0|1|2|3}'.
//
// NOTE: this trips 'opt' installed via HomeBrew (Mac OS). It's a known issues:
//    https://github.com/sampsyo/llvm-pass-skeleton/issues/7
// I've tried all of the suggestions, but no luck. Locally I recommend either
// building from sources or commenting this out. On Linux this always works
// fine.
static llvm::RegisterStandardPasses RegisterOpcodeCounter(
    llvm::PassManagerBuilder::EP_EarlyAsPossible,
    [](const llvm::PassManagerBuilder &Builder,
       llvm::legacy::PassManagerBase &PM) { PM.add(new LegacyOpcodeCounter()); });

//------------------------------------------------------------------------------
// Helper functions
//------------------------------------------------------------------------------
void printOpcodeCounterResult(raw_ostream &OutS,
                              const ResultOpcodeCounter &OC) {
  OutS << "================================================="
       << "\n";
  OutS << "LLVM-TUTOR: static analysis results\n";
  OutS << "=================================================\n";
  const char *str1 = "NAME";
  const char *str2 = "#N DIRECT CALLS";
  OutS << format("%-20s %-10s\n", str1, str2);
  OutS << "-------------------------------------------------"
       << "\n";
  for (auto &Inst : OC) {
    OutS << format("%-20s %-10lu\n", Inst.first().str().c_str(), Inst.second);
  }
}
