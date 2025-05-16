#include "artist.h"
#include "sqlite3.h"
#include <iostream>

Artist::Artist(const string& name, int albums, int songs)
    : name(name), numAlbums(albums), numSongs(songs) {
}

string Artist::getName() const {
    return name;
}

void Artist::saveToDatabase() {
    sqlite3* db;
    int exit = sqlite3_open("spotify.db", &db);
    if (exit != SQLITE_OK) {
        cerr << "Failed to open database: " << sqlite3_errmsg(db) << endl;
        return;
    }

    string sql = "INSERT INTO artist (name, num_albums, num_songs) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, numAlbums);
        sqlite3_bind_int(stmt, 3, numSongs);

        if (sqlite3_step(stmt) == SQLITE_DONE)
            cout << "Artist saved successfully.\n";
        else
            cerr << "Failed to save artist.\n";
    }
    else {
        cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}
