#ifndef ARTIST_H
#define ARTIST_H

#include <string>
#include "sqlite3.h"

class Artist {
private:
    std::string name;
    int numAlbums;
    int numSongs;
    std::string releasedSongs;
public:
    Artist();
    Artist(std::string n, int albums, int songs, std::string released);
    bool saveToDatabase(); 
    void addArtist(sqlite3* db);
};

#endif
