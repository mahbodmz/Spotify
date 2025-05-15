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
