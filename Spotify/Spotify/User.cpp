#include "user.h"
#include <iostream>
#include "Artist.h"
#include "sqlite3.h"
#include "Admin.h"
#include <limits>

using namespace std;


User::User() {
    username = "";
    password = "";
}


bool User::login(sqlite3* db) {
    sqlite3_stmt* stmt;
    cout << "Enter username: ";
    cin >> username;
    cout << "Enter password: ";
    cin >> password;

    const char* selectSQL = "SELECT id, password FROM users WHERE username = ?;";
    if (sqlite3_prepare_v2(db, selectSQL, -1, &stmt, NULL) != SQLITE_OK) {
        cout << "Failed to prepare SELECT statement: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int dbUserId = sqlite3_column_int(stmt, 0);  
        const unsigned char* dbPassword = sqlite3_column_text(stmt, 1); 

        if (password == (const char*)dbPassword) {
            user_id = dbUserId;
            cout << "Login successful. Your user ID is " << user_id << endl;
            sqlite3_finalize(stmt);
            userMenu(db);
            return true;
        }
    }

    cout << "Invalid username or password!" << endl;
    sqlite3_finalize(stmt);
    return false;
}
int User::getUserId() {
    return user_id;
}


bool User::signUp(sqlite3* db) {
    sqlite3_stmt* stmt;

    cout << "Enter new username: ";
    cin >> username;

    
    const char* checkSQL = "SELECT COUNT(*) FROM users WHERE username = ?;";
    if (sqlite3_prepare_v2(db, checkSQL, -1, &stmt, NULL) != SQLITE_OK) {
        cout << "Failed to prepare SELECT statement: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int count = sqlite3_column_int(stmt, 0);
        if (count > 0) {
            cout << "Username already exists! Please choose a different one." << endl;
            sqlite3_finalize(stmt);
            return false;
        }
    }
    sqlite3_finalize(stmt);

    
    cout << "Enter password: ";
    cin >> password;

    
    const char* insertSQL = "INSERT INTO users (username, password) VALUES (?, ?);";
    if (sqlite3_prepare_v2(db, insertSQL, -1, &stmt, NULL) != SQLITE_OK) {
        cout << "Failed to prepare INSERT statement: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        cout << "Failed to insert user: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return false;
    }

    user_id = static_cast<int>(sqlite3_last_insert_rowid(db));  
    cout << "Sign-up successful. Your user ID is " << user_id << endl;
    sqlite3_finalize(stmt);

    
    return true;
}

void User::userMenu(sqlite3* db) {
    int userchoice;
    while (true) {
        cout << "\n--- User Menu ---\n";
        cout << "1.View all the songs\n";
        cout << "2.View all Playlists\n";
        cout << "3.Saved songs\n";
        cout << "4.Save a song\n";
        cout << "5.Liked songs\n";
        cout << "6.Like a song\n";
        cout << "7.Liked Playlists\n";
        cout << "8.Like a Playlist\n";
        cout << "9.Create new Playlist\n";
        cout << "10.View my Playlists\n";
        cout << "0.Logout\n";
        cin >> userchoice;
        
        switch (userchoice)
        {
        case 0:
            system("cls");
            cout << "Logging out...\n";
            return;
        case 1: 
            system("cls");
            viewOrSearchSongs(db);
            break;
        
        case 2: {
            int choice;
            system("cls");
            viewAllPlaylists(db);
            cout << "1.View songs in Playlist\n";
            cout << "2.Exit\n";
            cin >> choice;
            switch (choice)
            {
            case 1:
                viewSongsInAnyPlaylist( db);
                break;
            case 2:
                break;
            default:
                cout << "Invalid choice!!\n";
                break;
            }
            break;
        }
        case 3: {
            system("cls");
            int choice;
            viewSavedSongs(db);
            cout << "Do you want to delete a Song ?(1.Yes , etc.No)" << endl;
            cin >> choice;
            if (choice == 1)
            {
                removeSavedSong(db);
            }
            break;
        }
        case 4:
            system("cls");
            saveSong(db);
            break;
        case 5: {
            system("cls");
            int choice;
            viewLikedSongs(db);
            cout << "Do you want to delete a Song ?(1.Yes , etc.No)" << endl;
            cin >> choice;
            if (choice == 1)
            {
                removeLikedSong( db);
            }
            break;
        }
        case 6:
            system("cls");
            likeSong( db);
            break;
        case 7: {
            system("cls");
            int choice;
            viewLikedPlaylists(db);
            cout << "1.Remove a Playlist\n";
            cout << "2.View the songs of a Playlist\n";
            cin >> choice;
            switch (choice)
            {
            case 1:
                removeLikedPlaylist( db);
                break;
            case 2:
                viewSongsInLikedPlaylist( db);
                break;
            default:
                cout << "Invalid choice!!\n";
                break;
            }
            break;
        }
        case 8:
            system("cls");
            likePlaylist( db);
            break;
        case 9:
            system("cls");
            createPlaylist( db);
            break;
        case 10: {
            system("cls");
            int choice;
            viewMyPlaylists(db);
            cout << "1.Delete a Playlist\n";
            cout << "2.Add Songs to Playlist\n";
            cout << "3.View Songs in Playlist\n";
            cout << "4.Back to menu\n";
            cin >> choice;
            switch (choice)
            {
            case 1:
                deleteMyPlaylist(db);
                break;
            case 2:
                addSongToMyPlaylist( db);
                break;
            case 3: {
                int choice;
                viewSongsInMyPlaylist(db);
                cout << "1.Delete a Song\n";
                cin >> choice;
                switch (choice)
                {
                case 1:
                    removeSongFromMyPlaylist( db);
                    break;
                default:
                    cout << "Invalid choice!!\n";
                    break;
                }
                break;
            }
            case 4:
                system("cls");
                break;
            default:
                cout << "Invalid Choice!\n";
                break;
            }
            break;
        }
        default:
            system("cls");
            cout << "Invalid choice ! try again\n";
            break;
        }


    }
}

void User::viewOrSearchSongs(sqlite3* db) {
    string searchQuery;
    cout << "\n--- View or Search Songs ---\n";
    cout << "Enter song title to search (or press ENTER to view all): ";
    cin.ignore();
    getline(cin, searchQuery);

    sqlite3_stmt* stmt;
    const char* sql;

    if (!searchQuery.empty()) {
        sql = "SELECT id, title, artist, genre, duration FROM song WHERE title = ?;";
    }
    else {
        sql = "SELECT id, title, artist, genre, duration FROM song;";
    }

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare song query.\n";
        return;
    }

    if (!searchQuery.empty()) {
        sqlite3_bind_text(stmt, 1, searchQuery.c_str(), -1, SQLITE_STATIC);
    }

    cout << "\n--- Songs ---\n";
    bool found = false;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        found = true;
        int id = sqlite3_column_int(stmt, 0);
        string title = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        string artist = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        string genre = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        string duration = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));

        cout << "ID: " << id << " | Title: " << title << " | Artist: " << artist
            << " | Genre: " << genre << " | Duration: " << duration << " min\n";
    }

    if (!found) {
        cout << "No songs found.\n";
    }

    sqlite3_finalize(stmt);
}

void User::viewAllPlaylists(sqlite3* db) {
    sqlite3_stmt* stmt;
    const char* sql = "SELECT id, name FROM playlist;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare playlist query: " << sqlite3_errmsg(db) << endl;
        return;
    }

    cout << "\n--- Playlists ---\n";
    bool found = false;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        found = true;
        int id = sqlite3_column_int(stmt, 0);
        string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        cout << "ID: " << id << " | Name: " << name << endl;
    }

    if (!found) {
        cout << "No playlists found.\n";
    }

    sqlite3_finalize(stmt);
}
void User:: debugCheckSongExists(sqlite3* db, int songId) {
    sqlite3_stmt* stmt;
    const char* sql = "SELECT COUNT(*) FROM song WHERE id = ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, songId);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            int count = sqlite3_column_int(stmt, 0);
            cout << "Song ID " << songId << (count > 0 ? " exists." : " does NOT exist.") << endl;
        }
    }
    sqlite3_finalize(stmt);
}

void User:: debugCheckUserExists(sqlite3* db, int userId) {
    sqlite3_stmt* stmt;
    const char* sql = "SELECT COUNT(*) FROM users WHERE id = ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, userId);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            int count = sqlite3_column_int(stmt, 0);
            cout << "User ID " << userId << (count > 0 ? " exists." : " does NOT exist.") << endl;
        }
    }
    sqlite3_finalize(stmt);
}





void User::saveSong(sqlite3* db) {
    int song_id;
    

    
    cout << "Enter the ID of the song you want to save: ";
    cin >> song_id;
    debugCheckSongExists(db, song_id);
    debugCheckUserExists(db, getUserId());

    
    sqlite3_stmt* stmt;
    const char* checkSQL = "SELECT COUNT(*) FROM SavedSongs WHERE user_id = ? AND song_id = ?;";

    if (sqlite3_prepare_v2(db, checkSQL, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare check statement: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_int(stmt, 1, getUserId());  
    sqlite3_bind_int(stmt, 2, song_id);  

    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        int count = sqlite3_column_int(stmt, 0);
        if (count > 0) {
            cout << "This song is already saved to your account." << endl;
        }
        else {
            
            const char* insertSQL = "INSERT INTO SavedSongs (user_id, song_id) VALUES (?, ?);";
            if (sqlite3_prepare_v2(db, insertSQL, -1, &stmt, nullptr) != SQLITE_OK) {
                cerr << "Failed to prepare insert statement: " << sqlite3_errmsg(db) << endl;
                return;
            }

            sqlite3_bind_int(stmt, 1, getUserId());  
            sqlite3_bind_int(stmt, 2, song_id);  

            rc = sqlite3_step(stmt);
            if (rc == SQLITE_DONE) {
                cout << "Song successfully saved!" << endl;
            }
            else {
                cout << "Failed to save song: " << sqlite3_errmsg(db) << endl;
            }
        }
    }
    else {
        cerr << "Error checking for existing saved song: " << sqlite3_errmsg(db) << endl;
    }

    sqlite3_finalize(stmt);
}


void User::viewSavedSongs(sqlite3* db) {
    sqlite3_stmt* stmt;
    const char* sql = "SELECT song.id, song.title, song.artist FROM song "
        "JOIN SavedSongs ON song.id = SavedSongs.song_id "
        "WHERE SavedSongs.user_id = ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare saved songs query: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_int(stmt, 1, getUserId()); 

    cout << "\n--- Saved Songs ---\n";
    bool found = false;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        found = true;
        int id = sqlite3_column_int(stmt, 0);
        string title = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        string artist = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));

        cout << "ID: " << id << " | Title: " << title << " | Artist: " << artist << endl;
    }

    if (!found) {
        cout << "You have no saved songs." << endl;
    }

    sqlite3_finalize(stmt);
}

void User::likeSong(sqlite3* db) {
    int songId;
    cout << "Enter the Song ID to like: ";
    cin >> songId;

    const char* sql = "INSERT INTO LikedSongs (user_id, song_id) VALUES (?, ?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        cout << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_int(stmt, 1, user_id);
    sqlite3_bind_int(stmt, 2, songId);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        cout << "Failed to like song: " << sqlite3_errmsg(db) << endl;
    }
    else {
        cout << "Song liked successfully!" << endl;
    }

    sqlite3_finalize(stmt);
}

void User::viewLikedSongs(sqlite3* db) {
    const char* sql = R"(
        SELECT song.id, song.title, song.artist, song.genre, song.release_date
        FROM song
        JOIN LikedSongs ON song.id = LikedSongs.song_id
        WHERE LikedSongs.user_id = ?;
    )";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        cout << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_int(stmt, 1, user_id);

    cout << "--- Liked Songs ---\n";
    bool found = false;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        found = true;
        int id = sqlite3_column_int(stmt, 0);
        const unsigned char* title = sqlite3_column_text(stmt, 1);
        const unsigned char* artist = sqlite3_column_text(stmt, 2);
        const unsigned char* genre = sqlite3_column_text(stmt, 3);
        const unsigned char* release_date = sqlite3_column_text(stmt, 4);

        cout << "ID: " << id << ", Title: " << title
            << ", Artist: " << artist << ", Genre: " << genre
            << ", Release Date: " << release_date << endl;
    }

    if (!found) {
        cout << "You haven't liked any songs yet.\n";
    }

    sqlite3_finalize(stmt);
}

void User::likePlaylist(sqlite3* db) {
    int playlistId;
    cout << "Enter the Playlist ID you want to like: ";
    cin >> playlistId;

    const char* insertSQL = "INSERT INTO LikedPlaylists (user_id, playlist_id) VALUES (?, ?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, insertSQL, -1, &stmt, nullptr) != SQLITE_OK) {
        cout << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_int(stmt, 1, user_id);
    sqlite3_bind_int(stmt, 2, playlistId);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        cout << "Playlist liked successfully.\n";
    }
    else {
        cout << "Failed to like playlist: " << sqlite3_errmsg(db) << endl;
    }

    sqlite3_finalize(stmt);
}


void User::viewLikedPlaylists(sqlite3* db) {
    const char* query = R"(
        SELECT playlist.id, playlist.name
        FROM LikedPlaylists
        JOIN playlist ON LikedPlaylists.playlist_id = playlist.id
        WHERE LikedPlaylists.user_id = ?;
    )";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK) {
        cout << "Failed to prepare SELECT: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_int(stmt, 1, user_id);

    bool found = false;
    cout << "--- Your Liked Playlists ---\n";
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char* name = (const char*)sqlite3_column_text(stmt, 1);
        cout << "ID: " << id << " | Name: " << name << endl;
        found = true;
    }

    if (!found) {
        cout << "You haven't liked any playlists yet.\n";
    }

    sqlite3_finalize(stmt);
}
void User::removeSavedSong(sqlite3* db) {
    int songId;
    cout << "Enter the ID of the song you want to remove from your saved songs: ";
    cin >> songId;

    sqlite3_stmt* stmt;
    const char* deleteSQL = "DELETE FROM SavedSongs WHERE user_id = ? AND song_id = ?;";

    if (sqlite3_prepare_v2(db, deleteSQL, -1, &stmt, nullptr) != SQLITE_OK) {
        cout << "Failed to prepare DELETE statement: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_int(stmt, 1, user_id);  
    sqlite3_bind_int(stmt, 2, songId);   

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        if (sqlite3_changes(db) > 0) {
            cout << "Song removed from your saved list.\n";
        }
        else {
            cout << "That song was not in your saved list.\n";
        }
    }
    else {
        cout << "Failed to remove song: " << sqlite3_errmsg(db) << endl;
    }

    sqlite3_finalize(stmt);
}

void User::removeLikedSong(sqlite3* db) {
    int songId;
    cout << "Enter the ID of the song you want to remove from your liked songs: ";
    cin >> songId;

    sqlite3_stmt* stmt;
    const char* deleteSQL = "DELETE FROM LikedSongs WHERE user_id = ? AND song_id = ?;";

    if (sqlite3_prepare_v2(db, deleteSQL, -1, &stmt, nullptr) != SQLITE_OK) {
        cout << "Failed to prepare DELETE statement: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_int(stmt, 1, user_id);  
    sqlite3_bind_int(stmt, 2, songId);   

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        if (sqlite3_changes(db) > 0) {
            cout << "Song removed from your liked songs.\n";
        }
        else {
            cout << "That song was not in your liked songs.\n";
        }
    }
    else {
        cout << "Failed to remove liked song: " << sqlite3_errmsg(db) << endl;
    }

    sqlite3_finalize(stmt);
}

void User::removeLikedPlaylist(sqlite3* db) {
    int playlistId;
    cout << "Enter the ID of the playlist you want to remove from your liked playlists: ";
    cin >> playlistId;

    sqlite3_stmt* stmt;
    const char* deleteSQL = "DELETE FROM LikedPlaylists WHERE user_id = ? AND playlist_id = ?;";

    if (sqlite3_prepare_v2(db, deleteSQL, -1, &stmt, nullptr) != SQLITE_OK) {
        cout << "Failed to prepare DELETE statement: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_int(stmt, 1, user_id);       
    sqlite3_bind_int(stmt, 2, playlistId);    

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        if (sqlite3_changes(db) > 0) {
            cout << "Playlist removed from your liked playlists.\n";
        }
        else {
            cout << "That playlist was not in your liked playlists.\n";
        }
    }
    else {
        cout << "Failed to remove liked playlist: " << sqlite3_errmsg(db) << endl;
    }

    sqlite3_finalize(stmt);
}


void User::createPlaylist(sqlite3* db) {
    string playlistName;
    cout << "Enter a name for your new playlist: ";
    cin.ignore(); 
    getline(cin, playlistName);

    sqlite3_stmt* stmt;
    const char* insertSQL = "INSERT INTO playlist (name, num_songs, artist_id, creator_type, creator_id) VALUES (?, 0, NULL, 'user', ?);";

    if (sqlite3_prepare_v2(db, insertSQL, -1, &stmt, nullptr) != SQLITE_OK) {
        cout << "Failed to prepare INSERT statement: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_text(stmt, 1, playlistName.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, user_id);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        cout << "Playlist \"" << playlistName << "\" created successfully.\n";
    }
    else {
        cout << "Failed to create playlist: " << sqlite3_errmsg(db) << endl;
    }

    sqlite3_finalize(stmt);
}
void User::viewMyPlaylists(sqlite3* db) {
    sqlite3_stmt* stmt;
    const char* selectSQL = "SELECT id, name, num_songs FROM playlist WHERE creator_type = 'user' AND creator_id = ?;";

    if (sqlite3_prepare_v2(db, selectSQL, -1, &stmt, nullptr) != SQLITE_OK) {
        cout << "Failed to prepare SELECT statement: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_int(stmt, 1, user_id);

    cout << "\n--- Your Playlists ---\n";
    bool found = false;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const unsigned char* name = sqlite3_column_text(stmt, 1);
        int numSongs = sqlite3_column_int(stmt, 2);

        cout << "ID: " << id << " | Name: " << name << " | Songs: " << numSongs << endl;
        found = true;
    }

    if (!found) {
        cout << "You haven't created any playlists yet.\n";
    }

    sqlite3_finalize(stmt);
}

void User::deleteMyPlaylist(sqlite3* db) {
    int playlistId;
    cout << "\nEnter the ID of the playlist you want to delete: ";
    cin >> playlistId;

    
    sqlite3_stmt* checkStmt;
    const char* checkSQL = "SELECT COUNT(*) FROM playlist WHERE id = ? AND creator_type = 'user' AND creator_id = ?;";
    if (sqlite3_prepare_v2(db, checkSQL, -1, &checkStmt, nullptr) != SQLITE_OK) {
        cout << "Failed to prepare check statement: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_int(checkStmt, 1, playlistId);
    sqlite3_bind_int(checkStmt, 2, user_id);

    int count = 0;
    if (sqlite3_step(checkStmt) == SQLITE_ROW) {
        count = sqlite3_column_int(checkStmt, 0);
    }
    sqlite3_finalize(checkStmt);

    if (count == 0) {
        cout << "Playlist not found or you don't have permission to delete it.\n";
        return;
    }

    
    sqlite3_stmt* deleteSongsStmt;
    const char* deleteSongsSQL = "DELETE FROM PlaylistSongs WHERE playlist_id = ?;";
    if (sqlite3_prepare_v2(db, deleteSongsSQL, -1, &deleteSongsStmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(deleteSongsStmt, 1, playlistId);
        sqlite3_step(deleteSongsStmt);
        sqlite3_finalize(deleteSongsStmt);
    }

    
    sqlite3_stmt* deleteStmt;
    const char* deleteSQL = "DELETE FROM playlist WHERE id = ?;";
    if (sqlite3_prepare_v2(db, deleteSQL, -1, &deleteStmt, nullptr) != SQLITE_OK) {
        cout << "Failed to prepare DELETE statement: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_int(deleteStmt, 1, playlistId);

    if (sqlite3_step(deleteStmt) == SQLITE_DONE) {
        cout << "Playlist deleted successfully.\n";
    }
    else {
        cout << "Failed to delete playlist: " << sqlite3_errmsg(db) << endl;
    }

    sqlite3_finalize(deleteStmt);
}

void User::addSongToMyPlaylist(sqlite3* db) {
    int playlistId, songId;

    // Show user's playlists
    cout << "\n--- Your Playlists ---\n";
    sqlite3_stmt* stmt;
    const char* showSQL = "SELECT id, name FROM playlist WHERE creator_type = 'user' AND creator_id = ?;";
    if (sqlite3_prepare_v2(db, showSQL, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, user_id);

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            const unsigned char* name = sqlite3_column_text(stmt, 1);
            cout << "ID: " << id << " | Name: " << name << endl;
        }
        sqlite3_finalize(stmt);
    }
    else {
        cout << "Failed to fetch playlists: " << sqlite3_errmsg(db) << endl;
        return;
    }

    // Get playlist ID
    cout << "\nEnter the ID of the playlist you want to add a song to: ";
    cin >> playlistId;

    // Show all songs
    cout << "\n--- All Songs ---\n";
    const char* showSongsSQL = "SELECT id, title, artist FROM song;";
    if (sqlite3_prepare_v2(db, showSongsSQL, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            const unsigned char* title = sqlite3_column_text(stmt, 1);
            const unsigned char* artist = sqlite3_column_text(stmt, 2);
            cout << "ID: " << id << " | Title: " << title << " | Artist: " << artist << endl;
        }
        sqlite3_finalize(stmt);
    }
    else {
        cout << "Failed to fetch songs: " << sqlite3_errmsg(db) << endl;
        return;
    }

    cout << "\nEnter the ID of the song you want to add: ";
    cin >> songId;

    // Insert into PlaylistSongs
    const char* insertSQL = "INSERT INTO PlaylistSongs (playlist_id, song_id) VALUES (?, ?);";
    if (sqlite3_prepare_v2(db, insertSQL, -1, &stmt, nullptr) != SQLITE_OK) {
        cout << "Failed to prepare INSERT: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_int(stmt, 1, playlistId);
    sqlite3_bind_int(stmt, 2, songId);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        cout << "Song added to playlist successfully.\n";
    }
    else {
        cout << "Failed to add song to playlist: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return;
    }
    sqlite3_finalize(stmt);

    
    const char* updateSQL = "UPDATE playlist SET num_songs = num_songs + 1 WHERE id = ?;";
    if (sqlite3_prepare_v2(db, updateSQL, -1, &stmt, nullptr) != SQLITE_OK) {
        cout << "Failed to prepare UPDATE: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_int(stmt, 1, playlistId);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        cout << "Failed to update num_songs: " << sqlite3_errmsg(db) << endl;
    }

    sqlite3_finalize(stmt);
}


void User::viewSongsInMyPlaylist(sqlite3* db) {
    int playlistId;
    sqlite3_stmt* stmt;

    // Step 1: Show user's playlists
    cout << "\n--- Your Playlists ---\n";
    const char* getPlaylistsSQL = "SELECT id, name FROM playlist WHERE creator_type = 'user' AND creator_id = ?;";
    if (sqlite3_prepare_v2(db, getPlaylistsSQL, -1, &stmt, nullptr) != SQLITE_OK) {
        cout << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_int(stmt, 1, user_id);
    bool hasPlaylists = false;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        hasPlaylists = true;
        int id = sqlite3_column_int(stmt, 0);
        const unsigned char* name = sqlite3_column_text(stmt, 1);
        cout << "ID: " << id << " | Name: " << name << endl;
    }
    sqlite3_finalize(stmt);

    if (!hasPlaylists) {
        cout << "You haven't created any playlists yet.\n";
        return;
    }

    // Step 2: Select playlist
    cout << "\nEnter the ID of the playlist to view its songs: ";
    cin >> playlistId;

    // Step 3: Show songs in that playlist
    cout << "\n--- Songs in Playlist ---\n";
    const char* getSongsSQL = R"(
        SELECT song.id, song.title, song.artist, song.genre, song.release_date, song.duration
        FROM PlaylistSongs
        JOIN song ON PlaylistSongs.song_id = song.id
        WHERE PlaylistSongs.playlist_id = ?;
    )";

    if (sqlite3_prepare_v2(db, getSongsSQL, -1, &stmt, nullptr) != SQLITE_OK) {
        cout << "Failed to prepare song query: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_int(stmt, 1, playlistId);
    bool hasSongs = false;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        hasSongs = true;
        int id = sqlite3_column_int(stmt, 0);
        const unsigned char* title = sqlite3_column_text(stmt, 1);
        const unsigned char* artist = sqlite3_column_text(stmt, 2);
        const unsigned char* genre = sqlite3_column_text(stmt, 3);
        const unsigned char* release = sqlite3_column_text(stmt, 4);
        const unsigned char* duration = sqlite3_column_text(stmt, 5);

        cout << "ID: " << id << " | Title: " << title << " | Artist: " << artist
            << " | Genre: " << genre << " | Release: " << release
            << " | Duration: " << duration << endl;
    }

    if (!hasSongs) {
        cout << "This playlist has no songs.\n";
    }

    sqlite3_finalize(stmt);
}

void User::removeSongFromMyPlaylist(sqlite3* db) {
    sqlite3_stmt* stmt;
    int playlistId;

    // Step 1: Show user's playlists
    cout << "\n--- Your Playlists ---\n";
    const char* getPlaylistsSQL = "SELECT id, name FROM playlist WHERE creator_type = 'user' AND creator_id = ?;";
    if (sqlite3_prepare_v2(db, getPlaylistsSQL, -1, &stmt, nullptr) != SQLITE_OK) {
        cout << "Failed to fetch playlists: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_int(stmt, 1, user_id);
    bool hasPlaylists = false;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        hasPlaylists = true;
        int id = sqlite3_column_int(stmt, 0);
        const unsigned char* name = sqlite3_column_text(stmt, 1);
        cout << "ID: " << id << " | Name: " << name << endl;
    }
    sqlite3_finalize(stmt);

    if (!hasPlaylists) {
        cout << "You haven't created any playlists.\n";
        return;
    }

    // Step 2: Select playlist
    cout << "\nEnter the ID of the playlist: ";
    cin >> playlistId;

    // Step 3: Show songs in the selected playlist
    const char* getSongsSQL = R"(
        SELECT song.id, song.title
        FROM PlaylistSongs
        JOIN song ON PlaylistSongs.song_id = song.id
        WHERE PlaylistSongs.playlist_id = ?;
    )";

    if (sqlite3_prepare_v2(db, getSongsSQL, -1, &stmt, nullptr) != SQLITE_OK) {
        cout << "Failed to fetch songs: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_int(stmt, 1, playlistId);
    bool hasSongs = false;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        hasSongs = true;
        int songId = sqlite3_column_int(stmt, 0);
        const unsigned char* title = sqlite3_column_text(stmt, 1);
        cout << "ID: " << songId << " | Title: " << title << endl;
    }
    sqlite3_finalize(stmt);

    if (!hasSongs) {
        cout << "This playlist has no songs.\n";
        return;
    }

    // Step 4: Choose song to delete
    int songIdToRemove;
    cout << "\nEnter the ID of the song to remove: ";
    cin >> songIdToRemove;

    // Step 5: Delete from PlaylistSongs
    const char* deleteSQL = "DELETE FROM PlaylistSongs WHERE playlist_id = ? AND song_id = ?;";
    if (sqlite3_prepare_v2(db, deleteSQL, -1, &stmt, nullptr) != SQLITE_OK) {
        cout << "Failed to prepare DELETE: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_int(stmt, 1, playlistId);
    sqlite3_bind_int(stmt, 2, songIdToRemove);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        cout << "Song removed from playlist.\n";
    }
    else {
        cout << "Failed to remove song: " << sqlite3_errmsg(db) << endl;
    }
    sqlite3_finalize(stmt);

    // Step 6: Update num_songs in playlist
    const char* updateSQL = "UPDATE playlist SET num_songs = num_songs - 1 WHERE id = ? AND num_songs > 0;";
    if (sqlite3_prepare_v2(db, updateSQL, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, playlistId);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }
}


void User::viewSongsInLikedPlaylist(sqlite3* db) {
    sqlite3_stmt* stmt;

    const char* likedPlaylistsSQL =
        "SELECT Playlist.id, Playlist.name FROM LikedPlaylists "
        "JOIN Playlist ON LikedPlaylists.playlist_id = Playlist.id "
        "WHERE LikedPlaylists.user_id = ?;";

    if (sqlite3_prepare_v2(db, likedPlaylistsSQL, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare liked playlists query.\n";
        return;
    }

    sqlite3_bind_int(stmt, 1, user_id);

    cout << "\n--- Your Liked Playlists ---\n";
    bool found = false;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        found = true;
        int id = sqlite3_column_int(stmt, 0);
        string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        cout << "ID: " << id << " | Name: " << name << endl;
    }
    sqlite3_finalize(stmt);

    if (!found) {
        cout << "You have no liked playlists.\n";
        return;
    }

    int playlistId;
    cout << "\nEnter the ID of the liked playlist to view songs: ";
    cin >> playlistId;

    const char* songSQL =
        "SELECT song.id, song.title, song.artist, song.genre "
        "FROM song "
        "JOIN playlist_song ON song.id = playlist_song.song_id "
        "WHERE playlist_song.playlist_id = ?;";

    if (sqlite3_prepare_v2(db, songSQL, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare song query.\n";
        return;
    }

    sqlite3_bind_int(stmt, 1, playlistId);

    cout << "\n--- Songs in Playlist ---\n";
    found = false;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        found = true;
        int songId = sqlite3_column_int(stmt, 0);
        string title = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        string artist = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        string genre = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        cout << "ID: " << songId << " | Title: " << title << " | Artist: " << artist << " | Genre: " << genre << endl;
    }

    if (!found) {
        cout << "This playlist has no songs.\n";
    }

    sqlite3_finalize(stmt);
}

void User::viewSongsInAnyPlaylist(sqlite3* db) {
    sqlite3_stmt* stmt;

    const char* allPlaylistsSQL =
        "SELECT id, name FROM Playlist;";

    if (sqlite3_prepare_v2(db, allPlaylistsSQL, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare playlists query.\n";
        return;
    }

    cout << "\n--- All Playlists in the System ---\n";
    bool found = false;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        found = true;
        int id = sqlite3_column_int(stmt, 0);
        string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        cout << "ID: " << id << " | Name: " << name << endl;
    }
    sqlite3_finalize(stmt);

    if (!found) {
        cout << "There are no playlists in the system.\n";
        return;
    }

    int playlistId;
    cout << "\nEnter the ID of the playlist to view songs: ";
    cin >> playlistId;

    const char* songSQL =
        "SELECT song.id, song.title, song.artist, song.genre "
        "FROM song "
        "JOIN playlist_song ON song.id = playlist_song.song_id "
        "WHERE playlist_song.playlist_id = ?;";

    if (sqlite3_prepare_v2(db, songSQL, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare song query.\n";
        return;
    }

    sqlite3_bind_int(stmt, 1, playlistId);

    cout << "\n--- Songs in Playlist ---\n";
    found = false;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        found = true;
        int songId = sqlite3_column_int(stmt, 0);
        string title = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        string artist = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        string genre = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        cout << "ID: " << songId << " | Title: " << title
            << " | Artist: " << artist << " | Genre: " << genre << endl;
    }

    if (!found) {
        cout << "This playlist has no songs.\n";
    }

    sqlite3_finalize(stmt);
}



