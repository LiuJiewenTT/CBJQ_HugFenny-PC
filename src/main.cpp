#include "program_info.hpp"
#include "utils.hpp"
#include <stdio.h>
#include <locale>


int main() {
    #ifdef _WIN32 
    SetConsoleOutputCP(CP_UTF8); 
    #endif
    setlocale(LC_ALL, "C.UTF-8");
    printf("%s\n", PROGRAM_INFO_STRING);

    /*--- divide line ---*/

    string command_content_str, command_output_str, command_error_str;
    bool command_executed_success_flag = false;
    int command_exit_code = 0;

    // command_content_str = "adb --version";
    command_content_str = "adb shell \"ls /sdcard/Android/data/ | grep snow\"";
    command_executed_success_flag = ExecuteCommand(command_content_str, &command_output_str, &command_error_str, &command_exit_code);
    printf("command_content_str: %s\n", command_content_str.c_str());
    printf("command_executed_success_flag: %d\n", command_executed_success_flag);
    printf("command_exit_code: %d\n", command_exit_code);
    printf("command_output_str: %s\n", command_output_str.c_str());
    printf("command_error_str: %s\n", command_error_str.c_str());
    
    /*--- divide line ---*/
    return 0;
}
