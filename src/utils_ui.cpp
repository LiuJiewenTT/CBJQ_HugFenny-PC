#include "utils_ui.hpp"


int select_single_item(const std::vector<string> &items, const string &title) {
    int retv_fail = -1;
    if (items.empty()) {
        return retv_fail;
    }

    u_char key;
    std::vector<bool> highlights(items.size() + 1, 0);
    int current_position = 0;
    COORD start_coord;
    COORD cursor_pos;

    cout << title << endl;
    start_coord = getCursorPosition();

    while (true) {
        display_menu(items, highlights, current_position, start_coord);
        key = getch();
        switch (key) {
            case 13: // Enter
                highlights[current_position] = 1; // highlight current item
                display_menu(items, highlights, current_position, start_coord);
                cursor_pos = start_coord;
                cursor_pos.Y += items.size();
                setCursorPosition(cursor_pos);
                return current_position;
                break;
            case 224: {
                key = getch();
                switch (key) {
                    case 72: // Up
                        current_position = (current_position - 1 + items.size()) % items.size();
                        break;
                    case 80: // Down
                        current_position = (current_position + 1) % items.size();
                        break;
                }
                break;
            }
            default:
                break;
        }
    }
    return retv_fail; // Not reached
}


string select_single_item_str(const std::vector<string> &items, const string &title) {
    int index = select_single_item(items, title);
    if (index < 0) {
        return "";
    }
    return items[index];
}


std::vector<int> select_multiple_items(const std::vector<string> &items, const string &title) {
    if (items.empty()) {
        return {};
    }

    std::vector<int> selected_items;
    std::vector<string> available_items = items;
    std::vector<bool> highlights(available_items.size() + 1, 0);
    u_char key;
    int current_position = 0;
    COORD start_coord;
    COORD cursor_pos;

    available_items.insert(available_items.begin(), "(All)");

    cout << title << endl;
    start_coord = getCursorPosition();

    while (true) {
        display_menu(available_items, highlights, current_position, start_coord);
        key = getch();
        switch (key) {
            case 13: // Enter
                if (highlights.empty()) {
                    selected_items.clear(); // empty vector
                } else {
                    if (highlights[0]) {
                        // all items selected
                        for (int i = 1; i < highlights.size(); ++i) {
                            selected_items.push_back(i - 1);
                        }
                    } else {
                        for (int i = 1; i < highlights.size(); ++i) {
                            if (highlights[i]) {
                                selected_items.push_back(i - 1);
                            }
                        }
                    }
                }
                cursor_pos = start_coord;
                cursor_pos.Y += available_items.size();
                setCursorPosition(cursor_pos);
                return selected_items;
                break;
            case 224: {
                key = getch();
                switch (key) {
                    case 72: // Up
                        current_position = (current_position - 1 + available_items.size()) % available_items.size();
                        break;
                    case 80: // Down
                        current_position = (current_position + 1) % available_items.size();
                        break;
                }
                break;
            }
            case 32: // Space
                highlights[current_position] = !highlights[current_position]; // toggle highlight
                break;
            default:
                break;
        }
    }
    return selected_items; // Not reached
}


std::vector<string> select_multiple_items_str(const std::vector<string> &items, const string &title) {
    if (items.empty()) {
        return {};
    }

    std::vector<int> selected_items;
    std::vector<string> selected_items_str;
    selected_items = select_multiple_items(items, title);
    for (int i : selected_items) {
        selected_items_str.push_back(items[i]);
    }
    return selected_items_str;
}


void display_menu(const std::vector<string> &items, const std::vector<bool> &highlights, const int &current_position, const COORD &start_coord) {
    int length = items.size();
    for (int i = 0; i < length; ++i) {
        if (highlights[i]) {
            cout << (i == current_position ? "> " : "  ") << "[√] " << items[i] << endl;
        } else {
            cout << (i == current_position ? "> " : "  ") << "[ ] " << items[i] << endl;
        }
    }
    setCursorPosition(start_coord);
}

