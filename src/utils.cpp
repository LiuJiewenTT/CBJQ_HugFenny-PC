#include "utils.hpp"


wchar_t *convertCharToWChar(const char* message){
    // 将 char 字符串转换为 wchar_t 字符串
    int len = MultiByteToWideChar(CP_UTF8, 0, message, -1, NULL, 0);
    wchar_t* wMessage = (wchar_t*)malloc(len * sizeof(wchar_t));
    MultiByteToWideChar(CP_UTF8, 0, message, -1, wMessage, len);
    return wMessage;
}


bool ExecuteCommand(const string& command, string* output, string* error, int* returnCode) {
    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    // 创建管道用于获取标准输出
    HANDLE hStdOutRead, hStdOutWrite;
    if (!CreatePipe(&hStdOutRead, &hStdOutWrite, &saAttr, 0)) {
        cerr << format("CreatePipe (StdOut) failed ({})", GetLastError()) << wendl;
        return false;
    }

    // 创建管道用于获取标准错误
    HANDLE hStdErrRead, hStdErrWrite;
    if (!CreatePipe(&hStdErrRead, &hStdErrWrite, &saAttr, 0)) {
        cerr << format("CreatePipe (StdErr) failed ({})", GetLastError()) << wendl;
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
    wchar_t* wCommand = convertCharToWChar(command.c_str());

    // 执行命令
    if (!CreateProcess(NULL, wCommand, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        cerr << format("CreateProcess failed ({})", GetLastError()) << wendl;
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


