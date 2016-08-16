//
//  client.hpp
//  Server
//
//  Created by Thinh Hung Ly on 6/3/16.
//  Copyright © 2016 Thinh Hung Ly. All rights reserved.
//

#ifndef client_hpp
#define client_hpp

#include <stdio.h>
#include <cpprest/json.h>

#include "user.hpp"

#define CLIENTS_JSON_TAG U("clients")
#define CLIENT_PASSWORD U("password")

using namespace utility;

#define LOGIN_USER_FILE U("client_login.dat")

class Client
{
public:
    Client(string_t name, string_t password);
    static void update_login_user(string_t name, bool want_login);
    static bool check_login_user(string_t name);
    static Client get_client(string_t name);
    
    string_t name;
    string_t password;
};

#endif /* client_hpp */
