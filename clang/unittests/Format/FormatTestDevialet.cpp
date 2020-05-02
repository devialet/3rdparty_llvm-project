//===- unittest/Format/FormatTest.cpp - Formatting unit tests -------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "clang/Format/Format.h"

#include "../Tooling/ReplacementTest.h"
#include "FormatTestUtils.h"

#include "clang/Frontend/TextDiagnosticPrinter.h"
#include "llvm/Support/Debug.h"
#include "gtest/gtest.h"

#define DEBUG_TYPE "format-test"

namespace clang {
namespace format {
namespace {

FormatStyle getDevialetStyle() { return getDevialetStyle(FormatStyle::LK_Cpp); }

class FormatTestDevialet : public ::testing::Test {
protected:
  enum StatusCheck { SC_ExpectComplete, SC_ExpectIncomplete, SC_DoNotCheck };

  std::string format(llvm::StringRef Code,
                     const FormatStyle &Style = getDevialetStyle(),
                     StatusCheck CheckComplete = SC_ExpectComplete) {
    LLVM_DEBUG(llvm::errs() << "---\n");
    LLVM_DEBUG(llvm::errs() << Code << "\n\n");
    std::vector<tooling::Range> Ranges(1, tooling::Range(0, Code.size()));
    FormattingAttemptStatus Status;
    tooling::Replacements Replaces =
        reformat(Style, Code, Ranges, "<stdin>", &Status);
    if (CheckComplete != SC_DoNotCheck) {
      bool ExpectedCompleteFormat = CheckComplete == SC_ExpectComplete;
      EXPECT_EQ(ExpectedCompleteFormat, Status.FormatComplete)
          << Code << "\n\n";
    }
    ReplacementCount = Replaces.size();
    auto Result = applyAllReplacements(Code, Replaces);
    EXPECT_TRUE(static_cast<bool>(Result));
    LLVM_DEBUG(llvm::errs() << "\n" << *Result << "\n\n");
    return *Result;
  }

  void verifyFormat(llvm::StringRef Code,
                    const FormatStyle &Style = getDevialetStyle()) {
    EXPECT_EQ(Code.str(), format(Code, Style)) << "Expected code is not stable";
    EXPECT_EQ(Code.str(), format(test::messUp(Code), Style));
  }

  int ReplacementCount;
};

TEST_F(FormatTestDevialet, AnonymousNamespace) {
  verifyFormat("namespace\n"
               "{\n"
               "    int i;\n"
               "}");
}

TEST_F(FormatTestDevialet, Namespace) {
  verifyFormat("namespace N {\n"
               "\n"
               "int i;\n"
               "\n"
               "}");
}

TEST_F(FormatTestDevialet, NestedNamespaces) {
  verifyFormat("namespace N1 {\n"
               "namespace N2 {\n"
               "\n"
               "int i;\n"
               "\n"
               "}\n"
               "}");
}

TEST_F(FormatTestDevialet, AsyncLambda) {
  verifyFormat("setDiscoverability( discoverabssssility )->finishWith(\n"
               "    [ discoverability ]()\n"
               "    {\n"
               "        int i;\n"
               "    } );");
}

/*
setDiscoverability( discoverability )->finishWith([ discsssoverability ]( const auto& result )\n"
*/
} // namespace
} // namespace format
} // namespace clang
