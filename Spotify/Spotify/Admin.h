#ifndef ADMIN_H
#define ADMIN_H
#include "sqlite3.h"
#include <string>

class Admin {
private:
    std::string username;
    std::string password;
    void displayAllArtists(sqlite3* db);

public:
    Admin();
    bool login();
    void adminMenu(sqlite3* db);
    void addSong(sqlite3* db);
    void displaySongsByArtist(sqlite3* db, int artistId);
};

#endif
