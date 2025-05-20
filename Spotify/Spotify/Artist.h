#ifndef ARTIST_H
#define ARTIST_H

#include <string>
using namespace std;

class Artist {
private:
    string name;
    int numAlbums;
    int numSongs;
    string releasedSongs;
public:
    Artist(string n, int albums, int songs, string released);
    bool saveToDatabase();
};

#endif
