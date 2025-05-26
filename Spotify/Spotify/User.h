#ifndef USER_H
#define USER_H

#include <string>
#include"sqlite3.h"
#include<vector>
enum class PlayMode {
    IN_ORDER,
    RANDOM,
    REPEAT_ONE,
    LOOP_ALL
};

struct SimpleSong {
    int id;
    std::string title;
    std::string artist;
    std::string genre;
    
};


class User {
private:
    std::string username;
    std::string password;
    int user_id;
    

public:
    User(); 
    bool login(sqlite3* db);
    bool signUp(sqlite3* db);
    void userMenu(sqlite3* db);
    void viewOrSearchSongs(sqlite3* db);
    void viewOrSearchPlaylists(sqlite3* db);
    int getUserId();
    void saveSong(sqlite3* db);
    void viewSavedSongs(sqlite3* db);
    void debugCheckUserExists(sqlite3* db, int userId);
    void debugCheckSongExists(sqlite3* db, int songId);
    void likeSong(sqlite3* db);
    void viewLikedSongs(sqlite3* db);
    void likePlaylist(sqlite3* db);
    void viewLikedPlaylists(sqlite3* db);
    void removeSavedSong(sqlite3* db);
    void removeLikedSong(sqlite3* db);
    void removeLikedPlaylist(sqlite3* db);
    void createPlaylist(sqlite3* db);
    void viewMyPlaylists(sqlite3* db);
    void deleteMyPlaylist(sqlite3* db);
    void addSongToMyPlaylist(sqlite3* db);
    void viewSongsInMyPlaylist(sqlite3* db);
    void removeSongFromMyPlaylist(sqlite3* db);
    void viewSongsInLikedPlaylist(sqlite3* db);
    void viewSongsInAnyPlaylist(sqlite3* db);
    void playPlaylist(const std::vector<SimpleSong>& songs, PlayMode mode);
    void viewArtistProfiles(sqlite3* db);
    void viewSongsByArtist(sqlite3* db);
    void viewSongsByGenre(sqlite3* db);
    void viewSongsByReleaseDate(sqlite3* db);
    void viewSongsInAlphabeticalOrder(sqlite3* db);
    void viewAllSongs(sqlite3* db);
    
    
};

#endif

