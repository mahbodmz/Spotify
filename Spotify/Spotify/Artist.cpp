#include "artist.h"
#include "sqlite3.h"
#include <iostream>
using namespace std;

Artist::Artist() {
    this->name = "empty";
    this->numAlbums = 0;
    this->numSongs = 0;
}

Artist::Artist(string n, int albums, int songs, string released)
    : name(n), numAlbums(albums), numSongs(songs), releasedSongs(released) {
}

bool Artist::saveToDatabase() {
    sqlite3* db;
    char* errorMsg;

    int exit = sqlite3_open("spotify.db", &db);
    if (exit) {
        cerr << "Error opening DB: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    string sql = "INSERT INTO artist (name, num_albums, num_songs, released_songs) VALUES ('" +
        name + "', " + to_string(numAlbums) + ", " + to_string(numSongs) + ", '" + releasedSongs + "');";

    exit = sqlite3_exec(db, sql.c_str(), NULL, 0, &errorMsg);
    if (exit != SQLITE_OK) {
        cerr << "Error inserting artist: " << errorMsg << endl;
        sqlite3_free(errorMsg);
        sqlite3_close(db);
        return false;
    }

    sqlite3_close(db);
    return true;
}
void Artist::addArtist(sqlite3* db) {
    string name;
    cout << "Enter artist name: ";
    cin.ignore();
    getline(cin, name);

    const char* sql = "INSERT INTO artist (name, num_albums, num_songs) VALUES (?, 0, 0);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            cout << "Artist added successfully.\n";
        }
        else {
            cerr << "Failed to add artist: " << sqlite3_errmsg(db) << endl;
        }

        sqlite3_finalize(stmt);
    }
    else {
        cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
    }
}
void Artist::updateNumSongs(sqlite3* db, int artistId) {
    const char* sql = "UPDATE artist SET num_songs = num_songs + 1 WHERE id = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, artistId);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            cerr << "Error updating number of songs for artist with ID: " << artistId << endl;
        }
        sqlite3_finalize(stmt);
    }
    else {
        cerr << "Failed to prepare statement to update num_songs.\n";
    }
}

