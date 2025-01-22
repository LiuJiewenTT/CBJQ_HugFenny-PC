#include "program_info.hpp"
#include "utils.hpp"
#include <stdio.h>
#include <locale>


int main() {
    #ifdef _WIN32 
    SetConsoleOutputCP(CP_UTF8); 
    #endif
    setlocale(LC_ALL, "C.UTF-8");
    printf("%s\n\n", PROGRAM_INFO_STRING);

    /*--- divide line ---*/

    string adb_version_str;

    std::vector<string> app_package_name_list;

    int substring_start_index = 0;
    string command_content_str, command_output_str, command_error_str;
    bool command_executed_success_flag = false;
    int command_exit_code = 0;
    
    std::istringstream iss;
    string line_str;

    command_content_str = "adb --version | findstr /C:\"version\"";
    execute_command(command_content_str, &command_output_str, &command_error_str, &command_exit_code, &command_executed_success_flag);
    ns_string::trim(command_output_str);
    substring_start_index = command_output_str.find("version");
    adb_version_str = command_output_str.substr(substring_start_index + 8);
    cout << format("adb version: {}", adb_version_str) << endl;

    command_content_str = wrapStringToEscapeCmd("adb shell \"ls /sdcard/Android/data/ | grep snow\"");
    execute_command(command_content_str, &command_output_str, &command_error_str, &command_exit_code, &command_executed_success_flag);
    ns_string::trim(command_output_str);
    // clog << format("command_output_str: {}", command_output_str) << endl;
    ns_string::split_lines(command_output_str, app_package_name_list);
    cout << format("app package name list: {}", ns_string::join_strings(app_package_name_list, ", ")) << endl;

    /*--- divide line ---*/
    return 0;
}
