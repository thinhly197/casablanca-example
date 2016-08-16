//
//  user.hpp
//  Server
//
//  Created by Thinh Hung Ly on 6/2/16.
//  Copyright © 2016 Thinh Hung Ly. All rights reserved.
//

#ifndef user_hpp
#define user_hpp

#include <stdio.h>
#include <cpprest/json.h>

#include "transaction.hpp"

using namespace utility;

#define PIN U("pin")
#define AVAILABLE U("available")
#define NAME U("name")
#define USERS U("users")
#define DATA_FILE U("data.json")
#define REPLACE_EMPTY_TRANSACTION_JSON U("\"transactions\":[]")
#define REPLACE_TRANSACTION_JSON U("\"transactions\":[{")

class User
{
public:
    User(string_t name, string_t password, double_t available);
    bool authenicate(string_t name, string_t password);
    web::json::value print_transactions();
    web::json::value withdrawal(int amount);
    web::json::value change_pin(string_t pin);
    
    web::json::value get_available_amount() const
    {
        web::json::value result = web::json::value::object();
        result[AVAILABLE] = web::json::value::number(available);
        return result;
    }
    
    web::json::value as_json() const
    {
        web::json::value result = web::json::value::object();
        result[NAME] = web::json::value::string(name);
        result[AVAILABLE] = web::json::value::number(available);
        result[PIN] = web::json::value::string(password);
        return result;
    }
    
    static User get_user(string_t name);
    static User from_json(const web::json::object &object);
    static void update_data(const web::json::object &data);
    static stringstream_t add_transaction(stringstream_t &replace_stream, string_t to_string);
    
    string_t get_password();
    double_t get_available();
private:
    string_t name;
    string_t password;
    double_t available;
};

#endif /* user_hpp */
