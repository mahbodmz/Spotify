#include <iostream>
#include "Admin.h"
#include "User.h"
#include "song.h"
#include "Artist.h"

using namespace std;

int main() {


    Artist a("Imagine Dragons", 4, 20, "Believer-Thunder-Whatever It Takes");
    if (a.saveToDatabase()) {
        cout << "Artist added successfully.\n";
    }
    else {
        cout << "Failed to add artist.\n";
    }
    
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
