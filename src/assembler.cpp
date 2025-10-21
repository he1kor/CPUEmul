#include "assembler.h"

#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <format>
#include <unordered_map>

#include <iostream>

size_t UNKNOWN_LINE = -1;

std::string Assembler::toStr(TranslationError error){
    return std::format("Translation error: {}(line: {})\n", Assembler::toStr(error.code), error.line);
}

std::expected<std::vector<Assembly>, Assembler::TranslationError> Assembler::translate(const std::string &source, size_t instructionLimit) const{
    size_t lines = calcLines(source);
    if (lines > instructionLimit){
        return std::unexpected(TranslationError{TranslationError::Code::BloatError, lines});
    }

    std::vector<Assembly> output;
    output.reserve(lines);

    std::istringstream iss(source);
    std::string line;
    
    size_t line_i = 0;
    while (std::getline(iss, line)){
        auto expectedAssembly = parseLine(line);
        if (!expectedAssembly){
            auto& error = expectedAssembly.error();
            error.line = line_i;
            return std::unexpected(error);
        }
        line_i++;
        if (!(*expectedAssembly))
            continue;
        output.push_back(**expectedAssembly);
    }
    
    return output;
}

size_t Assembler::calcLines(std::string_view source) const{
    return std::ranges::count(source, '\n');
}


std::expected<std::optional<Assembly>, Assembler::TranslationError> Assembler::parseLine(std::string& line) const{
    std::istringstream line_stream(line);
    std::string token;
    
    size_t word_i = 0;
    
    Assembly result;
    
    size_t expectedTokens = 1;

    while (line_stream >> token) {
        if (token[0] == '#')
            break;
        if (word_i == 0){
            auto expectedInstruction = parseInstructionToken(token);
            if (!expectedInstruction){
                return std::unexpected(expectedInstruction.error());
            }
            result.instructionCode = *expectedInstruction;
            expectedTokens = totalTokensFor(result.instructionCode);
        }
        if (word_i == 1){
            result.isLiteral = checkIsLiteralType(token);
            std::string_view view(token);
            if (!result.isLiteral)
                view = view.substr(1);
            auto expectedValue = parseValue(view);
            if (!expectedValue){
                return std::unexpected(expectedValue.error());
            }
            result.value = *expectedValue;
        }
        if (word_i >= expectedTokens){
            return std::unexpected(TranslationError{TranslationError::Code::UnexpectedToken, UNKNOWN_LINE});
        }
        word_i++;
    }
    if (word_i == 0){
        return std::nullopt;
    }
    if (word_i != expectedTokens){
        return std::unexpected(TranslationError{TranslationError::Code::IncompleteLine, UNKNOWN_LINE});
    }
    return result;
}
std::expected<uint16_t, Assembler::TranslationError> Assembler::parseInstructionToken(std::string_view token) const{

    static const std::unordered_map<std::string_view, uint16_t> parseMap{
        {"NOP", Asm::NOP},
        {"LOAD", Asm::LOAD},
        {"STORE", Asm::STORE},
        {"LOADI", Asm::LOADI},
        {"ADD", Asm::ADD},
        {"SUB", Asm::SUB},
        {"INC", Asm::INC},
        {"DEC", Asm::DEC},
        {"AND", Asm::AND},
        {"OR", Asm::OR},
        {"XOR", Asm::XOR},
        {"NOT", Asm::NOT},
        {"NOT", Asm::NOT},
        {"SHL", Asm::SHL},
        {"SHR", Asm::SHR},
        {"JMP", Asm::JMP},
        {"JZ", Asm::JZ},
        {"JNZ", Asm::JNZ},
        {"JC", Asm::JC},
        {"JNC", Asm::JNC},
        {"HLT", Asm::HLT},
    };

    auto it = parseMap.find(token);
    if (it == parseMap.end()){
        return std::unexpected(TranslationError{TranslationError::Code::BadToken, UNKNOWN_LINE});
    }
    return it->second;
}

std::expected<uint16_t, Assembler::TranslationError> Assembler::parseValue(std::string_view token) const{
    uint64_t result;
    auto fromCharsResult = std::from_chars(token.data(), token.data() + token.size(), result);

    if (fromCharsResult.ec == std::errc() && 
        fromCharsResult.ptr == token.data() + token.size()){

        uint64_t value_mask = ((static_cast<uint64_t>(1) << VALUE_BITS_COUNT) - 1);
        if ((value_mask & result) != result){
            return std::unexpected(TranslationError{TranslationError::Code::OutOfRange, UNKNOWN_LINE});
        }
        return value_mask & result;
    }
    return std::unexpected(TranslationError{TranslationError::Code::BadValue, UNKNOWN_LINE});
}

bool Assembler::checkIsLiteralType(std::string_view line) const{
    if (line[0] == '*')
        return false;
    return true;
}

size_t Assembler::totalTokensFor(uint16_t token) const{
    static size_t totalTokensFor[20] {
        [Asm::NOP] = 1,
        [Asm::LOAD] = 2,
        [Asm::STORE] = 2,
        [Asm::LOADI] = 2,

        [Asm::ADD] = 2,
        [Asm::SUB] = 2,
        [Asm::INC] = 1,
        [Asm::DEC] = 1,

        [Asm::AND] = 2,
        [Asm::OR] = 2,
        [Asm::XOR] = 2,
        [Asm::NOT] = 1,
        
        [Asm::SHL] = 2,
        [Asm::SHR] = 2,

        [Asm::JMP] = 2,
        [Asm::JZ] = 2,
        [Asm::JNZ] = 2,
        [Asm::JC] = 2,
        [Asm::JNC] = 2,

        [Asm::HLT] = 1,
    };
    return totalTokensFor[token];
}
