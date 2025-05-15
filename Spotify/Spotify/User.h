#ifndef USER_H
#define USER_H

#include <string>
using namespace std;

class User {
private:
    string username;
    string password;

public:
    User(); // Constructor
    bool login();
    bool signUp();
};

#endif

