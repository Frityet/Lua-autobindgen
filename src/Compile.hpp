/**
 * Copyright 2021 Timo Nicolai
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
**/

#pragma once

#include <cassert>
#include <memory>
#include <string>

#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/CompilerInvocation.h>
#include <clang/Lex/PreprocessorOptions.h>
#include <llvm/ADT/ArrayRef.h>
#include <llvm/Support/MemoryBuffer.h>
#include <clang/CodeGen/CodeGenAction.h>

namespace LuaClang
{
    template<typename IT>
    static inline void compile(clang::CompilerInstance *CI,
                        std::string const &FileName,
                        IT FileBegin,
                        IT FileEnd)
    {
        auto &CodeGenOpts { CI->getCodeGenOpts() };
        auto &Target { CI->getTarget() };
        auto &Diagnostics { CI->getDiagnostics() };

        // create new compiler instance
        auto CInvNew { std::make_shared<clang::CompilerInvocation>() };

        auto arr = std::vector<const char *>(CodeGenOpts.CommandLineArgs.size());
        std::transform(CodeGenOpts.CommandLineArgs.begin(), CodeGenOpts.CommandLineArgs.end(),
                       arr.begin(), [](std::string const &s) { return s.c_str(); });

        bool CInvNewCreated { clang::CompilerInvocation::CreateFromArgs(*CInvNew, arr, Diagnostics) };

        assert(CInvNewCreated);

        clang::CompilerInstance CINew;
        CINew.setInvocation(CInvNew);
        CINew.setTarget(&Target);
        CINew.createDiagnostics();

        // create rewrite buffer
        std::string FileContent { FileBegin, FileEnd };
        auto FileMemoryBuffer { llvm::MemoryBuffer::getMemBufferCopy(FileContent) };

        // create "virtual" input file
        auto &PreprocessorOpts { CINew.getPreprocessorOpts() };
        PreprocessorOpts.addRemappedFile(FileName, FileMemoryBuffer.get());

        // generate code
        clang::EmitObjAction EmitObj;
        CINew.ExecuteAction(EmitObj);

        // clean up rewrite buffer

        (void)FileMemoryBuffer.release();
    }
}
