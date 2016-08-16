//
//  user.hpp
//  Client
//
//  Created by Thinh Hung Ly on 6/2/16.
//  Copyright © 2016 Thinh Hung Ly. All rights reserved.
//

#ifndef user_hpp
#define user_hpp

#include <stdio.h>

#include <stdio.h>
#include <cpprest/json.h>

#include "md5.hpp"
#include "transaction.h"

using namespace utility;

#define PIN U("pin")
#define AVAILABLE U("available")
#define NAME U("name")

class User
{
public:
    User(string_t name, string_t password, double_t available);
    //User(string_t name, string_t password, double_t available, Transaction[5]);
    bool authenicate(string_t name, string_t password);
    void print_transactions();
    void withdrawal(int amount);
    void change_pin(string_t pin);
    double_t get_available_amount();
    
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
    
private:
    string_t name;
    string_t password;
    double_t available;
    // Because it is an mini-statement so it's limited to 5 latest transactions
    //Transaction transaction[5];
};

#endif /* user_hpp */
