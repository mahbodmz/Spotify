#include "admin.h"
#include <iostream>
#include "sqlite3.h"
#include "artist.h"
using namespace std;

Admin::Admin() {}

bool Admin::login() {
    sqlite3* db;
    sqlite3_stmt* stmt;
    int exit = sqlite3_open("spotify.db", &db);

    if (exit) {
        cout << "Failed to open database: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    string inputUsername, inputPassword;
    cout << "Enter admin username: ";
    cin >> inputUsername;
    this->username = inputUsername;
    cout << "Enter admin password: ";
    cin >> inputPassword;

    const char* sql = "SELECT * FROM admin WHERE username = ? AND password = ?;";
    exit = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (exit != SQLITE_OK) {
        cout << "Failed to prepare statement.\n";
        sqlite3_close(db);
        return false;
    }

    sqlite3_bind_text(stmt, 1, inputUsername.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, inputPassword.c_str(), -1, SQLITE_STATIC);

    bool loginSuccess = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        cout << "Login successful. Welcome, " << inputUsername << "!\n";
        loginSuccess = true;
    }
    else {
        cout << "Login failed. Invalid credentials.\n";
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return loginSuccess;
}
void Admin::displayAllArtists(sqlite3* db) {
    const char* sql = "SELECT id, name, num_albums, num_songs FROM artist;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        cout << "\nAvailable Artists:\n";
        cout << "-----------------------------\n";
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            const unsigned char* name = sqlite3_column_text(stmt, 1);
            int numAlbums = sqlite3_column_int(stmt, 2);
            int numSongs = sqlite3_column_int(stmt, 3);

            cout << "ID: " << id << "\n"
                << "Name: " << name << "\n"
                << "Albums: " << numAlbums << ", Songs: " << numSongs << "\n"
                << "-----------------------------\n";
        }
        sqlite3_finalize(stmt);
    }
    else {
        cerr << "Failed to fetch artists.\n"<<sqlite3_errmsg(db) << endl;
    }
}
void Admin::displaySongsByArtist(sqlite3* db, int artistId) {
    const char* sql = "SELECT title, release_date, genre, duration, filepath FROM song WHERE artist_id = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, artistId);

        cout << "\nSongs by Artist ID " << artistId << ":\n";
        cout << "-----------------------------------------\n";

        bool found = false;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            found = true;
            const unsigned char* title = sqlite3_column_text(stmt, 0);
            const unsigned char* releaseDate = sqlite3_column_text(stmt, 1);
            const unsigned char* genre = sqlite3_column_text(stmt, 2);
            int duration = sqlite3_column_int(stmt, 3);
            const unsigned char* filepath = sqlite3_column_text(stmt, 4);

            cout << "Title: " << title << "\n"
                << "Release Date: " << releaseDate << "\n"
                << "Genre: " << genre << "\n"
                << "Duration: " << duration << " sec\n"
                << "Filepath: " << filepath << "\n"
                << "-----------------------------------------\n";
        }

        if (!found) {
            cout << "No songs found for this artist.\n";
        }

        sqlite3_finalize(stmt);
    }
    else {
        cerr << "Failed to fetch songs for artist.\n";
    }
}


void Admin::adminMenu(sqlite3* db) {
    int adminChoice;
    while (true) {
        cout << "\n--- Admin Menu ---\n";
        cout << "1. Add Song\n";
        cout << "2. Add Artist\n";
        cout << "3. View All Artists\n";
        cout << "4. View Songs by Artist\n";
        cout << "5. Delete Artit\n";
        cout << "6. Delete Song\n";
        cout << "7. Create Playlist\n";
        cout << "8. View PLaylists\n";
        cout << "9. Delete Playlists\n";
        cout << "10. Add song to Playlist\n";
        cout << "11. Log out\n";
        cout << "Enter choice: ";
        cin >> adminChoice;

        switch (adminChoice) {
        case 1:
            system("cls");
            addSong(db);
            break;
        case 2: {
            system("cls");
            Artist artist;
            artist.addArtist(db);
            break;
        }
        case 3:
            system("cls");
            displayAllArtists(db);  
            break;
        case 4: {
            system("cls");
            displayAllArtists(db);  
            cout << "Enter Artist ID to view their songs (0 to cancel): ";
            int artistId;
            cin >> artistId;
            if (artistId != 0)
                displaySongsByArtist(db, artistId);
            break;
        }
        case 5:
            system("cls");
            displayAllArtists(db);
            deleteArtist(db);
            break;
        case 6:
            system("cls");
            deleteSong(db);
            break;
        case 7:
            system("cls");
            createPlaylist(db, username);
            break;
        case 8:
            system("cls");
            viewMyPlaylists(db, username);
            break;
        case 9:
            viewMyPlaylists(db, username);
            deletePlaylist(db);
            break;
        case 10:
            addSongToPlaylist( db);
            break;
        case 11:
            system("cls");
            cout << "Logging out...\n";
            return;  
        default:
            cout << "Invalid choice, please try again.\n";
            break;
        }
    }
}



void Admin::addSong(sqlite3* db) {
    
    const char* showArtistsSQL = "SELECT id, name FROM artist;";
    sqlite3_stmt* stmt;
    cout << "\nAvailable Artists:\n-------------------\n";

    if (sqlite3_prepare_v2(db, showArtistsSQL, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int artistID = sqlite3_column_int(stmt, 0);
            const unsigned char* artistName = sqlite3_column_text(stmt, 1);
            cout << "ID: " << artistID << " - Name: " << artistName << endl;
        }
        sqlite3_finalize(stmt);
    }
    else {
        cerr << "Failed to fetch artists.\n";
        return;
    }

    int artistID;
    cout << "Enter the ID of the artist this song belongs to: ";
    cin >> artistID;
    cin.ignore();

    
    string title, artistName, releaseDate, genre, duration, filepath;

    cout << "Enter song title: ";
    getline(cin, title);

    cout << "Enter artist name (for display): ";
    getline(cin, artistName);

    cout << "Enter release date (YYYY-MM-DD): ";
    getline(cin, releaseDate);

    cout << "Enter genre: ";
    getline(cin, genre);

    cout << "Enter duration (e.g. 3:45): ";
    getline(cin, duration);

    cout << "Enter file path: ";
    getline(cin, filepath);

    const char* insertSQL = "INSERT INTO song (title, artist, release_date, genre, duration, filepath, artist_id) VALUES (?, ?, ?, ?, ?, ?, ?);";
    sqlite3_stmt* insertStmt;

    if (sqlite3_prepare_v2(db, insertSQL, -1, &insertStmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(insertStmt, 1, title.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(insertStmt, 2, artistName.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(insertStmt, 3, releaseDate.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(insertStmt, 4, genre.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(insertStmt, 5, duration.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(insertStmt, 6, filepath.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(insertStmt, 7, artistID);

        if (sqlite3_step(insertStmt) == SQLITE_DONE) {
            cout << "Song added successfully.\n";
            const char* updateArtistSQL = "UPDATE artist SET num_songs = num_songs + 1 WHERE id = ?;";
            sqlite3_stmt* updateStmt;

            if (sqlite3_prepare_v2(db, updateArtistSQL, -1, &updateStmt, nullptr) == SQLITE_OK) {
                sqlite3_bind_int(updateStmt, 1, artistID);
                if (sqlite3_step(updateStmt) != SQLITE_DONE) {
                    cerr << "Failed to update artist's song count.\n";
                }
                sqlite3_finalize(updateStmt);
            }
            else {
                cerr << "Failed to prepare update statement.\n";
            }
        }
        else {
            cerr << "Failed to add song.\n";
        }

        sqlite3_finalize(insertStmt);
    }
    else {
        cerr << "Failed to prepare song insert statement.\n";
    }
}
void Admin::deleteArtist(sqlite3* db) {
    int artistId;
    cout << "Enter the ID of the artist to delete: ";
    cin >> artistId;

    const char* sql = "DELETE FROM artist WHERE id = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, artistId);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            cout << "Artist deleted successfully.\n";
        }
        else {
            cerr << "Failed to delete artist.\n";
        }

        sqlite3_finalize(stmt);
    }
    else {
        cerr << "Failed to prepare statement.\n";
    }
}
void Admin::deleteSong(sqlite3* db) {
    int artistId;
    cout << "\n--- Delete Song ---\n";
    displayAllArtists(db);
    cout << "Enter Artist ID to view their songs: ";
    cin >> artistId;

    const char* sqlSelect = "SELECT id, title FROM song WHERE artist_id = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sqlSelect, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, artistId);

        cout << "\nSongs by Artist ID " << artistId << ":\n";
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int songId = sqlite3_column_int(stmt, 0);
            const unsigned char* title = sqlite3_column_text(stmt, 1);
            cout << "ID: " << songId << " | Title: " << title << "\n";
        }

        sqlite3_finalize(stmt);
    }
    else {
        cerr << "Failed to retrieve songs.\n";
        return;
    }

    int songIdToDelete;
    cout << "Enter the ID of the song to delete: ";
    cin >> songIdToDelete;

    const char* sqlDelete = "DELETE FROM song WHERE id = ?;";
    if (sqlite3_prepare_v2(db, sqlDelete, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, songIdToDelete);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            cout << "Song deleted successfully.\n";
            const char* updateArtistSQL = "UPDATE artist SET num_songs = num_songs - 1 WHERE id = ?;";
            sqlite3_stmt* updateStmt = nullptr;

            if (sqlite3_prepare_v2(db, updateArtistSQL, -1, &updateStmt, nullptr) == SQLITE_OK) {
                sqlite3_bind_int(updateStmt, 1, artistId);
                if (sqlite3_step(updateStmt) != SQLITE_DONE) {
                    std::cerr << "Failed to update artist's num_songs.\n";
                }
            }
            else {
                std::cerr << "Failed to prepare update statement for artist.\n";
            }

            sqlite3_finalize(updateStmt);

        }
        else {
            cerr << "Failed to delete song.\n";
        }

        sqlite3_finalize(stmt);
    }
    else {
        cerr << "Failed to prepare delete statement.\n";
    }
}

void Admin::createPlaylist(sqlite3* db,  string& username) {
    string name;
    char choice;
    int artistId = -1;

    cout << "Enter playlist name: ";
    cin.ignore();
    getline(cin, name);

    cout << "Does this playlist belong to an artist (album)? (y/n): ";
    cin >> choice;

    if (choice == 'y' || choice == 'Y') {
        string artistQuery = "SELECT id, name FROM artist;";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, artistQuery.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            cout << "Available Artists:\n";
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                cout << sqlite3_column_int(stmt, 0) << ". " << sqlite3_column_text(stmt, 1) << endl;
            }
            sqlite3_finalize(stmt);
        }

        cout << "Enter artist ID to assign this playlist to: ";
        cin >> artistId;
    }
    

   
    int adminId = -1;
    string findAdminId = "SELECT id FROM admin WHERE username = ?;";
    sqlite3_stmt* adminStmt;
    if (sqlite3_prepare_v2(db, findAdminId.c_str(), -1, &adminStmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(adminStmt, 1, username.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(adminStmt) == SQLITE_ROW) {
            adminId = sqlite3_column_int(adminStmt, 0);
        }
        else {
            cout << "Error: Admin not found in database.\n";
            sqlite3_finalize(adminStmt);
            return;
        }
        

        sqlite3_finalize(adminStmt);
    }

    string insertSQL = "INSERT INTO Playlist (name, num_songs, artist_id, creator_type, creator_id) "
        "VALUES (?, 0, ?, 'admin', ?);";

    sqlite3_stmt* insertStmt;
    if (sqlite3_prepare_v2(db, insertSQL.c_str(), -1, &insertStmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(insertStmt, 1, name.c_str(), -1, SQLITE_STATIC);

        if (artistId == -1)
            sqlite3_bind_null(insertStmt, 2);
        else
            sqlite3_bind_int(insertStmt, 2, artistId);

        sqlite3_bind_int(insertStmt, 3, adminId);

        if (sqlite3_step(insertStmt) == SQLITE_DONE) {
            cout << "Playlist created successfully!\n";
            if (artistId != -1) {
                const char* updateArtistSQL = "UPDATE artist SET num_albums = num_albums + 1 WHERE id = ?;";
                sqlite3_stmt* updateStmt = nullptr;

                if (sqlite3_prepare_v2(db, updateArtistSQL, -1, &updateStmt, nullptr) == SQLITE_OK) {
                    sqlite3_bind_int(updateStmt, 1, artistId);
                    if (sqlite3_step(updateStmt) != SQLITE_DONE) {
                        std::cerr << "Failed to update artist's numAlbums.\n";
                    }
                }
                else {
                    std::cerr << "Failed to prepare update statement for artist.\n";
                }

                sqlite3_finalize(updateStmt);
            }
        }
        else {
            cout << "Failed to create playlist: " << sqlite3_errmsg(db) << endl;
        }

        sqlite3_finalize(insertStmt);
    }
    else {
        cout << "Failed to prepare insert statement: " << sqlite3_errmsg(db) << endl;
    }
}

void Admin::viewMyPlaylists(sqlite3* db, const std::string& username) {
    sqlite3_stmt* stmt = nullptr;
    const char* findIdSQL = "SELECT id FROM admin WHERE username = ?;";
    int adminId = -1;

    if (sqlite3_prepare_v2(db, findIdSQL, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            adminId = sqlite3_column_int(stmt, 0);
        }
    }
    sqlite3_finalize(stmt);

    if (adminId == -1) {
        std::cout << "Admin not found.\n";
        return;
    }

    const char* query = R"(
        SELECT p.id, p.name, p.num_songs, a.name
        FROM Playlist p
        LEFT JOIN Artist a ON p.artist_id = a.id
        WHERE p.creator_type = 'admin' AND p.creator_id = ?;
    )";

    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, adminId);

        std::cout << "\n--- Your Playlists ---\n";
        bool found = false;

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            found = true;
            int id = sqlite3_column_int(stmt, 0);
            std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            int numSongs = sqlite3_column_int(stmt, 2);
            const unsigned char* artistText = sqlite3_column_text(stmt, 3);
            std::string artistName = artistText ? reinterpret_cast<const char*>(artistText) : "None";

            std::cout << "ID: " << id << ", Name: " << name
                << ", Songs: " << numSongs
                << ", Artist (if album): " << artistName << "\n";
        }

        if (!found) {
            std::cout << "No playlists found.\n";
        }
    }
    else {
        std::cerr << "Failed to query playlists: " << sqlite3_errmsg(db) << "\n";
    }

    sqlite3_finalize(stmt);
}
void Admin::deletePlaylist(sqlite3* db) {
    int playlistId;
    cout << "Enter the ID of the playlist you want to delete: ";
    cin >> playlistId;

    
    const char* getArtistSQL = "SELECT artist_id FROM Playlist WHERE id = ?;";
    sqlite3_stmt* getArtistStmt;

    int artistId = -1;
    if (sqlite3_prepare_v2(db, getArtistSQL, -1, &getArtistStmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(getArtistStmt, 1, playlistId);
        if (sqlite3_step(getArtistStmt) == SQLITE_ROW) {
            artistId = sqlite3_column_int(getArtistStmt, 0);
        }
    }
    sqlite3_finalize(getArtistStmt);

    
    const char* deletePlaylistSQL = "DELETE FROM Playlist WHERE id = ?;";
    sqlite3_stmt* deleteStmt;

    if (sqlite3_prepare_v2(db, deletePlaylistSQL, -1, &deleteStmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare delete statement: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_int(deleteStmt, 1, playlistId);

    if (sqlite3_step(deleteStmt) == SQLITE_DONE) {
        cout << "Playlist deleted successfully.\n";

        
        if (artistId != -1) {
            const char* updateArtistSQL = "UPDATE Artist SET num_albums = num_albums - 1 WHERE id = ?;";
            sqlite3_stmt* updateStmt;
            if (sqlite3_prepare_v2(db, updateArtistSQL, -1, &updateStmt, nullptr) == SQLITE_OK) {
                sqlite3_bind_int(updateStmt, 1, artistId);
                sqlite3_step(updateStmt);
                sqlite3_finalize(updateStmt);
            }
        }

    }
    else {
        cerr << "Failed to delete playlist: " << sqlite3_errmsg(db) << endl;
    }

    sqlite3_finalize(deleteStmt);
}

void Admin::addSongToPlaylist(sqlite3* db) {
    viewMyPlaylists(db, username);
    int playlistId;
    cout << "Enter the ID of the playlist you want to add a song to: ";
    cin >> playlistId;

    
    const char* getPlaylistSQL = "SELECT artist_id FROM Playlist WHERE id = ?;";
    sqlite3_stmt* playlistStmt;
    int artistId = -1;

    if (sqlite3_prepare_v2(db, getPlaylistSQL, -1, &playlistStmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(playlistStmt, 1, playlistId);

        if (sqlite3_step(playlistStmt) == SQLITE_ROW) {
            if (sqlite3_column_type(playlistStmt, 0) != SQLITE_NULL) {
                artistId = sqlite3_column_int(playlistStmt, 0);
            }
        }
        else {
            cout << "Playlist not found.\n";
            sqlite3_finalize(playlistStmt);
            return;
        }
    }
    sqlite3_finalize(playlistStmt);

    
    const char* getSongsSQL = (artistId != -1)
        ? "SELECT id, title FROM Song WHERE artist_id = ?;"
        : "SELECT id, title FROM Song;";

    sqlite3_stmt* songStmt;

    if (sqlite3_prepare_v2(db, getSongsSQL, -1, &songStmt, nullptr) == SQLITE_OK) {
        if (artistId != -1) sqlite3_bind_int(songStmt, 1, artistId);

        cout << "\n--- Available Songs ---\n";
        while (sqlite3_step(songStmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(songStmt, 0);
            const unsigned char* title = sqlite3_column_text(songStmt, 1);
            cout << "ID: " << id << " | Title: " << title << "\n";
        }
    }
    sqlite3_finalize(songStmt);

    
    int songId;
    cout << "\nEnter the ID of the song to add: ";
    cin >> songId;

    
    const char* insertSQL = "INSERT INTO playlist_song (playlist_id, song_id) VALUES (?, ?);";
    sqlite3_stmt* insertStmt;

    if (sqlite3_prepare_v2(db, insertSQL, -1, &insertStmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(insertStmt, 1, playlistId);
        sqlite3_bind_int(insertStmt, 2, songId);

        if (sqlite3_step(insertStmt) == SQLITE_DONE) {
            cout << "Song added to playlist.\n";

            
            const char* updateSQL = "UPDATE Playlist SET num_songs = num_songs + 1 WHERE id = ?;";
            sqlite3_stmt* updateStmt;
            if (sqlite3_prepare_v2(db, updateSQL, -1, &updateStmt, nullptr) == SQLITE_OK) {
                sqlite3_bind_int(updateStmt, 1, playlistId);
                sqlite3_step(updateStmt);
                sqlite3_finalize(updateStmt);
            }
        }
        else {
            cout << "Failed to add song to playlist: " << sqlite3_errmsg(db) << "\n";
        }
    }
    sqlite3_finalize(insertStmt);
}
void Admin::deleteSongFromPlaylist(sqlite3* db) {
    int playlistId;
    cout << "Enter the ID of the playlist you want to modify: ";
    cin >> playlistId;

    // Show songs in the playlist
    const char* selectSongsSQL =
        "SELECT song.id, song.title, song.artist "
        "FROM song "
        "JOIN playlist_song ON song.id = playlist_song.song_id "
        "WHERE playlist_song.playlist_id = ?;";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, selectSongsSQL, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare select songs query.\n";
        return;
    }
    sqlite3_bind_int(stmt, 1, playlistId);

    cout << "\nSongs in Playlist ID " << playlistId << ":\n";
    bool found = false;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        found = true;
        int songId = sqlite3_column_int(stmt, 0);
        string title = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        string artist = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        cout << "ID: " << songId << " | Title: " << title << " | Artist: " << artist << "\n";
    }
    sqlite3_finalize(stmt);

    if (!found) {
        cout << "This playlist has no songs.\n";
        return;
    }

    int songToDelete;
    cout << "Enter the ID of the song to delete from the playlist: ";
    cin >> songToDelete;

    // Delete the song from the playlist_song table
    const char* deleteSQL =
        "DELETE FROM playlist_song WHERE playlist_id = ? AND song_id = ?;";

    sqlite3_stmt* deleteStmt;
    if (sqlite3_prepare_v2(db, deleteSQL, -1, &deleteStmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare delete statement.\n";
        return;
    }
    sqlite3_bind_int(deleteStmt, 1, playlistId);
    sqlite3_bind_int(deleteStmt, 2, songToDelete);

    if (sqlite3_step(deleteStmt) != SQLITE_DONE) {
        cerr << "Failed to delete song from playlist.\n";
    } else {
        // Decrement num_songs in the Playlist table
        const char* updateSQL =
            "UPDATE Playlist SET num_songs = num_songs - 1 WHERE id = ?;";
        sqlite3_stmt* updateStmt;
        if (sqlite3_prepare_v2(db, updateSQL, -1, &updateStmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_int(updateStmt, 1, playlistId);
            sqlite3_step(updateStmt);
            sqlite3_finalize(updateStmt);
        }
        cout << "Song removed from playlist.\n";
    }

    sqlite3_finalize(deleteStmt);
}







