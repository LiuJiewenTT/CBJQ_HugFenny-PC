#pragma once

#include "utils.hpp"
#include "utils_ui.hpp"


std::vector<string> select_apps(const std::vector<string> &apps);


std::vector<bool> exist_all_localization_files(const std::vector<string> &apps);


bool exist_localization_file(const string &app);


std::vector<bool> delete_all_localization_files(const std::vector<string> &apps, const std::vector<bool> &delete_flags);


bool delete_localization_file(const string &app);


std::vector<int> get_localization_values(const std::vector<string> &apps);


int get_localization_value(const string &app);


std::vector<bool> set_localization_values(const std::vector<string> &apps, const std::vector<int> &values, const std::vector<int> *reference_values = nullptr);
std::vector<bool> set_localization_values_same_for_all(const std::vector<string> &apps, const int &value, const std::vector<int> *reference_values = nullptr);


bool set_localization_value(const string &app, const int value, const int *reference_value = nullptr);

