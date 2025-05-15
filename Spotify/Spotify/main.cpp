#include <iostream>
#include "admin.h"

using namespace std;

int main() {
    int choice;

    while (true) {
        cout << "\n--- Spotify Menu ---\n";
        cout << "1. Login as Admin\n";
        cout << "2. Login as User (not implemented yet)\n";
        cout << "3. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        if (choice == 1) {
            Admin admin;
            admin.login();
        }
        else if (choice == 2) {
            cout << "User login is not available yet.\n";
        }
        else if (choice == 3) {
            cout << "Exiting...\n";
            break;
        }
        else {
            cout << "Invalid choice. Try again.\n";
        }
    }

    return 0;
}
