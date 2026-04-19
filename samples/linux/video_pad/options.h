#pragma once

#include <string>

enum ErrorCodes { Parsed = 0, Error, Command };

struct Options {
    Options(): help(false), width(1920), height(1080),
               padLeft(0), padRight(0), padTop(0), padBottom(0),
               padColor(0xFF000000U) {}

    bool help;
    std::string inputFile;
    std::string outputFile;
    int width;
    int height;
    int padLeft;
    int padRight;
    int padTop;
    int padBottom;
    unsigned int padColor;
};

ErrorCodes prepareOptions(Options& opt, int argc, char* argv[]);
