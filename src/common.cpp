#include "common.h"

#include <regex>
#include <filesystem>
#include <fstream>
#include <sstream>

// get files recursively in a folder based on regex
std::vector<std::string> getFiles(const std::string& dir, const std::string& regex) {
    std::vector<std::string> files;
    std::regex re(regex);
    for (const auto& entry : std::filesystem::recursive_directory_iterator(dir)) {
        if (std::regex_match(entry.path().string(), re)) {
            files.push_back(entry.path().string());
        }
    }
    return files;
}

std::string loadFile(const std::string& file) {
    std::ifstream in(file);
    std::stringstream buffer;
    buffer << in.rdbuf();
    return buffer.str();
}

bool fileExists(const std::string& file) {
    std::ifstream f(file);
    return f.good();
}
