#pragma once

enum ErrorCodes
{ 
	Parsed = 0,			
	Error,
	Command,
};

struct Options
{
    Options(): help(false){}
    
    std::wstring inputFile;
    std::wstring outputFile;

    bool help;
};

ErrorCodes prepareOptions(Options &opt, int argc, wchar_t* argv[]);

