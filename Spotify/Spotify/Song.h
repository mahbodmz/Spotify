#ifndef SONG_H
#define SONG_H

#include <string>
#include "sqlite3.h"

using namespace std;

class Song {
private:
    string title;
    string artist;
    string releaseDate;
    string genre;
    int duration; 
    string filepath; 
    int artist_id;

public:
    Song(const string& title, const string& artist, const string& releaseDate,
        const string& genre, int duration, const string& filepath, int artist_id);

    void saveToDatabase(sqlite3* db);
};

#endif
