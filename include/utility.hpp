#pragma once

#include <filesystem>
#include <vector>
#include <string>
#include <format>

namespace file {
    namespace fs = std::filesystem;
    
    std::vector<std::string> FindFile(std::string_view dir, std::string_view extension) {
        std::vector<std::string> files;
        
        for (const auto& entry : std::filesystem::recursive_directory_iterator(std::filesystem::current_path().string())) {
            if (entry.is_directory() && entry.path().filename() == dir) {
                for (const auto& file : std::filesystem::directory_iterator(entry.path())) {
                    if (file.is_regular_file() && file.path().extension() == extension) {
                        files.push_back(file.path().string());
                    }
                }

                if (files.empty()) {
                    std::string mes = std::format("File with '{}' extension was not found in the '{}' folder.\n", 
                        extension, dir);
                    throw std::runtime_error(mes);
                }
            }
        }

        if (files.empty()) {
            std::string mes = std::format("'{}' folder was not found.\n", dir);
            throw std::runtime_error(mes);
        }

        return files;
    }
} // namespace file