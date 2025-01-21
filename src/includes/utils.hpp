#pragma once
#define _UNICODE
#define UNICODE

#include <iostream>
#include <cstdio>
#include <string>
#include <format>
#include <windows.h>

using std::cout;
using std::cerr;
using std::clog;
using std::endl;
using std::string;

using std::wcout;
using std::wcerr;
using std::wclog;

namespace std {
    #define wendl endl
}
using std::wendl;
using std::wstring;

using std::format;

namespace ns_string {
    // replace all occurance of t in s to w  
    std::string replace_all(const std::string & s, std::string const & t, std::string const & w);
}


wchar_t *convertCharToWChar(const char* message);
#define WCharChar(x) (convertCharToWChar(x))

#define free2NULL(x) free(x);x=NULL;


/**
 * Executes a command and returns the output and error messages.
 */
bool ExecuteCommand(const string& command, string* output, string* error, int* returnCode=NULL);
