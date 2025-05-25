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
        int dbUserId = sqlite3_column_int(stmt, 0);  // id is now at index 0
        const unsigned char* dbPassword = sqlite3_column_text(stmt, 1); // password is at index 1

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

    // Check if username already exists
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

    // Input password
    cout << "Enter password: ";
    cin >> password;

    // Insert new user
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
        cout << "9.View my Playlists\n";
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
        
        case 2: 
            system("cls");
            viewAllPlaylists(db);
            break;
        case 3: {
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
            saveSong(db);
            break;
        case 5: {
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
            likeSong( db);
            break;
        case 7: {
            int choice;
            viewLikedPlaylists(db);
            cout << "Do you want to delete a Song ?(1.Yes , etc.No)" << endl;
            cin >> choice;
            if (choice == 1)
            {
                removeLikedPlaylist( db);
            }
            break;
        }
        case 8:
            likePlaylist( db);
            break;
        default:
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
    

    // Ask the user to select a song (this could be from a list of available songs)
    cout << "Enter the ID of the song you want to save: ";
    cin >> song_id;
    debugCheckSongExists(db, song_id);
    debugCheckUserExists(db, getUserId());

    // Check if the song is already saved by this user
    sqlite3_stmt* stmt;
    const char* checkSQL = "SELECT COUNT(*) FROM SavedSongs WHERE user_id = ? AND song_id = ?;";

    if (sqlite3_prepare_v2(db, checkSQL, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare check statement: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_int(stmt, 1, getUserId());  // Bind user_id
    sqlite3_bind_int(stmt, 2, song_id);  // Bind song_id

    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        int count = sqlite3_column_int(stmt, 0);
        if (count > 0) {
            cout << "This song is already saved to your account." << endl;
        }
        else {
            // Insert song into SavedSongs table
            const char* insertSQL = "INSERT INTO SavedSongs (user_id, song_id) VALUES (?, ?);";
            if (sqlite3_prepare_v2(db, insertSQL, -1, &stmt, nullptr) != SQLITE_OK) {
                cerr << "Failed to prepare insert statement: " << sqlite3_errmsg(db) << endl;
                return;
            }

            sqlite3_bind_int(stmt, 1, getUserId());  // Bind user_id
            sqlite3_bind_int(stmt, 2, song_id);  // Bind song_id

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

    sqlite3_bind_int(stmt, 1, getUserId());  // Bind user_id

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

    sqlite3_bind_int(stmt, 1, user_id);  // Bind current user's ID
    sqlite3_bind_int(stmt, 2, songId);   // Bind target song ID

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

    sqlite3_bind_int(stmt, 1, user_id);  // Current user ID
    sqlite3_bind_int(stmt, 2, songId);   // Song to unlike

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







