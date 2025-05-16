#ifndef ARTIST_H
#define ARTIST_H

#include <string>
using namespace std;

class Artist {
private:
    string name;
    int numAlbums;
    int numSongs;

public:
    Artist(const string& name, int albums, int songs);
    void saveToDatabase();
    string getName() const;
};

#endif
