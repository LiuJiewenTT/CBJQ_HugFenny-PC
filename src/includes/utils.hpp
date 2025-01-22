#pragma once
#define _UNICODE
#define UNICODE

#include <cstdio>
#include <format>
#include <iostream>
#include <string>
#include <windows.h>
#include <vector>
#include <sstream>
#include <ranges>

using std::cerr;
using std::clog;
using std::cout;
using std::endl;
using std::string;

using std::wcerr;
using std::wclog;
using std::wcout;

namespace std {
#define wendl endl
} // namespace std
using std::wendl;
using std::wstring;

using std::format;

namespace ns_string {
    // replace all occurance of t in s to w
    std::string replace_all(const std::string &s, std::string const &t, std::string const &w);

    /* Trim functions from https://stackoverflow.com/a/217605 */

    void ltrim(std::string &s);
    void rtrim(std::string &s);
    void trim(std::string &s);
    std::string ltrim_copy(std::string s);
    std::string rtrim_copy(std::string s);
    std::string trim_copy(std::string s);

    // std::vector<std::string> split(const std::string &s, char delimiter);
    void split_lines(const std::string &s, std::vector<std::string> &lines);
    std::string join_strings(const std::vector<std::string>& strings, const std::string& delimiter);
} // namespace ns_string


wchar_t *convertCharToWChar(const char *message);
#define WCharChar(x) (convertCharToWChar(x))


template <typename T> inline void free2NULL(T *&ptr) {
    free(ptr);     // 释放内存
    ptr = nullptr; // 将指针置为空
}


string wrapStringToEscapeCmd(const string &str);


/**
 * Executes a command and returns the output and error messages.
 */
bool ExecuteCommand(const string &command, string *output, string *error, int *returnCode = nullptr);


/**
 * Executes a command and returns the output and error messages. This function is a wrapper for the ExecuteCommand function and it will log
 * the process.
 */
int execute_command(const string &command_content_str, string *command_output_str = nullptr, string *command_error_str = nullptr,
                    int *command_exit_code = nullptr, bool *command_executed_success_flag = nullptr);
