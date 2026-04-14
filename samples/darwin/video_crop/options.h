#pragma once

enum ErrorCodes { Parsed = 0, Error, Command };

struct Options {
    Options(): help(false), cropLeft(0), cropRight(0), cropTop(0), cropBottom(0) {}
    
    bool help;
    std::string inputFile;
    std::string outputFile;
    int cropLeft;
    int cropRight;
    int cropTop;
    int cropBottom;
};

ErrorCodes prepareOptions(Options& opt, int argc, char* argv[]);
