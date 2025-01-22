#include "program_info.hpp"
#include "utils.hpp"
#include "utils_toplevel.hpp"
#include <stdio.h>
#include <locale>
#include <fstream>


bool disable_log_stream_flag = true;


int main(int argc, char *argv[]) {
    SetConsoleOutputCP(CP_UTF8); 
    setlocale(LC_ALL, "C.UTF-8");
    printf("%s\n\n", PROGRAM_INFO_STRING);
    std::ofstream nullStream("nul");

    /*--- divide line ---*/

    string adb_version_str;

    std::vector<string> app_package_name_list;
    std::vector<string> selected_apps_package_name_list;
    std::vector<int> selected_apps_localization_value_list;
    std::vector<bool> selected_apps_localization_file_existence_list;
    std::vector<bool> selected_apps_localization_file_may_try_deletion_list;
    std::vector<bool> selected_apps_localization_file_existence_after_deletion_list;
    std::vector<bool> selected_apps_localization_file_may_try_deletion_failed_list;
    std::vector<bool> selected_apps_localization_file_may_try_deletion_failed_list_1;
    std::vector<bool> selected_apps_localization_file_may_try_deletion_failed_list_2;
    std::vector<bool> selected_apps_write_localization_value_failed_list;

    int operation_choice = 0;
    string operation_choice_str;
    std::vector<string> operation_choice_list;
    std::vector<string> operation_choice_list_1 = {"All 1", "All 0", "Let me decide on each app"};
    std::vector<string> operation_choice_list_2 = {"All 1", "All 0", "Let me decide on each app"};
    std::vector<string> operation_choice_list_3 = {"Yes", "No"};

    int substring_start_index = 0;
    string command_content_str, command_output_str, command_error_str;
    bool command_executed_success_flag = false;
    int command_exit_code = 0;

    #define exit_on_error() if (!command_executed_success_flag || command_exit_code) { \
        cerr << format("command execution failed: {}", command_error_str) << endl; \
        return EXIT_FAILURE; \
    }


    {
        string filename = string(argv[0]) + ".enable_log_stream";
        FILE *fp = fopen(filename.c_str(), "r");
        if (fp) {
            fclose(fp);
        } else {
            disable_log_stream_flag = false;
            clog.rdbuf(nullStream.rdbuf());
        }
    }

    command_content_str = "adb --version | findstr /C:\"version\"";
    execute_command(command_content_str, &command_output_str, &command_error_str, &command_exit_code, &command_executed_success_flag);
    exit_on_error();
    ns_string::trim(command_output_str);
    substring_start_index = command_output_str.find("version");
    adb_version_str = command_output_str.substr(substring_start_index + 8);
    cout << format("adb version: {}", adb_version_str) << endl;

    // 暂时不打算支持多设备选择

    command_content_str = wrapStringToEscapeCmd("adb shell \"ls /sdcard/Android/data/ | grep snow\"");
    execute_command(command_content_str, &command_output_str, &command_error_str, &command_exit_code, &command_executed_success_flag);
    exit_on_error();
    ns_string::trim(command_output_str);
    ns_string::split_lines(command_output_str, app_package_name_list);
    cout << format("app package name list: {}", ns_string::join_strings(app_package_name_list, ", ")) << endl;

    selected_apps_package_name_list = select_apps(app_package_name_list);
    cout << format("selected app package name list: {}", ns_string::join_strings(selected_apps_package_name_list, ", ")) << endl;
    if (selected_apps_package_name_list.empty()) {
        cout << "no app selected, exit" << endl;
        return EXIT_SUCCESS;
    }

    selected_apps_localization_file_existence_list = exist_all_localization_files(selected_apps_package_name_list);
    cout << "selected app localization file existence list: " << endl;
    for (int i = 0; i < selected_apps_package_name_list.size(); ++i) {
        string app_package_name = selected_apps_package_name_list[i];
        bool localization_file_existence = selected_apps_localization_file_existence_list[i];
        cout << format("app package name: {}, localization file existence: {}", app_package_name, localization_file_existence) << endl;
    }

    selected_apps_localization_value_list = get_localization_values(selected_apps_package_name_list);
    cout << "selected app localization value list: " << endl;
    for (int i = 0; i < selected_apps_package_name_list.size(); ++i) {
        string app_package_name = selected_apps_package_name_list[i];
        int localization_value = selected_apps_localization_value_list[i];
        cout << format("app package name: {}, localization value: {}", app_package_name, localization_value) << endl;
    }

    for (int i = 0; i < selected_apps_package_name_list.size(); ++i) {
        string app_package_name = selected_apps_package_name_list[i];
        bool localization_file_existence = selected_apps_localization_file_existence_list[i];
        int localization_value = selected_apps_localization_value_list[i];
        
        if (localization_file_existence && localization_value == -1) {
            selected_apps_localization_file_may_try_deletion_list.push_back(true);
        } else {
            selected_apps_localization_file_may_try_deletion_list.push_back(false);
        }
    }

    operation_choice = select_single_item(operation_choice_list_2, "Do you want to try the solution of deletion on selected apps: ");
    operation_choice_str = operation_choice_list_2[operation_choice];
    cout << format("operation choice: {}", operation_choice_str) << endl;

    switch (operation_choice) {
        case 0:
            selected_apps_localization_file_may_try_deletion_failed_list_1 = delete_all_localization_files(selected_apps_package_name_list, selected_apps_localization_file_may_try_deletion_list);
            break;
        case 1:
            // Do nothing, passed.
            break;
        case 2: {
            for (int i = 0; i < selected_apps_package_name_list.size(); ++i) {
                string app_package_name = selected_apps_package_name_list[i];
                bool localization_file_may_try_deletion = selected_apps_localization_file_may_try_deletion_list[i];
                if (localization_file_may_try_deletion) {
                    operation_choice = select_single_item(operation_choice_list_3, "Do you want to try the solution of deletion on selected app: " + app_package_name + ": ");
                    operation_choice_str = operation_choice_list_3[operation_choice];
                    cout << format("operation choice: {}", operation_choice_str) << endl;
                    switch (operation_choice) {
                        case 0: {
                            bool try_deletion_failed = delete_localization_file(app_package_name);
                            selected_apps_localization_file_may_try_deletion_failed_list_1.push_back(try_deletion_failed);
                            break;
                        }
                        default:
                            selected_apps_localization_file_may_try_deletion_failed_list_1.push_back(false);
                    }
                } else {
                    selected_apps_localization_file_may_try_deletion_failed_list_1.push_back(false);
                }
            }
            break;
        }
    }

    cout << "selected app localization file may try deletion list: " << endl;
    for (int i = 0; i < selected_apps_package_name_list.size(); ++i) {
        string app_package_name = selected_apps_package_name_list[i];
        bool localization_file_may_try_deletion = selected_apps_localization_file_may_try_deletion_list[i];
        cout << format("app package name: {}, localization file may try deletion: {}", app_package_name, localization_file_may_try_deletion) << endl;
    }

    operation_choice = select_single_item(operation_choice_list_2, "Please select an operation to perform on selected apps: ");
    operation_choice_str = operation_choice_list_2[operation_choice];
    cout << format("operation choice: {}", operation_choice_str) << endl;

    switch (operation_choice) {
        case 0:
            selected_apps_write_localization_value_failed_list = set_localization_values_same_for_all(selected_apps_package_name_list, 1, &selected_apps_localization_value_list);
            break;
        case 1:
            selected_apps_write_localization_value_failed_list = set_localization_values_same_for_all(selected_apps_package_name_list, 0, &selected_apps_localization_value_list);
            break;
        case 2:
            break;
    }

    #undef exit_on_error
    /*--- divide line ---*/
    return EXIT_SUCCESS;
}
