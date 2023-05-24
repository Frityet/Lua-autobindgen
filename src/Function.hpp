#pragma once

#include <clang/Frontend/CompilerInstance.h>
#include <clang/AST/ASTConsumer.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Rewrite/Core/Rewriter.h>
#include <clang/Lex/Pragma.h>

#include <unordered_map>

using namespace clang;

namespace LuaClang
{
    struct Module;
    //Maps primitive types (i.e int) to the lua function to convert, i.e integer
    //TODO: This should be a { [QualType] : const char * }!
    static std::unordered_map<std::string, std::string_view> LUA_TYPE_CONVERTERS = {
        { "int",                    "integer" },
        { "float",                  "number" },
        { "double",                 "number" },
        { "bool",                   "boolean" },
        {  "_Bool",                 "boolean" },
        { "char",                   "integer" },
        { "unsigned int",           "integer" },
        { "unsigned char",          "integer" },
        { "unsigned short",         "integer" },
        { "unsigned long",          "integer" },
        { "unsigned long long",     "integer" },
        { "long",                   "integer" },
        { "long long",              "integer" },
        { "short",                  "integer" },
        { "void",                   "integer" },
        { "signed char",            "integer" },
        { "wchar_t",                "integer" },
        { "char16_t",               "integer" },
        { "char32_t",               "integer" },
        { "int8_t",                 "integer" },
        { "int16_t",                "integer" },
        { "int32_t",                "integer" },
        { "int64_t",                "integer" },
        { "uint8_t",                "integer" },
        { "uint16_t",               "integer" },
        { "uint32_t",               "integer" },
        { "uint64_t",               "integer" },
        { "size_t",                 "integer" },
        { "ptrdiff_t",              "integer" },
        { "intptr_t",               "integer" },
        { "uintptr_t",              "integer" },

        { "const char *",           "string" },
        { "char *",                 "string" },
    };


    struct Function {
        std::string name;
        std::vector<QualType> arguments;
        QualType return_type;

        std::string wrapper_name() const
        { return "$luawrapper_"+name+"$"; }

        std::string to_wrapper() const;
    };

    class FunctionVisitor : public RecursiveASTVisitor<FunctionVisitor> {
    public:
        FunctionVisitor(Rewriter &rw, Module &module) : _rewriter(rw), _module(module) {}

        bool VisitFunctionDecl(FunctionDecl *func);

    private:
        Rewriter &_rewriter;
        Module &_module;
    };

    class FunctionASTConsumer : public ASTConsumer {
    public:
        FunctionASTConsumer(CompilerInstance &comp, Rewriter &rw, Module &module)
            : _module(module), _rewriter(rw), _visitor(rw, module) {}

        bool HandleTopLevelDecl(DeclGroupRef decls) override;

    private:
        Module &_module;
        Rewriter &_rewriter;
        FunctionVisitor _visitor;
    };
}
