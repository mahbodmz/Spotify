#include "admin.h"
#include <iostream>
#include "sqlite3.h"

using namespace std;

Admin::Admin() {}

bool Admin::login() {
    sqlite3* db;
    sqlite3_stmt* stmt;
    int exit = sqlite3_open("spotify.db", &db);

    if (exit) {
        cout << "Failed to open database: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    string inputUsername, inputPassword;
    cout << "Enter admin username: ";
    cin >> inputUsername;
    cout << "Enter admin password: ";
    cin >> inputPassword;

    const char* sql = "SELECT * FROM admin WHERE username = ? AND password = ?;";
    exit = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (exit != SQLITE_OK) {
        cout << "Failed to prepare statement.\n";
        sqlite3_close(db);
        return false;
    }

    sqlite3_bind_text(stmt, 1, inputUsername.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, inputPassword.c_str(), -1, SQLITE_STATIC);

    bool loginSuccess = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        cout << "Login successful. Welcome, " << inputUsername << "!\n";
        loginSuccess = true;
    }
    else {
        cout << "Login failed. Invalid credentials.\n";
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return loginSuccess;
}
void Admin::displayAllArtists(sqlite3* db) {
    const char* sql = "SELECT id, name, num_albums, num_songs FROM artist;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        cout << "\nAvailable Artists:\n";
        cout << "-----------------------------\n";
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            const unsigned char* name = sqlite3_column_text(stmt, 1);
            int numAlbums = sqlite3_column_int(stmt, 2);
            int numSongs = sqlite3_column_int(stmt, 3);

            cout << "ID: " << id << "\n"
                << "Name: " << name << "\n"
                << "Albums: " << numAlbums << ", Songs: " << numSongs << "\n"
                << "-----------------------------\n";
        }
        sqlite3_finalize(stmt);
    }
    else {
        cerr << "Failed to fetch artists.\n"<<sqlite3_errmsg(db) << endl;
    }
}

void Admin::adminMenu(sqlite3* db) {
    int adminChoice;
    while (true) {
        cout << "\n--- Admin Menu ---\n";
        cout << "1. Add Song\n";
        cout << "2. Add Artist\n";
        cout << "3. View All Artists\n";
        cout << "4. Log out\n";
        cout << "Enter choice: ";
        cin >> adminChoice;

        switch (adminChoice) {
        case 1:
            cout << "Add Song functionality\n";
            break;
        case 2:
            cout << "Add Artist functionality\n";
            break;
        case 3:
            displayAllArtists(db);  // Calling the method from Admin class
            break;
        case 4:
            cout << "Logging out...\n";
            return;  // Exit the admin menu and return to main menu
        default:
            cout << "Invalid choice, please try again.\n";
            break;
        }
    }
}
