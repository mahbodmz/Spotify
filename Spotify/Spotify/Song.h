#ifndef SONG_H
#define SONG_H

#include <string>
#include "sqlite3.h"



class Song {
private:
    std::string title;
    std::string artist;
    std::string releaseDate;
    std::string genre;
    int duration;
    std::string filepath;
    int artistId;

public:
    Song(const std::string& title, const std::string& artist, const std::string& releaseDate,
        const std::string& genre, int duration, const std::string& filepath, int artistId);

    void saveToDatabase(sqlite3* db);
};

#endif
