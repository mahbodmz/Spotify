#ifndef PLAYLIST_H
#define PLAYLIST_H

#include "sqlite3.h"
#include <string>
using namespace std;

class Playlist {
private:
    int id;
    string name;
    int numSongs;
    int artistId;  

public:
    Playlist();  
    void createPlaylist(sqlite3* db);
    
};
#endif
