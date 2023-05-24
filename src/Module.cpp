#include "Module.hpp"
#include <clang/Lex/Preprocessor.h>
#include <clang/Lex/Token.h>

#include <sstream>

using namespace LuaClang;

std::string
LuaClang::Module::to_code()
{
    std::ostringstream code = {};

    // Includes
    code << "#include <lua.h>\n";
    code << "#include <lauxlib.h>\n";
    code << "#include <lualib.h>\n";

    //First, generate the function wrappers
    for (Function &func : functions)
        code << func.to_wrapper() << "\n";

    //Make sure it isn't compiled away
    code << "__attribute__((used))\n";
    code << "\nstatic luaL_Reg $lib" << name <<"_functions$[] = {\n";
    {
        for (Function &func : functions)
            code << "    { \"" << func.name << "\", " << func.wrapper_name() << " },\n";
    }
    code << "};\n";

    code << "int luaopen_" << name << "(lua_State *state)\n";
    code << "{\n";
    {
        code << "    luaL_newlib(state, $lib" << name <<"_functions$);\n";
        code << "    return 1;\n";
    }
    code << "}\n";

    return code.str();
}

void ModulePragmaHandler::HandlePragma(Preprocessor &preproc, PragmaIntroducer intro, Token &ft)
{
    auto loc = ft.getLocation();
    auto &sm = preproc.getSourceManager();

    llvm::outs() << "Found #pragma lua module at " << loc.printToString(preproc.getSourceManager()) << "\n";

    Token tk = {};
    std::ostringstream directive = {};

    while(tk.isNot(tok::eod)) {
        preproc.Lex(tk);
        if (tk.isNot(tok::eod))
            directive << preproc.getSpelling(tk);
    }

    std::string s = directive.str();
    if (s.empty()) {
        llvm::errs() << "#pragma lua module must be followed with a module name";
        return;
    }

    _module.name = s.substr(1).substr(0, s.length()-2);
    llvm::outs() << "Module name: " << _module.name << "\n";
}
