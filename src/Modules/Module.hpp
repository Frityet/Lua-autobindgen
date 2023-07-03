///
/// @author Amrit Bhogal on 04/06/2023
/// @brief 
/// @version 1.0.0
///
#pragma once

#include <clang/Lex/Preprocessor.h>
#include <clang/AST/Type.h>

#include "Common.hpp"
#include "Entries/Entry.hpp"

namespace LuaClang
{
    /// @brief Represents a module, which is a collection of entries.
    class Module : public clang::PragmaHandler, public virtual ICodeGenerator {
    public:
        std::string name;
        std::vector<std::unique_ptr<Entry>> entries;

        virtual std::string pragma_prefix()
        { return "lua"; }

        void HandlePragma(clang::Preprocessor &preproc, clang::PragmaIntroducer intro, clang::Token &tok) override;
    };

}
