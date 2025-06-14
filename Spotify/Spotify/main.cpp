#include <iostream>
#include "Admin.h"
#include "User.h"
#include "song.h"
#include "Artist.h"

using namespace std;






int main() {
    sqlite3* db;

    int choice;

    if (sqlite3_open("C:/coding/Projects in C++/second project/Spotify/Spotify/Spotify/spotify.db", &db) != SQLITE_OK) {
        cout << "Failed to open the database: " << sqlite3_errmsg(db) << endl;
        return 1; 
    }

    sqlite3_exec(db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);
    
    
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
             
             if (admin.login())
             {
                 admin.adminMenu(db);
             }
             
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
                if (user.login(db)) {
                    user.userMenu(db);  
                }
                
            }
            else if (userChoice == 2) {
                if (user.signUp(db)) {
                    user.userMenu(db);  
                }
            }
            break;
        }
        case(3): {
            system("cls");
            cout << "Exiting...\n";
            sqlite3_close(db);
            return 0;
        }
        default:
            system("cls");
            cout << "Invalid choice. Try again.\n";
            break;
        }
       
    }

    return 0;
}
