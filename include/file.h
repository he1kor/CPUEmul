#pragma once

#include <expected>
#include <string>
#include <array>
#include <fstream>
#include <filesystem>

namespace file{

    enum class FileError {
        FileNotFound,
        NotAFile,
        AccessDenied,
        ReadError,
        FileTooLarge,
        EmptyFile,
        InvalidEncoding
    };

    static constexpr std::array<std::string, 7> fileErrorStringCodes{
        "FileNotFound",
        "NotAFile",
        "AccessDenied",
        "ReadError",
        "FileTooLarge",
        "EmptyFile",
        "InvalidEncoding"
    };
    
    inline std::string toStr(FileError code) {return fileErrorStringCodes[static_cast<uint64_t>(code)];};

    inline std::expected<std::string, FileError> read(const std::filesystem::path &filepath){

        std::error_code ec;
        if (!std::filesystem::is_regular_file(filepath, ec) || ec) {
            return std::unexpected(FileError::FileNotFound);
        }
        
        std::ifstream file(filepath, std::ios::binary);
        if (!file) {
            return std::unexpected(FileError::AccessDenied);
        }
        
        std::string source;
        file.seekg(0, std::ios::end);
        auto size = file.tellg();
        
        if (size == 0) {
            return std::unexpected(FileError::EmptyFile);
        }
        
        file.seekg(0, std::ios::beg);
        source.resize(size);
        
        if (!file.read(source.data(), size)) {
            return std::unexpected(FileError::ReadError);
        }
        return source;
    }
}