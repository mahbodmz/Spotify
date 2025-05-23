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
    int artistId;  // -1 means not linked to any artist

public:
    Playlist();  // Default constructor
    void createPlaylist(sqlite3* db);
    // You’ll later add: addSongToPlaylist(), deleteSongFromPlaylist(), deletePlaylist(), etc.
};
#endif
