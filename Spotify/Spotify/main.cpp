#include <iostream>
#include "Admin.h"
#include "User.h"
#include "song.h"

using namespace std;

int main() {

    sqlite3* db;
    char* errorMsg;

    int exit = sqlite3_open("spotify.db", &db);
    if (exit) {
        cerr << "Error opening DB: " << sqlite3_errmsg(db) << endl;
        return -1;
    }

    const char* createArtistTable = R"(
        CREATE TABLE IF NOT EXISTS artist (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT UNIQUE NOT NULL,
            num_albums INTEGER,
            num_songs INTEGER,
            released_songs TEXT
        );
    )";

    exit = sqlite3_exec(db, createArtistTable, NULL, 0, &errorMsg);
    if (exit != SQLITE_OK) {
        cerr << "Error creating table: " << errorMsg << endl;
        sqlite3_free(errorMsg);
    }
    else {
        cout << "Artist table created successfully." << endl;
    }

    sqlite3_close(db);
    
    
    int choice;

    while (true) {
        cout << "\n--- Spotify Menu ---\n";
        cout << "1. Login as Admin\n";
        cout << "2. Login as User \n";
        cout << "3. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice)
        {
        case(1): {
            system("cls");
             Admin admin;
             admin.login();
             break;
        }
        case(2): { 
            system("cls");
            User user;
            int userChoice;
            cout << "1. Login\n";
            cout << "2. Sign Up\n";
            cout << "Enter choice: ";
            cin >> userChoice;

            if (userChoice == 1) {
                user.login();
            }
            else if (userChoice == 2) {
                user.signUp();
            }
            break;
        }
        case(3): {
            system("cls");
            cout << "Exiting...\n";
            break;
        }
        default:
            system("cls");
            cout << "Invalid choice. Try again.\n";
            break;
        }
       
    }

    return 0;
}
