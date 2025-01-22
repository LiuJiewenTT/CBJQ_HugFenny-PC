#include "utils.hpp"

namespace ns_string {
    // replace all occurance of t in s to w
    std::string replace_all(const std::string &s, std::string const &t, std::string const &w) {
        std::string result = s;
        std::string::size_type pos = result.find(t), t_size = t.size(), r_size = w.size();
        while (pos != std::string::npos) { // found
            result.replace(pos, t_size, w);
            pos = result.find(t, pos + r_size);
        }
        return result;
    }

    // trim from start (in place)
    void ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
            return !std::isspace(ch);
        }));
    }

    // trim from end (in place)
    void rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
        }).base(), s.end());
    }

    // trim from both ends (in place)
    void trim(std::string &s) {
        rtrim(s);
        ltrim(s);
    }

    // trim from start (copying)
    std::string ltrim_copy(std::string s) {
        ltrim(s);
        return s;
    }

    // trim from end (copying)
    std::string rtrim_copy(std::string s) {
        rtrim(s);
        return s;
    }

    // trim from both ends (copying)
    std::string trim_copy(std::string s) {
        trim(s);
        return s;
    }

    void split_lines(const std::string &s, std::vector<std::string> &lines) {
        lines.clear();
        std::istringstream iss(s);
        std::string line;
        while (std::getline(iss, line)) {
            trim(line);
            lines.push_back(line);
        }
    }

    std::string join_strings(const std::vector<std::string>& strings, const std::string& delimiter) {
        std::ostringstream oss;
        int size = strings.size();
        for (int i = 0; i < size; ++i) {
            oss << strings[i];
            if (i != size - 1) {
                oss << delimiter;
            }
        }
        return oss.str();
    }
} // namespace ns_string


wchar_t *convertCharToWChar(const char *message) {
    // 将 char 字符串转换为 wchar_t 字符串
    int len = MultiByteToWideChar(CP_UTF8, 0, message, -1, NULL, 0);
    wchar_t *wMessage = (wchar_t *)malloc(len * sizeof(wchar_t));
    MultiByteToWideChar(CP_UTF8, 0, message, -1, wMessage, len);
    return wMessage;
}


string wrapStringToEscapeCmd(const string &str) {
    string result = str;
    result = ns_string::replace_all(result, "\"", "\"\"");
    result = ns_string::replace_all(result, "|", "^|");
    result = ns_string::replace_all(result, ">", "^>");
    result = ns_string::replace_all(result, "<", "^<");
    return result;
}


bool ExecuteCommand(const string &command, string *output, string *error, int *returnCode) {
    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    string command_to_execute;
    clog << format("command passed in: {}", command) << endl;    
    command_to_execute = format("cmd /c \"{}\"", command);
    // command_to_execute = format("cmd /c \"{}\"", command);
    clog << format("command to execute: {}", command_to_execute) << endl;

    // 创建管道用于获取标准输出
    HANDLE hStdOutRead, hStdOutWrite;
    if (!CreatePipe(&hStdOutRead, &hStdOutWrite, &saAttr, 0)) {
        cerr << format("CreatePipe (StdOut) failed ({})", GetLastError()) << endl;
        return false;
    }

    // 创建管道用于获取标准错误
    HANDLE hStdErrRead, hStdErrWrite;
    if (!CreatePipe(&hStdErrRead, &hStdErrWrite, &saAttr, 0)) {
        cerr << format("CreatePipe (StdErr) failed ({})", GetLastError()) << endl;
        return false;
    }

    // 创建进程启动信息
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.hStdOutput = hStdOutWrite;
    si.hStdError = hStdErrWrite;
    si.dwFlags |= STARTF_USESTDHANDLES;

    // 创建进程信息
    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    // 将命令行参数转换为 wchar_t 字符串
    wchar_t *wCommand = convertCharToWChar(command_to_execute.c_str());

    // 执行命令
    if (!CreateProcess(NULL, wCommand, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        cerr << format("CreateProcess failed ({})", GetLastError()) << endl;
        return false;
    }

    free2NULL(wCommand);

    // 关闭写管道
    CloseHandle(hStdOutWrite);
    CloseHandle(hStdErrWrite);

    // 读取标准输出
    char buffer[4096];
    DWORD bytesRead;
    while (ReadFile(hStdOutRead, buffer, sizeof(buffer), &bytesRead, NULL) && bytesRead > 0) {
        output->append(buffer, bytesRead);
    }

    // 读取标准错误
    while (ReadFile(hStdErrRead, buffer, sizeof(buffer), &bytesRead, NULL) && bytesRead > 0) {
        error->append(buffer, bytesRead);
    }

    // 等待进程结束并获取返回值
    WaitForSingleObject(pi.hProcess, INFINITE);
    if (returnCode) {
        GetExitCodeProcess(pi.hProcess, (LPDWORD)returnCode);
    }

    // 关闭进程和句柄
    CloseHandle(hStdOutRead);
    CloseHandle(hStdErrRead);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return true;
}


int execute_command(const string &command_content_str, string *command_output_str, string *command_error_str, int *command_exit_code,
                    bool *command_executed_success_flag) {
    clog << format("command_content_str: {}", command_content_str) << endl;
    string output, error;
    bool status;
    int exit_code = 0;
    status = ExecuteCommand(command_content_str, &output, &error, &exit_code);
    command_executed_success_flag != nullptr ? (*command_executed_success_flag = status) : 0;
    command_exit_code != nullptr ? (*command_exit_code = exit_code) : 0;
    command_output_str != nullptr ? (*command_output_str = output) : 0;
    command_error_str != nullptr ? (*command_error_str = error) : 0;

    clog << format("command_executed_success_flag: {}", status) << endl;
    clog << format("command_exit_code: {}", exit_code) << endl;
    clog << format("command_output_str: {}", output) << endl;
    clog << format("command_error_str: {}", error) << endl;
    return exit_code;
}
