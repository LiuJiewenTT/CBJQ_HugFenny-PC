#include "utils_ui.hpp"


std::vector<string> select_multiple_items(const std::vector<string> &items, const string &title) {
    std::vector<string> selected_items;
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
                        selected_items = items; // all items selected
                    } else {
                        for (int i = 1; i < highlights.size(); ++i) {
                            if (highlights[i]) {
                                selected_items.push_back(available_items[i]);
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

