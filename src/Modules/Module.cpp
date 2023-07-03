///
/// @author Amrit Bhogal on 04/06/2023
/// @brief 
/// @version 1.0.0
///

#include "Modules/Module.hpp"

#include <sstream>

#include "Log.hpp"

using namespace LuaClang;

void Module::HandlePragma(clang::Preprocessor &preproc, clang::PragmaIntroducer intro, clang::Token &tok)
{
    auto loc = tok.getLocation();
    auto &sm = preproc.getSourceManager();
    debug("Found #pragma lua module at line ", sm.getSpellingLineNumber(loc));

    clang::Token tk = {};
    std::ostringstream directive = {};

    while(tk.isNot(clang::tok::eod)) {
        preproc.Lex(tk);
        if (tk.isNot(clang::tok::eod))
            directive << preproc.getSpelling(tk);
    }

    std::string s = directive.str().substr(1, directive.str().size() - 2);
    std::replace(s.begin(), s.end(), '.', '_');
    if (s.empty()) error("#pragma lua module must be followed with a module name");

    name = std::move(s);
    debug("Module name: ", name);
}
