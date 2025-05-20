#include "artist.h"
#include "sqlite3.h"
#include <iostream>
using namespace std;

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
