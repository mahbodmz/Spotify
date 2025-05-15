#include "user.h"
#include <iostream>
#include "sqlite3.h"

using namespace std;

// Constructor
User::User() {
    username = "";
    password = "";
}

// Login function
bool User::login() {
    sqlite3* db;
    sqlite3_stmt* stmt;
    int exit;

    // Open the database
    exit = sqlite3_open("spotify.db", &db);
    if (exit) {
        cout << "Error opening database: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    cout << "Enter username: ";
    cin >> username;
    cout << "Enter password: ";
    cin >> password;

    // Prepare the SELECT statement to check if the user exists and verify the password
    const char* selectSQL = "SELECT password FROM users WHERE username = ?;";
    exit = sqlite3_prepare_v2(db, selectSQL, -1, &stmt, NULL);
    if (exit != SQLITE_OK) {
        cout << "Failed to prepare SELECT statement: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        return false;
    }

    // Bind the username to the SQL query
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    // Step through the result
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char* dbPassword = sqlite3_column_text(stmt, 0);
        if (password == (const char*)dbPassword) {
            cout << "Login successful!" << endl;
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            return true;
        }
    }

    cout << "Invalid username or password!" << endl;

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return false;
}

// Sign-up function
bool User::signUp() {
    sqlite3* db;
    sqlite3_stmt* stmt;
    int exit;

    // Open the database
    exit = sqlite3_open("spotify.db", &db);
    if (exit) {
        cout << "Error opening database: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    cout << "Enter new username: ";
    cin >> username;

    // Check if the username already exists
    const char* checkSQL = "SELECT COUNT(*) FROM users WHERE username = ?;";
    exit = sqlite3_prepare_v2(db, checkSQL, -1, &stmt, NULL);
    if (exit != SQLITE_OK) {
        cout << "Failed to prepare SELECT statement: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int count = sqlite3_column_int(stmt, 0);
        if (count > 0) {
            cout << "Username already exists! Please choose a different one." << endl;
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            return false;
        }
    }

    // Get password for sign-up
    cout << "Enter password: ";
    cin >> password;

    // Insert the new user into the database
    const char* insertSQL = "INSERT INTO users (username, password) VALUES (?, ?);";
    exit = sqlite3_prepare_v2(db, insertSQL, -1, &stmt, NULL);
    if (exit != SQLITE_OK) {
        cout << "Failed to prepare INSERT statement: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);

    exit = sqlite3_step(stmt);
    if (exit != SQLITE_DONE) {
        cout << "Failed to insert data into table: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return false;
    }

    cout << "Sign-up successful! You can now log in with your username and password." << endl;

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return true;
}
