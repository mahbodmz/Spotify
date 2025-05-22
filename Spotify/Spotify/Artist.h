#ifndef ARTIST_H
#define ARTIST_H

#include <string>


class Artist {
private:
    std::string name;
    int numAlbums;
    int numSongs;
    std::string releasedSongs;
public:
    Artist(std::string n, int albums, int songs, std::string released);
    bool saveToDatabase();  
};

#endif
