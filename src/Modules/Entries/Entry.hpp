///
/// @author Amrit Bhogal on 04/06/2023
/// @brief 
/// @version 1.0.0
///
#pragma once

#include <clang/AST/Type.h>

#include "Common.hpp"

namespace LuaClang
{
    /// @brief Represents an entry in the module, for example, a function, class, or variable.
    class Entry : public virtual ICodeGenerator {
    public:
        std::string name;
    };

    /// @brief Represents a function entry in the module, has return type and arguments.
    class FunctionEntry : public Entry {
    public:
        clang::Type return_type;
        std::vector<clang::Type> arguments;

        std::string generate_code() override;
    };

    /// @brief Represents a variable entry in the module
    class VariableEntry : public Entry {
    public:
        clang::Type type;

        std::string generate_code() override;
    };

    /// @brief Represents a Userdata entry (classes count)
    class UserdataEntry : public Entry {
    public:
        std::vector<FunctionEntry> methods;
        std::vector<VariableEntry> instance_variables;

        std::string generate_code() override;
    };

//    /// @brief Represents a "light userdata" entry, but also any type that isn't explicitly registered.
//    /// @example
//    /// @code
//    /// ```cpp
//    /// struct [[lua_export(userdata)]] Foo {
//    ///     int x;
//    ///     struct Bar baz; //This is a light userdata, because it isn't explicitly registered, and thus cannot be used in Lua.
//    /// };
//    /// ```
//    /// @endcode
//    class LightUserdataEntry : public Entry {
//    public:
//        clang::Type type;
//
//        std::string generate_code() override;
//    };

}
