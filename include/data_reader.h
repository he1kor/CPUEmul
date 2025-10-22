#include <expected>
#include <array>
#include <string>
#include <sstream>

#include <vector>
#include <cstdint>

class DataReader {
public:
    struct TranslationError {
        enum class Code {
            INVALID_FORMAT,
            INVALID_ADDRESS,
            INVALID_VALUE,
            ADDRESS_OUT_OF_RANGE,
            VALUE_OUT_OF_RANGE,
            DUPLICATE_ADDRESS,
            UNKNOWN_ERROR
        };
        
        static constexpr std::array<std::string_view, 8> stringCodes{
            "Invalid format: expected 'ADDRESS VALUE'",
            "Invalid address format",
            "Invalid value format", 
            "Address out of range",
            "Value out of uint32_t range",
            "Duplicate address",
            "Unknown error"
        };
        
        Code code;
        size_t line;
        
        
    };
    static const char* toStr(TranslationError::Code code) {
        return TranslationError::stringCodes[static_cast<size_t>(code)].data();
    }

    template<uint32_t DMEM_SIZE>
    static std::expected<std::array<uint32_t, DMEM_SIZE>, TranslationError> 
    parseData(const std::string& dataSource) {
        std::array<uint32_t, DMEM_SIZE> result{0};
        std::istringstream stream(dataSource);
        std::string line;
        size_t lineNumber = 0;
        
        std::vector<bool> usedAddresses(DMEM_SIZE, false);
        
        while (std::getline(stream, line)) {
            lineNumber++;
            
            if (line.size() == 1 && (line[0] == '\n' || line[0] == '\r')) continue;
            if (line[0] == '#') continue;
            
            std::istringstream lineStream(line);
            std::string addressStr, valueStr;
            
            if (!(lineStream >> addressStr >> valueStr)) {
                return std::unexpected(TranslationError{
                    TranslationError::Code::INVALID_FORMAT, lineNumber
                });
            }
            
            std::string extra;
            if (lineStream >> extra) {
                return std::unexpected(TranslationError{
                    TranslationError::Code::INVALID_FORMAT, lineNumber
                });
            }
            
            uint32_t address;
            try {
                size_t pos;
                address = std::stoul(addressStr, &pos, 0);
                
                if (pos != addressStr.length()) {
                    return std::unexpected(TranslationError{
                        TranslationError::Code::INVALID_ADDRESS, lineNumber
                    });
                }
            } catch (const std::exception&) {
                return std::unexpected(TranslationError{
                    TranslationError::Code::INVALID_ADDRESS, lineNumber
                });
            }
            
            if (address >= DMEM_SIZE) {
                return std::unexpected(TranslationError{
                    TranslationError::Code::ADDRESS_OUT_OF_RANGE, lineNumber
                });
            }
            
            if (usedAddresses[address]) {
                return std::unexpected(TranslationError{
                    TranslationError::Code::DUPLICATE_ADDRESS, lineNumber
                });
            }
            
            uint32_t value;
            try {
                size_t pos;
                unsigned long long tempValue = std::stoull(valueStr, &pos, 0);
                
                if (pos != valueStr.length()) {
                    return std::unexpected(TranslationError{
                        TranslationError::Code::INVALID_VALUE, lineNumber
                    });
                }
                
                if (tempValue > UINT32_MAX) {
                    return std::unexpected(TranslationError{
                        TranslationError::Code::VALUE_OUT_OF_RANGE, lineNumber
                    });
                }
                
                value = static_cast<uint32_t>(tempValue);
            } catch (const std::exception&) {
                return std::unexpected(TranslationError{
                    TranslationError::Code::INVALID_VALUE, lineNumber
                });
            }
            
            result[address] = value;
            usedAddresses[address] = true;
        }
        
        return result;
    }
};