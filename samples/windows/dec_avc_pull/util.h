#pragma once

class stdout_utf16
{
public:
    stdout_utf16()
    {
        // change stdout to Unicode. Cyrillic and Ideographic characters will appear in the console (console font is unicode).
        _setmode(_fileno(stdout), _O_U16TEXT);
    }

    ~stdout_utf16()
    {
        // restore ANSI mode
        _setmode(_fileno(stdout), _O_TEXT);
    }
};

inline std::wstring getExeDir()
{
    WCHAR exedir[MAX_PATH];
    GetModuleFileName(NULL, exedir, MAX_PATH);
    PathRemoveFileSpec(exedir);
    return std::wstring(exedir);
}

inline void deleteFile(const wchar_t* file)
{
    DeleteFile(file);
}

inline std::wstring toWide(const std::string& s)
{
    return std::wstring(s.begin(), s.end());
}

inline void printError(const wchar_t* action, const primo::avblocks::modern::TErrorInfo& e)
{
    using namespace std;

    if (action)
        wcout << action << L": ";

    if (e.facility() == primo::error::ErrorFacility::Success)
    {
        wcout << L"Success" << endl;
        return;
    }

    if (!e.message().empty())
        wcout << toWide(e.message()) << L", ";

    wcout << L"facility:" << e.facility()
          << L", error:" << e.code()
          << L", hint:" << toWide(e.hint())
          << endl;
}

inline bool compareNoCase(const wchar_t* arg1, const wchar_t* arg2)
{
    return 0 == _wcsicmp(arg1, arg2);
}

inline std::vector<uint8_t> readFileBytes(const wchar_t* name)
{
    std::ifstream f(name, std::ios::binary);
    std::vector<uint8_t> bytes;
    if (f)
    {
        f.seekg(0, std::ios::end);
        size_t filesize = f.tellg();
        bytes.resize(filesize);
        f.seekg(0, std::ios::beg);
        f.read(reinterpret_cast<char*>(&bytes[0]), filesize);
    }
    return bytes;
}

inline bool makeDir(const std::wstring& dir)
{
    std::error_code ec;
    std::filesystem::create_directories(dir, ec);
    return !ec;
}
