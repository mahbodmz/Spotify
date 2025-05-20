#include "song.h"
#include <iostream>

Song::Song(const string& title, const string& artist, const string& releaseDate,
    const string& genre, int duration, const string& filepath, int artist_id)
    : title(title), artist(artist), releaseDate(releaseDate),
    genre(genre), duration(duration), filepath(filepath) , artist_id(artist_id) {}

void Song::saveToDatabase(sqlite3* db) {
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO songs (title, artist, release_date, genre, duration, filepath, artist_id) VALUES (?, ?, ?, ?, ?, ?,?);";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        cout << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_text(stmt, 1, title.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, artist.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, releaseDate.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, genre.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 5, duration);
    sqlite3_bind_text(stmt, 6, filepath.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 7, artist_id);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        cout << "Failed to insert song: " << sqlite3_errmsg(db) << endl;
    }
    else {
        cout << "Song inserted successfully.\n";
    }

    sqlite3_finalize(stmt);
}
