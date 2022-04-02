#pragma once

#include <vector>
#include <string>

// get files recursively in a folder based on regex
std::vector<std::string> getFiles(const std::string & dir, const std::string & regex);

// load a file into a string
std::string loadFile(const std::string & file);

// file exists
bool fileExists(const std::string & file);

// get currently used memory
void getMemoryUsage(double & memVM, double & memRSS);
