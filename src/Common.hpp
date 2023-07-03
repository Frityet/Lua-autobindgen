///
/// @author Amrit Bhogal on 04/06/2023
/// @brief 
/// @version 1.0.0
///
#pragma once

#include <string>

namespace LuaClang
{
    /// @brief Base interface for any code generator
    class ICodeGenerator {
    public:
        virtual std::string generate_code() = 0;

        virtual ~ICodeGenerator() = default;
    };
}
