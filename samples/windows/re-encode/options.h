/*
 *  Copyright (c) 2016 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.
 */
#pragma once

enum ErrorCodes
{
    Parsed = 0,
    Error,
    Command,
};

struct YesNo
{
    YesNo(bool v): val(v) {}
    bool val;
    operator bool() { return val; }
};

struct Options
{
    Options() : help(false),
        reEncodeVideo(true),
        reEncodeAudio(true)
    {}

    bool help;

    std::wstring inputFile;
    std::wstring outputFile;

    YesNo reEncodeVideo;
    YesNo reEncodeAudio;
};

std::wistringstream& operator>>(std::wistringstream& in, YesNo& yn);

inline std::basic_ostream<wchar_t>& operator<<(std::basic_ostream<wchar_t>& out, YesNo& yn)
{
    if (yn.val)
        out << L"yes";
    else
        out << L"no";

    return out;
}

ErrorCodes prepareOptions(Options& opt, int argc, wchar_t* argv[]);
