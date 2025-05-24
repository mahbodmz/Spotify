#ifndef USER_H
#define USER_H

#include <string>
#include"sqlite3.h"


class User {
private:
    std::string username;
    std::string password;
    

public:
    User(); 
    bool login(sqlite3* db);
    bool signUp(sqlite3* db);
    void userMenu(sqlite3* db);
    void viewOrSearchSongs(sqlite3* db);
    
    
};

#endif

