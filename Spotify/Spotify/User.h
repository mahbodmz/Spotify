#ifndef USER_H
#define USER_H

#include <string>


class User {
private:
    std::string username;
    std::string password;

public:
    User(); // Constructor
    bool login();
    bool signUp();
};

#endif

