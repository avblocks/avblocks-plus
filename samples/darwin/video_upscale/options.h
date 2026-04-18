#pragma once

#include <string>

enum ErrorCodes { Parsed = 0, Error, Command };

struct Options {
    Options(): help(false), width(1920), height(1080) {}

    bool help;
    std::string inputFile;
    std::string outputFile;
    int width;
    int height;
};

ErrorCodes prepareOptions(Options& opt, int argc, char* argv[]);
