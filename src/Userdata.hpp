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
    struct Userdata {
        std::string name;
        std::unordered_map<QualType, std::string> fields;
    };

}
