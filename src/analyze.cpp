// CMake-generated header containing timestamp of the build
#include "build_timestamp.h"

#include "common.h"

#include <vector>
#include <fstream>
#include <map>
#include <string>
#include <sstream>

struct BookInfo {
    std::string fname;
};

using BookList = std::vector<BookInfo>;

BookList readBookList(const std::string & fname) {
    BookList result;

    std::ifstream fin(fname);
    if (!fin) {
        throw std::runtime_error("Cannot open file " + fname);
    }

    std::string line;
    while (std::getline(fin, line)) {
        if (line.empty()) {
            continue;
        }

        BookInfo info;
        std::stringstream ss(line);
        ss >> info.fname;
        result.push_back(info);
    }

    return result;
}

std::string filter(const std::string & src) {
    std::string result;
    result.reserve(src.size());

    bool wasSpace = false;
    for (char c : src) {
        if (std::isalpha(c)) {
            wasSpace = false;
            result.push_back(std::tolower(c));
        } else {
            if (wasSpace == false) {
                result.push_back(' ');
                wasSpace = true;
            }
        }
    }

    return result;
}

int main(int argc, char** argv) {
    printf("%s\n", BUILD_TIMESTAMP);

    if (argc < 3) {
        printf("Usage: %s <books.txt> <path>\n", argv[0]);
        return 1;
    }

    const std::string fnameBooks = argv[1];
    const std::string path = argv[2];

    BookList books = readBookList(fnameBooks);
    printf("Loaded %lu books\n", books.size());

    int nFound = 0;
    for (const auto & book : books) {
        std::string fname = path + "/" + book.fname + "_text.txt";

        if (fileExists(fname) == false) {
            printf("File %s does not exist\n", fname.c_str());
            continue;
        }

        auto content = filter(loadFile(fname));
        ++nFound;
    }

    printf("Found %d books\n", nFound);

    return 0;
}
