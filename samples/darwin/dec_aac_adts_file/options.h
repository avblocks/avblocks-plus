#pragma once

enum ErrorCodes
{
    Parsed = 0,
    Error,
    Command,
};

struct Options
{
    Options(): help(false) {}
    
    std::string inputFile;
    std::string outputFile;
    
    bool help;
};

ErrorCodes prepareOptions(Options &opt, int argc, char* argv[]);