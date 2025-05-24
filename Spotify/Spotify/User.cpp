#include "user.h"
#include <iostream>
#include "Artist.h"
#include "sqlite3.h"
#include "Admin.h"
#include <limits>

using namespace std;


User::User() {
    username = "";
    password = "";
}


bool User::login(sqlite3* db) {
    sqlite3_stmt* stmt;
    cout << "Enter username: ";
    cin >> username;
    cout << "Enter password: ";
    cin >> password;

    const char* selectSQL = "SELECT password FROM users WHERE username = ?;";
    if (sqlite3_prepare_v2(db, selectSQL, -1, &stmt, NULL) != SQLITE_OK) {
        cout << "Failed to prepare SELECT statement: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char* dbPassword = sqlite3_column_text(stmt, 0);
        if (password == (const char*)dbPassword) {
            cout << "Login successful!" << endl;
            sqlite3_finalize(stmt);
            userMenu(db);
            return true;
        }
    }

    cout << "Invalid username or password!" << endl;
    sqlite3_finalize(stmt);
    return false;
}


bool User::signUp(sqlite3* db) {
    sqlite3_stmt* stmt;

    cout << "Enter new username: ";
    cin >> username;

    // Check if username already exists
    const char* checkSQL = "SELECT COUNT(*) FROM users WHERE username = ?;";
    if (sqlite3_prepare_v2(db, checkSQL, -1, &stmt, NULL) != SQLITE_OK) {
        cout << "Failed to prepare SELECT statement: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int count = sqlite3_column_int(stmt, 0);
        if (count > 0) {
            cout << "Username already exists! Please choose a different one." << endl;
            sqlite3_finalize(stmt);
            return false;
        }
    }
    sqlite3_finalize(stmt);

    // Input password
    cout << "Enter password: ";
    cin >> password;

    // Insert new user
    const char* insertSQL = "INSERT INTO users (username, password) VALUES (?, ?);";
    if (sqlite3_prepare_v2(db, insertSQL, -1, &stmt, NULL) != SQLITE_OK) {
        cout << "Failed to prepare INSERT statement: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        cout << "Failed to insert user: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    cout << "Sign-up successful! You can now log in with your username and password." << endl;
    return true;
}

void User::userMenu(sqlite3* db) {
    int userchoice;
    while (true) {
        cout << "\n--- User Menu ---\n";
        cout << "1.View all the songs\n";
        cout << "2.View all Playlists\n";
        cout << "3.Saved songs\n";
        cout << "4.Liked songs\n";
        cout << "5.Liked Playlists\n";
        cout << "6.View my Playlists\n";
        cout << "0.Logout\n";
        cin >> userchoice;
        
        switch (userchoice)
        {
        case 0:
            system("cls");
            cout << "Logging out...\n";
            return;
        case 1: 
            system("cls");
            viewOrSearchSongs(db);
            break;
        
        case 2: 
            system("cls");
            viewOrSearchPlaylists(db);
            break;
        
        default:
            cout << "Invalid choice ! try again\n";
            break;
        }


    }
}

void User::viewOrSearchSongs(sqlite3* db) {
    string searchQuery;
    cout << "\n--- View or Search Songs ---\n";
    cout << "Enter song title to search (or press ENTER to view all): ";
    cin.ignore();
    getline(cin, searchQuery);

    sqlite3_stmt* stmt;
    const char* sql;

    if (!searchQuery.empty()) {
        sql = "SELECT id, title, artist, genre, duration FROM song WHERE title = ?;";
    }
    else {
        sql = "SELECT id, title, artist, genre, duration FROM song;";
    }

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare song query.\n";
        return;
    }

    if (!searchQuery.empty()) {
        sqlite3_bind_text(stmt, 1, searchQuery.c_str(), -1, SQLITE_STATIC);
    }

    cout << "\n--- Songs ---\n";
    bool found = false;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        found = true;
        int id = sqlite3_column_int(stmt, 0);
        string title = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        string artist = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        string genre = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        string duration = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));

        cout << "ID: " << id << " | Title: " << title << " | Artist: " << artist
            << " | Genre: " << genre << " | Duration: " << duration << " min\n";
    }

    if (!found) {
        cout << "No songs found.\n";
    }

    sqlite3_finalize(stmt);
}

void User::viewOrSearchPlaylists(sqlite3* db) {
    string searchQuery;
    cout << "\n--- View or Search Playlists ---\n";
    cout << "Enter playlist name to search (or press ENTER to view all): ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // flush leftover newline
    getline(cin, searchQuery);

    sqlite3_stmt* stmt;
    const char* sql;

    if (!searchQuery.empty()) {
        sql = "SELECT id, name FROM playlist WHERE name LIKE ?;";
    }
    else {
        sql = "SELECT id, name FROM playlist;";
    }

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare playlist query: " << sqlite3_errmsg(db) << endl;
        return;
    }

    if (!searchQuery.empty()) {
        string likeQuery = "%" + searchQuery + "%";
        sqlite3_bind_text(stmt, 1, likeQuery.c_str(), -1, SQLITE_STATIC);
    }

    cout << "\n--- Playlists ---\n";
    bool found = false;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        found = true;
        int id = sqlite3_column_int(stmt, 0);
        string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));

        cout << "ID: " << id << " | Name: " << name << endl;
    }

    if (!found) {
        cout << "No playlists found.\n";
    }

    sqlite3_finalize(stmt);
}










