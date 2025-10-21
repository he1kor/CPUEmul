#pragma once

#include <string>
#include <vector>
#include <expected>

#include "assembly.h"

class Assembler{
public:
    struct TranslationError{
        enum class Code{
            BloatError,
            OutOfRange,
            BadToken,
            UnexpectedToken,
            IncompleteLine,
            BadValue
        };
        static constexpr std::array<std::string, 6> stringCodes{
            "BloatError",
            "OutOfRange",
            "BadToken",
            "UnexpectedToken",
            "IncompleteLine",
            "BadValue"
        };
        Code code;
        size_t line;
    };
    static std::string toStr(TranslationError::Code code) {return TranslationError::stringCodes[static_cast<uint64_t>(code)];};
    static std::string toStr(TranslationError error);

    std::expected<std::vector<Assembly>, TranslationError> translate(const std::string& source, size_t instructionLimit = 1024) const;
private:
    size_t calcLines(std::string_view source) const;
    std::expected<uint16_t, TranslationError> parseInstructionToken(std::string_view token) const;
    std::expected<std::optional<Assembly>, TranslationError> parseLine(std::string& token) const;
    std::expected<uint16_t, TranslationError> parseValue(std::string_view token) const;
    bool checkIsLiteralType(std::string_view line) const;

    size_t totalTokensFor(uint16_t token) const;

    size_t VALUE_BITS_COUNT = 10;
};