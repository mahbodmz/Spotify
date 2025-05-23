#include "Playlist.h"
#include <iostream>
#include "sqlite3.h"

using namespace std;

Playlist::Playlist() {
    name = "empty";
    numSongs = 0;
    artistId = -1;
}

void Playlist::createPlaylist(sqlite3* db) {
    cout << "Enter playlist name: ";
    cin.ignore();
    getline(cin, name);

    string sql = "INSERT INTO playlist (name, num_songs, artist_id) VALUES (?, 0, NULL);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        cerr << "Failed to insert playlist: " << sqlite3_errmsg(db) << endl;
    }
    else {
        cout << "Playlist created successfully.\n";
    }

    sqlite3_finalize(stmt);
}
