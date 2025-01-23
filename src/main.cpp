#include "program_info.hpp"
#include "utils.hpp"
#include "utils_toplevel.hpp"
#include <stdio.h>
#include <locale>
#include <fstream>


bool disable_log_stream_flag = true;
bool nopause_flag = false;


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
    std::vector<bool> selected_apps_localization_file_may_try_deletion_flag_list;
    std::vector<int> selected_apps_localization_file_may_try_deletion_index_list;
    bool may_need_deletion_flag = false;

    std::vector<bool> selected_apps_localization_file_may_try_deletion_failed_list;
    std::vector<bool> selected_apps_localization_file_may_try_deletion_failed_list_1;
    std::vector<bool> selected_apps_localization_file_may_try_deletion_failed_list_2;
    bool has_any_deletion_failed_flag = false;

    std::vector<bool> selected_apps_write_localization_value_failed_list;
    std::vector<bool> selected_apps_write_localization_value_failed_list_1;
    std::vector<bool> selected_apps_write_localization_value_failed_list_2;
    bool has_any_write_failed_flag = false;

    int operation_choice = 0;
    string operation_choice_str;
    std::vector<string> operation_choice_list;
    std::vector<string> operation_choice_list_1 = {"All 1", "All 0", "Let me decide on every app", "Keep current states"};
    std::vector<string> operation_choice_list_2 = {"Approve all", "Disapprove all", "Let me decide on every app"};
    std::vector<string> operation_choice_list_3 = {"Yes", "No"};
    std::vector<string> operation_choice_list_4 = {"Set to 1", "Set to 0", "Keep current state"};

    int substring_start_index = 0;
    string command_content_str, command_output_str, command_error_str;
    bool command_executed_success_flag = false;
    int command_exit_code = 0;

    #define try_pause() if (!nopause_flag) { \
        system("pause"); \
    } 

    #define exit_on_error() if (!command_executed_success_flag || command_exit_code) { \
        cerr << format("command execution failed: {}", command_error_str) << endl; \
        try_pause();\
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

    {
        string filename = string(argv[0]) + ".nopause";
        FILE *fp = fopen(filename.c_str(), "r");
        if (fp) {
            fclose(fp);
            nopause_flag = true;
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
        cerr << "no app selected, exit" << endl;
        try_pause();
        return EXIT_SUCCESS;
    }

    selected_apps_localization_file_existence_list = exist_all_localization_files(selected_apps_package_name_list);
    clog << "selected app localization file existence list: " << endl;
    for (int i = 0; i < selected_apps_package_name_list.size(); ++i) {
        string app_package_name = selected_apps_package_name_list[i];
        bool localization_file_existence = selected_apps_localization_file_existence_list[i];
        clog << format("app package name: {}, localization file existence: {}", app_package_name, localization_file_existence) << endl;
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
            selected_apps_localization_file_may_try_deletion_flag_list.push_back(true);
        } else {
            selected_apps_localization_file_may_try_deletion_flag_list.push_back(false);
        }
    }

    clog << "selected app localization file may try deletion list: " << endl;
    for (int i = 0; i < selected_apps_package_name_list.size(); ++i) {
        string app_package_name = selected_apps_package_name_list[i];
        bool localization_file_may_try_deletion = selected_apps_localization_file_may_try_deletion_flag_list[i];
        clog << format("app package name: {}, localization file may try deletion: {}", app_package_name, localization_file_may_try_deletion) << endl;
        may_need_deletion_flag |= localization_file_may_try_deletion;
    }

    if (may_need_deletion_flag) {
        for (int i = 0; i < selected_apps_package_name_list.size(); ++i) {
            if (selected_apps_localization_file_may_try_deletion_flag_list[i]) {
                selected_apps_localization_file_may_try_deletion_index_list.push_back(i);
            }
        }

        operation_choice = select_single_item(operation_choice_list_2, "Do you want to try the solution of deletion on some apps that require it: ");
        operation_choice_str = operation_choice_list_2[operation_choice];
        cout << format("operation choice: {}", operation_choice_str) << endl;

        switch (operation_choice) {
            case 0:
                selected_apps_localization_file_may_try_deletion_failed_list_1 = delete_all_localization_files(selected_apps_package_name_list, selected_apps_localization_file_may_try_deletion_flag_list);
                for (int i = 0; i < selected_apps_package_name_list.size(); ++i) {
                    selected_apps_localization_file_may_try_deletion_failed_list_1[i] = !selected_apps_localization_file_may_try_deletion_failed_list_1[i];
                }

                selected_apps_localization_file_existence_list = exist_all_localization_files(selected_apps_package_name_list);
                selected_apps_localization_file_may_try_deletion_failed_list_2 = selected_apps_localization_file_existence_list;
                for (int i = 0; i < selected_apps_package_name_list.size(); ++i) {
                    bool try_deletion_failed_1 = selected_apps_localization_file_may_try_deletion_failed_list_1[i];
                    bool try_deletion_failed_2 = selected_apps_localization_file_may_try_deletion_failed_list_2[i];
                    bool try_deletion_failed = (try_deletion_failed_1 || try_deletion_failed_2);
                    selected_apps_localization_file_may_try_deletion_failed_list.push_back(try_deletion_failed);
                    has_any_deletion_failed_flag |= try_deletion_failed;
                }
                break;
            case 1:
                // Do nothing, passed.
                for (int i = 0; i < selected_apps_package_name_list.size(); ++i) {
                    selected_apps_localization_file_may_try_deletion_failed_list_1.push_back(false);
                    selected_apps_localization_file_may_try_deletion_failed_list_2.push_back(false);
                    selected_apps_localization_file_may_try_deletion_failed_list.push_back(false);
                }
                has_any_deletion_failed_flag = false;   // default is false anyway.
                break;
            case 2: {
                for (int i = 0; i < selected_apps_package_name_list.size(); ++i) {
                    bool try_deletion_failed, try_deletion_failed_1, try_deletion_failed_2;
                    string app_package_name = selected_apps_package_name_list[i];
                    bool localization_file_may_try_deletion = selected_apps_localization_file_may_try_deletion_flag_list[i];
                    if (localization_file_may_try_deletion) {
                        operation_choice = select_single_item(operation_choice_list_3, "Do you want to try the solution of deletion on selected app: " + app_package_name + ": ");
                        operation_choice_str = operation_choice_list_3[operation_choice];
                        cout << format("operation choice: {}", operation_choice_str) << endl;
                        switch (operation_choice) {
                            case 0: {
                                try_deletion_failed_1 = !delete_localization_file(app_package_name);
                                selected_apps_localization_file_may_try_deletion_failed_list_1.push_back(try_deletion_failed_1);
                                break;
                            }
                            default:
                                try_deletion_failed_1 = false;
                                selected_apps_localization_file_may_try_deletion_failed_list_1.push_back(false);
                        }

                        try_deletion_failed_2 = exist_localization_file(app_package_name);
                        selected_apps_localization_file_may_try_deletion_failed_list_2.push_back(try_deletion_failed_2);
                        try_deletion_failed = (try_deletion_failed_1 || try_deletion_failed_2);
                        selected_apps_localization_file_may_try_deletion_failed_list.push_back(try_deletion_failed);
                        has_any_deletion_failed_flag |= try_deletion_failed;
                    } else {
                        selected_apps_localization_file_may_try_deletion_failed_list_1.push_back(false);
                        selected_apps_localization_file_may_try_deletion_failed_list_2.push_back(false);
                        selected_apps_localization_file_may_try_deletion_failed_list.push_back(false);
                    }
                }
                break;
            }
        }

        if (has_any_deletion_failed_flag) {
            cerr << "Apply the solution of deletion to some apps failed." << endl;
            for (int i = 0; i < selected_apps_package_name_list.size(); ++i) {
                string app_package_name = selected_apps_package_name_list[i];
                bool try_deletion_failed = selected_apps_localization_file_may_try_deletion_failed_list[i];
                bool try_deletion_failed_1 = selected_apps_localization_file_may_try_deletion_failed_list_1[i];
                bool try_deletion_failed_2 = selected_apps_localization_file_may_try_deletion_failed_list_2[i];
                if (try_deletion_failed) {
                    cerr << format("app package name: {}, try deletion failed: {} = {} || {}", app_package_name, try_deletion_failed, try_deletion_failed_1, try_deletion_failed_2) << endl;
                } else {
                    clog << format("app package name: {}, try deletion failed: {}", app_package_name, try_deletion_failed) << endl;
                }
            }
            try_pause();
            return EXIT_FAILURE;
        } else {
            cout << "Apply the solution of deletion to all selected apps successfully." << endl;
        }
    }
    

    operation_choice = select_single_item(operation_choice_list_1, "Please select an operation to perform on selected apps: ");
    operation_choice_str = operation_choice_list_1[operation_choice];
    cout << format("operation choice: {}", operation_choice_str) << endl;

    switch (operation_choice) {
        case 0:
            selected_apps_write_localization_value_failed_list_1 = set_localization_values_same_for_all(selected_apps_package_name_list, 1, &selected_apps_localization_value_list);
            for (int i = 0; i < selected_apps_package_name_list.size(); ++i) {
                selected_apps_write_localization_value_failed_list_1[i] = !selected_apps_write_localization_value_failed_list_1[i];
            }

            selected_apps_localization_value_list = get_localization_values(selected_apps_package_name_list);
            for (int i = 0; i < selected_apps_package_name_list.size(); ++i) {
                bool write_localization_value_failed_1 = selected_apps_write_localization_value_failed_list_1[i];
                bool write_localization_value_failed_2 = (selected_apps_localization_value_list[i] != 1);
                bool write_localization_value_failed = (write_localization_value_failed_1 || write_localization_value_failed_2);
                selected_apps_write_localization_value_failed_list_2.push_back(write_localization_value_failed_2);
                selected_apps_write_localization_value_failed_list.push_back(write_localization_value_failed);
                has_any_write_failed_flag |= write_localization_value_failed;
            }
            break;
        case 1:
            selected_apps_write_localization_value_failed_list_1 = set_localization_values_same_for_all(selected_apps_package_name_list, 0, &selected_apps_localization_value_list);
            for (int i = 0; i < selected_apps_package_name_list.size(); ++i) {
                selected_apps_write_localization_value_failed_list_1[i] = !selected_apps_write_localization_value_failed_list_1[i];
            }

            selected_apps_localization_value_list = get_localization_values(selected_apps_package_name_list);
            for (int i = 0; i < selected_apps_package_name_list.size(); ++i) {
                bool write_localization_value_failed_1 = selected_apps_write_localization_value_failed_list_1[i];
                bool write_localization_value_failed_2 = (selected_apps_localization_value_list[i] != 0);
                bool write_localization_value_failed = (write_localization_value_failed_1 || write_localization_value_failed_2);
                selected_apps_write_localization_value_failed_list_2.push_back(write_localization_value_failed_2);
                selected_apps_write_localization_value_failed_list.push_back(write_localization_value_failed);
                has_any_write_failed_flag |= write_localization_value_failed;
            }
            break;
        case 2: {
            for (int i = 0; i < selected_apps_package_name_list.size(); ++i) {
                bool write_localization_value_failed, write_localization_value_failed_1, write_localization_value_failed_2;
                write_localization_value_failed = false;
                string app_package_name = selected_apps_package_name_list[i];
                int localization_value = selected_apps_localization_value_list[i];
                int localization_value_after;

                if (localization_value == 0 || localization_value == 1) {
                    operation_choice = select_single_item(operation_choice_list_3, format("Do you want to toggle the localization value of selected app to {1}: {0}", app_package_name, !localization_value));
                    operation_choice_str = operation_choice_list_3[operation_choice];
                    cout << format("operation choice: {}", operation_choice_str) << endl;
                    switch (operation_choice) {
                        case 0:
                            localization_value = !localization_value;
                            write_localization_value_failed_1 = !set_localization_value(app_package_name, localization_value);
                            break;
                        case 1:
                            write_localization_value_failed_1 = false;
                            break;
                    }
                } else {
                    operation_choice = select_single_item(operation_choice_list_4, format("Please select an operation to perform on selected app: {0}", app_package_name));
                    operation_choice_str = operation_choice_list_4[operation_choice];
                    cout << format("operation choice: {}", operation_choice_str) << endl;
                    switch (operation_choice) {
                        case 0:
                            localization_value = 1;
                            write_localization_value_failed_1 = !set_localization_value(app_package_name, localization_value);
                            break;
                        case 1:
                            localization_value = 0;
                            write_localization_value_failed_1 = !set_localization_value(app_package_name, localization_value);
                            break;
                        case 2:
                            // Do nothing, keep current state.
                            write_localization_value_failed_1 = false;
                            break;
                    }
                }
                selected_apps_write_localization_value_failed_list_1.push_back(write_localization_value_failed_1);
                localization_value_after = get_localization_value(app_package_name);
                selected_apps_localization_value_list[i] = localization_value_after;
                
                // 这个地方不健壮，operation_choice == 2 并不能保证一直会如此。只是目前是这样，且大概率不会变。这里属于高耦合。
                if (operation_choice != 2 && localization_value_after != localization_value) {
                    write_localization_value_failed_2 = true;
                } else {
                    write_localization_value_failed_2 = false;
                }
                selected_apps_write_localization_value_failed_list_2.push_back(write_localization_value_failed_2);

                write_localization_value_failed = (write_localization_value_failed_1 || write_localization_value_failed_2);
                selected_apps_write_localization_value_failed_list.push_back(write_localization_value_failed);
                has_any_write_failed_flag |= write_localization_value_failed;
            }
            break;
        }
        case 3:
            // Do nothing, passed.
            for (int i = 0; i < selected_apps_package_name_list.size(); ++i) {
                selected_apps_write_localization_value_failed_list_1.push_back(false);
                selected_apps_write_localization_value_failed_list_2.push_back(false);
                selected_apps_write_localization_value_failed_list.push_back(false);
            }
            has_any_write_failed_flag = false;   // default is false anyway.
            break;
    }

    if (has_any_write_failed_flag) {
        cerr << "Setting the localization state of some apps failed." << endl;
        for (int i = 0; i < selected_apps_package_name_list.size(); ++i) {
            string app_package_name = selected_apps_package_name_list[i];
            bool write_localization_value_failed = selected_apps_write_localization_value_failed_list[i];
            bool write_localization_value_failed_1 = selected_apps_write_localization_value_failed_list_1[i];
            bool write_localization_value_failed_2 = selected_apps_write_localization_value_failed_list_2[i];
            if (write_localization_value_failed) {
                cerr << format("app package name: {}, write localization value failed: {} = {} || {}", app_package_name, write_localization_value_failed, write_localization_value_failed_1, write_localization_value_failed_2) << endl;
            } else {
                clog << format("app package name: {}, write localization value failed: {}", app_package_name, write_localization_value_failed) << endl;
            }
        }
        try_pause();
        return EXIT_FAILURE;
    } else {
        cout << "Setting the localization state of all selected apps successfully." << endl;
    }


    #undef exit_on_error
    /*--- divide line ---*/

    try_pause();
    #undef try_pause
    return EXIT_SUCCESS;
}
