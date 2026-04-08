#pragma once

#include <vector>
#include <string>

enum ErrorCodes
{ 
	Parsed = 0,			
	Error,
	Command,
};

struct Options
{
	bool help;
	std::vector<std::string> input_video;
    std::vector<std::string> input_audio;
	std::string output_file;
	
	Options(): help(false){}
};

ErrorCodes prepareOptions(Options& opt, int argc, char* argv[]);

