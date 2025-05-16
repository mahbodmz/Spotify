#include <iostream>
#include "Admin.h"
#include "User.h"
#include "song.h"

using namespace std;

int main() {
    sqlite3* db;
    int exit = sqlite3_open("spotify.db", &db);
    if (exit) {
        cout << "Failed to open DB: " << sqlite3_errmsg(db) << endl;
        return -1;
    }

    
    string testFilePath = "C:/Users/Mahbod/OneDrive/Desktop/New folder/victory.wav.wav";

    Song testSong("Test Title", "Test Artist", "2023-05-01", "Pop", 240, testFilePath);
    testSong.saveToDatabase(db);

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
