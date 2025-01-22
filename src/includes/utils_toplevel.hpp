#pragma once

#include "utils.hpp"
#include <conio.h>
#include <iterator>


std::vector<string> select_apps(const std::vector<string> &apps);


void display_menu(const std::vector<string> &items, const std::vector<bool> &highlights, const int &current_position, const COORD &start_coord);

