#include "utils_toplevel.hpp"


std::vector<string> select_apps(const std::vector<string> &items) {
    return select_multiple_items(items, "Select Applications: ");
}

