#include <clang/Basic/TokenKinds.h>

#include <sstream>

#include "Function.hpp"
#include "Module.hpp"

using namespace LuaClang;

constexpr const char *LUA_FUNCTION_EXPORT_ATTRIBUTE = "lua export function";

bool FunctionVisitor::VisitFunctionDecl(FunctionDecl *func)
{
    auto &out = llvm::outs();

    out << "Visiting function " << func->getNameAsString() << "\n";

    if (func->hasAttr<clang::AnnotateAttr>() and func->hasBody()) {
        out << "Found annotated function " << func->getNameAsString() << "\n";
        auto *attr = func->getAttr<clang::AnnotateAttr>();
        if (attr->getAnnotation() != LUA_FUNCTION_EXPORT_ATTRIBUTE)
            return true;

        auto &ctx = func->getASTContext();
        auto &sm = ctx.getSourceManager();
        auto loc = sm.getSpellingLoc(attr->getLocation());

        std::vector<QualType> arguments = {};

        for (ParmVarDecl *param : func->parameters())
            arguments.emplace_back(param->getOriginalType().getUnqualifiedType());

        out << "Vec size: " << _module.functions.size() << "\n";
        _module.functions.emplace_back(Function {
            .name = func->getName().str(),
            .arguments = arguments,
            .return_type = func->getReturnType().getUnqualifiedType()
        });

        out << "Found function " << func->getNameAsString() << " at " << loc.printToString(sm) << "\nInfo:\n";
        out << "  Return type: " << func->getReturnType().getAsString() << "\n";
        out << "  Arguments:\n";
        for (ParmVarDecl *param : func->parameters())
            out << "    " << param->getOriginalType().getAsString() << "\n";

        // SourceLocation start = func->getSourceRange().getBegin();
        // out << "Inserting wrapper at " << start.printToString(sm) << "\n";

        // //LLVM returns true on error. This is extremely counter-intuitive.
        // bool ok = !_rewriter.InsertText(start, _module.functions.back().to_wrapper(), false, true);
        // out << (ok ? "Ok" : "Err") << "\n";
        // out << "New contents: " << _rewriter.getRewrittenText(func->getSourceRange()) << "\n";
    }

    return true;
}

bool FunctionASTConsumer::HandleTopLevelDecl(DeclGroupRef decls)
{
    for (Decl *decl : decls)
        _visitor.TraverseDecl(decl);

    //Insert the module code at the end of the file
    // _rewriter.InsertTextAfter((*decls.end())->getEndLoc(), _module.to_code());
    // llvm::outs() << "New contents: " << _rewriter.getRewrittenText((*decls.end())->getSourceRange()) << "\n";

    return true;
}

std::string Function::to_wrapper() const
{
    std::stringstream ss;

    // Function declaration
    ss << "__typeof(" << return_type.getAsString() << ") " << name << "(";
    for (size_t i = 0; i < arguments.size(); ++i) {
        if (i != 0)
            ss << ", ";
        ss << "__typeof(" << arguments[i].getAsString() << ") $arg" << i << "$";
    }
    ss << ");\n";

    //Make sure it wont be compiled away
    ss << "__attribute__((used))\n";

    // Function definition
    ss << "static int " << wrapper_name() << "(lua_State *L)\n";
    ss << "{\n";
    // Variable declarations, to store the arguments
    for (size_t i = 0; i < arguments.size(); ++i) {
        std::string arg = arguments[i].getAsString();
        std::string conversion = "udata";
        if (LUA_TYPE_CONVERTERS.count(arg))
            conversion = LUA_TYPE_CONVERTERS[arg];

        if (conversion == "udata")
            ss << "    __typeof(" << arg << ") $arg" << i << "$ = (luaL_check" << conversion << "(L, " << i+1 << ", \"$type" << i << "$\"));\n";
        else
            ss << "    __typeof(" << arg << ") $arg" << i << "$ = (luaL_check" << conversion << "(L, " << i+1 << "));\n";

    }

    bool has_ret = return_type.getAsString() != "void";
    // Function call
    if (has_ret)
        ss << "    __typeof(" << return_type.getAsString() << ") $ret$ = ";

    ss << name << "    (";
    for (size_t i = 0; i < arguments.size(); ++i) {
        if (i != 0)
            ss << ", ";
        ss << "$arg" << i << "$";
    }
    ss << ");\n";

    // Push the result (lua_push*)
    std::string conversion = "userdata";
    if (LUA_TYPE_CONVERTERS.count(return_type.getAsString()))
        conversion = LUA_TYPE_CONVERTERS[return_type.getAsString()];

    if (has_ret)
        ss << "    lua_push" << conversion << "(L, $ret$);\n";

    ss << "    return " << (int)has_ret << ";\n";
    ss << "}\n";

    return ss.str();
}
