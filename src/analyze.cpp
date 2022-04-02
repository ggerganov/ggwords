// CMake-generated header containing timestamp of the build
#include "build_timestamp.h"

#include "common.h"

#include <vector>
#include <fstream>
#include <map>
#include <string>
#include <sstream>
#include <unordered_map>

#include <thread>
#include <atomic>

using TGram = uint32_t;
using TCount = uint64_t;
using TGramFreq = std::unordered_map<TGram, TCount>;

struct BookInfo {
    std::string fname;
};

struct LanguageModel {
    TCount nChars = 0;
    TCount nWords = 0;

    std::unordered_map<int, TGramFreq> nGramFreq = {
        { 1, {}, },
        { 2, {}, },
        { 3, {}, },
        { 4, {}, },
        { 5, {}, },
        { 6, {}, },
    };
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

std::string filter(const std::string & src, LanguageModel & lm) {
    std::string result;
    result.reserve(src.size());

    bool wasSpace = false;
    for (char c : src) {
        if (std::isalpha(c)) {
            wasSpace = false;
            result.push_back(std::tolower(c));
        } else if (wasSpace == false) {
            lm.nWords++;
            result.push_back(' ');
            wasSpace = true;
        }
    }

    lm.nChars += result.size();

    return result;
}

void process(const std::string & text, LanguageModel & lm) {
    TGram curGram = 0;

    int i = 0;
    for (char c : text) {
        if (c == ' ') {
            c = 26;
        } else if (c >= 'a' && c <= 'z') {
            c -= 'a';
        } else {
            printf("????????? %c\n", c);
        }

        curGram = (curGram << 5) | c;

        (i >= 0) && lm.nGramFreq[1][(curGram) & 0b11111]++;
        (i >= 1) && lm.nGramFreq[2][(curGram) & 0b1111111111]++;
        (i >= 2) && lm.nGramFreq[3][(curGram) & 0b111111111111111]++;
        (i >= 3) && lm.nGramFreq[4][(curGram) & 0b11111111111111111111]++;
        (i >= 4) && lm.nGramFreq[5][(curGram) & 0b1111111111111111111111111]++;
        (i >= 5) && lm.nGramFreq[6][(curGram) & 0b111111111111111111111111111111]++;

        i++;
    }
}

std::string gramToStr(int l, TGram gram) {
    std::string result;
    result.reserve(l);

    for (int i = 0; i < l; i++) {
        char c = gram & 0b11111;
        if (c == 26) {
            c = '_';
        } else {
            c += 'A';
        }
        result.push_back(c);
        gram >>= 5;
    }

    // return the revers of "result"
    std::reverse(result.begin(), result.end());
    return result;
}

void printNGramStats(const std::string & fname, const LanguageModel & lm, int l) {
    const auto & nGramFreq = lm.nGramFreq.at(l);

    std::vector<std::pair<TGram, TCount>> sorted;
    sorted.reserve(nGramFreq.size());
    for (const auto & kv : nGramFreq) {
        sorted.push_back(std::make_pair(kv.first, kv.second));
    }
    std::sort(sorted.begin(), sorted.end(), [](const auto & a, const auto & b) {
        return a.second > b.second;
    });

    {
        std::ofstream fout(fname);
        for (const auto & kv : sorted) {
            //printf("%s %d\n", gramToStr(l, kv.first).c_str(), kv.second);
            fout << gramToStr(l, kv.first) << " " << kv.second << '\n';
        }
        fout.close();
    }
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

    LanguageModel lm;
    std::atomic<int> nProcessed(0);

    //for (const auto & book : books) {
    //    std::string fname = path + "/" + book.fname + "_text.txt";

    //    if (fileExists(fname) == false) {
    //        //printf("File %s does not exist\n", fname.c_str());
    //        continue;
    //    }

    //    auto content = filter(loadFile(fname));
    //    process(content, lm);

    //    ++nProcessed;
    //    if (nProcessed % 100 == 0) {
    //        printf("Processed %d books\n", nProcessed);
    //    }

    //    {
    //        double memVM;
    //        double memRSS;
    //        getMemoryUsage(memVM, memRSS);
    //        printf("Used memory: %g MB / %g MB\n", memVM/1024.0, memRSS/1024.0);
    //    }
    //}


    {
        const int nThread = std::thread::hardware_concurrency();

        std::atomic<int> globalIndex(0);
        std::vector<std::thread> workers(nThread);
        std::vector<LanguageModel> lms(nThread);

        for (int ith = 0; ith < nThread; ith++) {
            workers[ith] = std::thread([&lms, &nProcessed, &globalIndex, &books, &path](int ith) {
                while (true) {
                    int curIdx = globalIndex.fetch_add(1);
                    if (curIdx >= (int) books.size()) {
                        return;
                    }
                    {
                        const auto & book = books[curIdx];
                        if (book.fname == "PG3512") {
                            printf("Skipping '%s'\n", book.fname.c_str());
                            continue;
                        }

                        const std::string fname = path + "/" + book.fname + "_text.txt";
                        if (fileExists(fname) == false) {
                            //printf("File %s does not exist\n", fname.c_str());
                            continue;
                        }

                        const auto content = filter(loadFile(fname), lms[ith]);
                        process(content, lms[ith]);

                        if (nProcessed.fetch_add(1) % 100 == 0) {
                            printf("Processed %d books. Index: %d / %lu\n", nProcessed.load(), curIdx, books.size());

                            {
                                double memVM;
                                double memRSS;
                                getMemoryUsage(memVM, memRSS);
                                printf("Used memory: %g MB / %g MB\n", memVM/1024.0, memRSS/1024.0);
                            }
                        }
                    }
                }
            }, ith);
        }

        for (auto & th : workers) {
            th.join();
        }

        // merge lms into lm
        for (int i = 0; i < nThread; i++) {
            for (int l = 1; l <= 6; l++) {
                const auto & nGramFreq = lms[i].nGramFreq.at(l);
                for (const auto & kv : nGramFreq) {
                    lm.nGramFreq[l][kv.first] += kv.second;
                }

                lm.nChars += lms[i].nChars;
                lm.nWords += lms[i].nWords;
            }
        }
    }

    printf("Processed %d books\n", nProcessed.load());
    printf("Processed %lu words\n", lm.nWords);
    printf("Processed %lu characters\n", lm.nChars);

    printNGramStats("ggwords-1-gram.dat", lm, 1);
    printNGramStats("ggwords-2-gram.dat", lm, 2);
    printNGramStats("ggwords-3-gram.dat", lm, 3);
    printNGramStats("ggwords-4-gram.dat", lm, 4);
    printNGramStats("ggwords-5-gram.dat", lm, 5);
    printNGramStats("ggwords-6-gram.dat", lm, 6);

    return 0;
}
