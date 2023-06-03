#pragma once

#include <clang/Frontend/CompilerInstance.h>
#include <clang/AST/ASTConsumer.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Rewrite/Core/Rewriter.h>
#include <clang/Lex/Pragma.h>
#include <unordered_map>
#include <typeinfo>

#include "Type.hpp"

using namespace clang;

namespace LuaClang
{
    ///
    /// Not userdata registered type
    ///
    class Struct : LuaClang::Type {
    public:

    };

    class Userdata : LuaClang::Type {
    public:
        std::unordered_map<std::string, std::shared_ptr<Type>> fields;

        Userdata(std::string name) : Type(name, "void *")
        {}

        Userdata &create(std::string name) const
        {
            auto it = registry.find(name);
            if (it != registry.end()) {
                auto sec = it->second.get();
                if (typeid(*sec) != typeid(*this))
                    throw std::runtime_error("Userdata " + name + " already exists, but is not the same type");
                else
                    return *dynamic_cast<Userdata *>(sec);
            }

            auto ptr = std::make_shared<Userdata>(name);
            registry.emplace(name, ptr);
            return *ptr;
        }
    };

}
