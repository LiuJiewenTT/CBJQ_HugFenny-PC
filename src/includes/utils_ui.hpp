#pragma once

#include "utils.hpp"
#include <conio.h>
// #include <numeric>


int select_single_item(const std::vector<string> &items, const string &title);
string select_single_item_str(const std::vector<string> &items, const string &title);


std::vector<int> select_multiple_items(const std::vector<string> &items, const string &title);
std::vector<string> select_multiple_items_str(const std::vector<string> &items, const string &title);


void display_menu(const std::vector<string> &items, const std::vector<bool> &highlights, const int &current_position, const COORD &start_coord);

