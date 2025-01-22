#pragma once

#include "utils.hpp"
#include <conio.h>
// #include <iterator>


std::vector<string> select_multiple_items(const std::vector<string> &items, const string &title);


void display_menu(const std::vector<string> &items, const std::vector<bool> &highlights, const int &current_position, const COORD &start_coord);

