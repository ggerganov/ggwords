#include "common.h"

#include <regex>
#include <filesystem>
#include <fstream>
#include <sstream>

// sysconf
#include <unistd.h>

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

void getMemoryUsage(double& memVM, double& memRSS) {
    using std::ios_base;
    using std::ifstream;
    using std::string;

    memVM = 0.0;
    memRSS = 0.0;

    // 'file' stat seems to give the most reliable results
    //
    ifstream stat_stream("/proc/self/stat", ios_base::in);

    // dummy vars for leading entries in stat that we don't care about
    //
    string pid, comm, state, ppid, pgrp, session, tty_nr;
    string tpgid, flags, minflt, cminflt, majflt, cmajflt;
    string utime, stime, cutime, cstime, priority, nice;
    string O, itrealvalue, starttime;

    // the two fields we want
    //
    unsigned long vsize;
    long rss;

    stat_stream
        >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
        >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
        >> utime >> stime >> cutime >> cstime >> priority >> nice
        >> O >> itrealvalue >> starttime >> vsize >> rss; // don't care about the rest

    stat_stream.close();

    long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages

    memVM  = vsize / 1024.0;
    memRSS = rss * page_size_kb;
}
