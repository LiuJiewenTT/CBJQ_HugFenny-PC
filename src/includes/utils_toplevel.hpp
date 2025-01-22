#pragma once

#include "utils.hpp"
#include "utils_ui.hpp"


std::vector<string> select_apps(const std::vector<string> &apps);


std::vector<bool> exist_all_localization_files(const std::vector<string> &apps);


bool exist_localization_file(const string &app);


std::vector<int> get_localization_values(const std::vector<string> &apps);


int get_localization_value(const string &app);

