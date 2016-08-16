//
//  User.cpp
//  Test
//
//  Created by Thinh Hung Ly on 6/1/16.
//  Copyright © 2016 Thinh Hung Ly. All rights reserved.
//

#include <stdio.h>

#include "user.hpp"

User::User(string_t name, string_t password, double_t available)
{
    this->name = name;
    this->password = password;
    this->available = available;
}

void User::withdrawal(int amount)
{
    this->available -= amount;
    //new Transaction(this->name, amount, );
}

void User::change_pin(string_t pin)
{
    this->password = pin;
}

User User::get_user(string_t user_name)
{
    ifstream_t f("data.json");
    stringstream_t s;
    web::json::value const v;
    
    string_t username;
    string_t password;
    double_t available_amount = 0.0;
    
    if (f)
    {
        s << f.rdbuf();
        f.close();
        try {
            auto data = v.parse(U(s)).as_object();
            for (auto iter = data.begin(); iter != data.end(); ++iter)
            {
                auto &key = iter->first;
                if(key.compare(string_t("users")) == 0)
                {
                    auto &value = iter->second.as_array();
                    for (auto user_iter = value.begin(); user_iter != value.end(); ++user_iter)
                    {
                        const auto &name = user_iter->as_object().find("name");
                        if (name->second.as_string().compare(user_name) == 0)
                        {
                            username = name->second.as_string();
                            for (auto user_detail_iter = user_iter->as_object().begin();
                                 user_detail_iter != user_iter->as_object().end(); ++user_detail_iter)
                            {
                                if (user_detail_iter->first.compare("pin") == 0)
                                {
                                    password = user_detail_iter->second.as_string();
                                }
                                if (user_detail_iter->first.compare("available") == 0)
                                {
                                    available_amount = user_detail_iter->second.as_double();
                                }
                            }
                            
                        }
                    }
                }
            }
        }
        catch(...)
        {
            ucout << "ERROR: Cannot get users from data.json" << std::endl;
        }
    }
    return *new User(username, password, available_amount);
}

User User::from_json(const web::json::object &object)
{
    /**
     {"available":5000,"name":"test","pin":"81dc9bdb52d04dc20036dbd8313ed055"}
     */
    string_t username;
    string_t password;
    double_t available_amount = 0.0;
    
    for (auto iter = object.begin(); iter != object.end(); ++iter)
    {
        auto &key = iter->first;
        if (key.compare(string_t("name")) == 0) {
            username = iter->second.as_string();
        }
        if (key.compare(string_t("pin")) == 0) {
            password = iter->second.as_string();
        }
        if (key.compare(string_t("available")) == 0) {
            available_amount = iter->second.as_double();
        }
    }
    
    return *new User(username, password, available_amount);
}

bool User::authenicate(string_t name, string_t password)
{
    if (name.compare(this->name) == 0 && password.compare(this->password) == 0) {
        return true;
    }
    return false;
}