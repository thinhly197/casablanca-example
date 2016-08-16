//
//  client.h
//  Client
//
//  Created by Thinh Hung Ly on 6/2/16.
//  Copyright © 2016 Thinh Hung Ly. All rights reserved.
//

#ifndef client_h
#define client_h

#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <cpprest/http_client.h>
#include <cpprest/json.h>

#include "user.hpp"

#define AUTH_FILE U("auth.json")
#define CLIENT_NAME_JSON U("clientname")
#define CLIENT_PASSWORD_JSON U("password")

using namespace std;
using namespace web;
using namespace web::json;
using namespace web::http;
using namespace web::http::client;
using namespace utility;

bool login_client_machine(http_client &client);
void logout_client_machine(http_client &client);
User get_user_info(const http_response &response);
double_t check_balance(const http_response &response);
int check_withdrawal(const http_response &response);
string_t check_changing_pin(const http_response &response);
void print_user_statement(const http_response &response);
void set_stdin_echo(bool enable = true);

#endif /* client_h */
