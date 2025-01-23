#include "utils_toplevel.hpp"


std::vector<string> select_apps(const std::vector<string> &items) {
    return select_multiple_items_str(items, "Select Applications: ");
}


std::vector<bool> exist_all_localization_files(const std::vector<string> &apps) {
    std::vector<bool> exist_flags;
    for (const auto &app : apps) {
        exist_flags.push_back(exist_localization_file(app));
    }
    return exist_flags;
}


bool exist_localization_file(const string &app) {
    string command_content_str, command_output_str, command_error_str;
    bool command_executed_success_flag = false;
    int command_exit_code = 0;

    command_content_str = format("adb shell \"ls /sdcard/Android/data/{}/files/localization.txt\"", app);
    command_content_str = wrapStringToEscapeCmd(command_content_str);
    execute_command(command_content_str, &command_output_str, &command_error_str, &command_exit_code, &command_executed_success_flag);
    if (!command_executed_success_flag || command_exit_code) {
        return false;
    }

    return true;
}


std::vector<bool> delete_all_localization_files(const std::vector<string> &apps, const std::vector<bool> &delete_flags) {
    std::vector<bool> success_flags;
    for (size_t i = 0; i < apps.size(); ++i) {
        if (delete_flags[i]) {
            success_flags.push_back(delete_localization_file(apps[i]));
        } else {
            success_flags.push_back(true);
        }
    }
    return success_flags;
}


bool delete_localization_file(const string &app) {
    string command_content_str, command_output_str, command_error_str;
    bool command_executed_success_flag = false;
    int command_exit_code = 0;

    command_content_str = format("adb shell \"rm /sdcard/Android/data/{}/files/localization.txt\"", app);
    command_content_str = wrapStringToEscapeCmd(command_content_str);
    execute_command(command_content_str, &command_output_str, &command_error_str, &command_exit_code, &command_executed_success_flag);
    if (!command_executed_success_flag || command_exit_code) {
        return false;
    }

    return true;
}


std::vector<int> get_localization_values(const std::vector<string> &apps) {
    std::vector<int> values;
    for (const auto &app : apps) {
        values.push_back(get_localization_value(app));
    }
    return values;
}


int get_localization_value(const string &app) {
    int retv_fail = -1;
    int retv = retv_fail;
    std::vector<string> string_parts;

    string command_content_str, command_output_str, command_error_str;
    bool command_executed_success_flag = false;
    int command_exit_code = 0;

    command_content_str = format("adb shell \"cat /sdcard/Android/data/{}/files/localization.txt\"", app);
    command_content_str = wrapStringToEscapeCmd(command_content_str);
    execute_command(command_content_str, &command_output_str, &command_error_str, &command_exit_code, &command_executed_success_flag);
    if (!command_executed_success_flag || command_exit_code) {
        return retv_fail;
    }

    ns_string::trim(command_output_str);
    string_parts = ns_string::split(command_output_str, "=");
    if (string_parts.size() < 2) {
        clog << "Error: Invalid localization file content: " << command_output_str << "parts.size(): " << string_parts.size() << endl;
        return retv_fail;
    }

    std::istringstream iss(string_parts[1]);
    iss >> retv;
    if (iss.fail()) {
        clog << "Error: Invalid localization value: " << string_parts[1] << endl;
        return retv_fail;
    }
    clog << "Localization value for " << app << " is " << retv << endl;

    return retv;
}


std::vector<bool> set_localization_values(const std::vector<string> &apps, const std::vector<int> &values, const std::vector<int> *reference_values) {
    std::vector<bool> success_flags;
    for (size_t i = 0; i < apps.size(); ++i) {
        success_flags.push_back(set_localization_value(apps[i], values[i], (reference_values == nullptr ? nullptr : &reference_values->at(i))));
    }
    return success_flags;
}


std::vector<bool> set_localization_values_same_for_all(const std::vector<string> &apps, const int &value, const std::vector<int> *reference_values) {
    std::vector<bool> success_flags;
    for (const auto &app : apps) {
        success_flags.push_back(set_localization_value(app, value, (reference_values == nullptr ? nullptr : &reference_values->at(0))));
    }
    return success_flags;
}


bool set_localization_value(const string &app, const int value, const int *reference_value) {
    string command_content_str, command_output_str, command_error_str;
    bool command_executed_success_flag = false;
    int command_exit_code = 0;

    command_content_str = format("adb shell \"echo localization_value={} > /sdcard/Android/data/{}/files/localization.txt\"", value, app);
    command_content_str = wrapStringToEscapeCmd(command_content_str);
    execute_command(command_content_str, &command_output_str, &command_error_str, &command_exit_code, &command_executed_success_flag);
    if (!command_executed_success_flag || command_exit_code) {
        return false;
    }

    return true;
}

