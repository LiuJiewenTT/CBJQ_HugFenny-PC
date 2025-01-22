#include "utils_toplevel.hpp"


std::vector<string> select_apps(const std::vector<string> &apps) {
    std::vector<string> selected_apps;
    std::vector<string> available_items = apps;
    std::vector<bool> highlights(available_items.size() + 1, 0);
    COORD start_coord = getCursorPosition();

    available_items.insert(available_items.begin(), "(All)");

    u_char key;
    int current_position = 0;
    COORD cursor_pos;

    while (true) {
        display_menu(available_items, highlights, current_position, start_coord);
        key = getch();
        switch (key) {
            case 13: // Enter
                if (highlights.empty()) {
                    selected_apps.clear(); // empty vector
                } else {
                    if (highlights[0]) {
                        selected_apps = apps; // all apps selected
                    } else {
                        for (int i = 1; i < highlights.size(); ++i) {
                            if (highlights[i]) {
                                selected_apps.push_back(available_items[i]);
                            }
                        }
                    }
                }
                cursor_pos = start_coord;
                cursor_pos.Y += available_items.size() + 1;
                setCursorPosition(cursor_pos);
                return selected_apps;
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
    return selected_apps; // Not reached
}


void display_menu(const std::vector<string> &items, const std::vector<bool> &highlights, const int &current_position, const COORD &start_coord) {
    cout << "Items to Select:" << endl;
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
