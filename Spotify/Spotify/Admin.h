#ifndef ADMIN_H
#define ADMIN_H

#include <string>

class Admin {
private:
    std::string username;
    std::string password;

public:
    Admin();
    bool login();
};

#endif
