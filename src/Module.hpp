#pragma once

#include <llvm/Support/raw_os_ostream.h>
#include <clang/Lex/Pragma.h>

#include <vector>
#include <string>

#include "Function.hpp"
#include "Userdata.hpp"

using namespace clang;

namespace LuaClang
{
    enum struct Type {
        INTEGER,
        NUMBER,
        STRING,
        FUNCTION,
        USERDATA,
        LIGHT_USERDATA
    };

    struct Module {
        std::string name, file;
        std::vector<Function> functions;
        std::vector<Userdata> userdatas;

        std::string to_code();
    };

    class ModulePragmaHandler : public PragmaHandler {
    public:
        ModulePragmaHandler(Module &module) : PragmaHandler("module"), _module(module)
        {}


        virtual void HandlePragma(Preprocessor &preproc, PragmaIntroducer intro, Token &ft) override;

    private:
        Module &_module;
    };
}
