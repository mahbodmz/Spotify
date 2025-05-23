#include "admin.h"
#include <iostream>
#include "sqlite3.h"
#include "artist.h"
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
void Admin::displaySongsByArtist(sqlite3* db, int artistId) {
    const char* sql = "SELECT title, release_date, genre, duration, filepath FROM song WHERE artist_id = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, artistId);

        cout << "\nSongs by Artist ID " << artistId << ":\n";
        cout << "-----------------------------------------\n";

        bool found = false;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            found = true;
            const unsigned char* title = sqlite3_column_text(stmt, 0);
            const unsigned char* releaseDate = sqlite3_column_text(stmt, 1);
            const unsigned char* genre = sqlite3_column_text(stmt, 2);
            int duration = sqlite3_column_int(stmt, 3);
            const unsigned char* filepath = sqlite3_column_text(stmt, 4);

            cout << "Title: " << title << "\n"
                << "Release Date: " << releaseDate << "\n"
                << "Genre: " << genre << "\n"
                << "Duration: " << duration << " sec\n"
                << "Filepath: " << filepath << "\n"
                << "-----------------------------------------\n";
        }

        if (!found) {
            cout << "No songs found for this artist.\n";
        }

        sqlite3_finalize(stmt);
    }
    else {
        cerr << "Failed to fetch songs for artist.\n";
    }
}


void Admin::adminMenu(sqlite3* db) {
    int adminChoice;
    while (true) {
        cout << "\n--- Admin Menu ---\n";
        cout << "1. Add Song\n";
        cout << "2. Add Artist\n";
        cout << "3. View All Artists\n";
        cout << "4. View Songs by Artist\n";
        cout << "5. Log out\n";
        cout << "Enter choice: ";
        cin >> adminChoice;

        switch (adminChoice) {
        case 1:
            addSong(db);
            break;
        case 2: {
            system("cls");
            Artist artist;
            artist.addArtist(db);
            break;
        }
        case 3:
            system("cls");
            displayAllArtists(db);  // Calling the method from Admin class
            break;
        case 4: {
            displayAllArtists(db);  // reuse to help choose artist
            cout << "Enter Artist ID to view their songs (0 to cancel): ";
            int artistId;
            cin >> artistId;
            if (artistId != 0)
                displaySongsByArtist(db, artistId);
            break;
        }
        case 5:
            cout << "Logging out...\n";
            return;  // Exit the admin menu and return to main menu
        default:
            cout << "Invalid choice, please try again.\n";
            break;
        }
    }
}

// In Admin.cpp

void Admin::addSong(sqlite3* db) {
    // Show available artists
    const char* showArtistsSQL = "SELECT id, name FROM artist;";
    sqlite3_stmt* stmt;
    cout << "\nAvailable Artists:\n-------------------\n";

    if (sqlite3_prepare_v2(db, showArtistsSQL, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int artistID = sqlite3_column_int(stmt, 0);
            const unsigned char* artistName = sqlite3_column_text(stmt, 1);
            cout << "ID: " << artistID << " - Name: " << artistName << endl;
        }
        sqlite3_finalize(stmt);
    }
    else {
        cerr << "Failed to fetch artists.\n";
        return;
    }

    int artistID;
    cout << "Enter the ID of the artist this song belongs to: ";
    cin >> artistID;
    cin.ignore();

    // Song details
    string title, artistName, releaseDate, genre, duration, filepath;

    cout << "Enter song title: ";
    getline(cin, title);

    cout << "Enter artist name (for display): ";
    getline(cin, artistName);

    cout << "Enter release date (YYYY-MM-DD): ";
    getline(cin, releaseDate);

    cout << "Enter genre: ";
    getline(cin, genre);

    cout << "Enter duration (e.g. 3:45): ";
    getline(cin, duration);

    cout << "Enter file path: ";
    getline(cin, filepath);

    const char* insertSQL = "INSERT INTO song (title, artist, release_date, genre, duration, filepath, artist_id) VALUES (?, ?, ?, ?, ?, ?, ?);";
    sqlite3_stmt* insertStmt;

    if (sqlite3_prepare_v2(db, insertSQL, -1, &insertStmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(insertStmt, 1, title.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(insertStmt, 2, artistName.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(insertStmt, 3, releaseDate.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(insertStmt, 4, genre.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(insertStmt, 5, duration.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(insertStmt, 6, filepath.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(insertStmt, 7, artistID);

        if (sqlite3_step(insertStmt) == SQLITE_DONE) {
            cout << "Song added successfully.\n";
            const char* updateArtistSQL = "UPDATE artist SET num_songs = num_songs + 1 WHERE id = ?;";
            sqlite3_stmt* updateStmt;

            if (sqlite3_prepare_v2(db, updateArtistSQL, -1, &updateStmt, nullptr) == SQLITE_OK) {
                sqlite3_bind_int(updateStmt, 1, artistID);
                if (sqlite3_step(updateStmt) != SQLITE_DONE) {
                    cerr << "Failed to update artist's song count.\n";
                }
                sqlite3_finalize(updateStmt);
            }
            else {
                cerr << "Failed to prepare update statement.\n";
            }
        }
        else {
            cerr << "Failed to add song.\n";
        }

        sqlite3_finalize(insertStmt);
    }
    else {
        cerr << "Failed to prepare song insert statement.\n";
    }
}

