//
//  user.cpp
//  Server
//
//  Created by Thinh Hung Ly on 6/2/16.
//  Copyright © 2016 Thinh Hung Ly. All rights reserved.
//

#include "user.hpp"

User::User(string_t name, string_t password, double_t available)
{
    this->name = name;
    this->password = password;
    this->available = available;
}

stringstream_t User::add_transaction(stringstream_t &replace_stream, string_t to_string)
{
    string_t temp = string_t(replace_stream.str());
    size_t start_pos = 0;
    string_t from_string;

    if(temp.find(REPLACE_EMPTY_TRANSACTION_JSON, start_pos) != std::string::npos)
    {
        from_string = REPLACE_EMPTY_TRANSACTION_JSON;
        to_string.append("");
    }
    if(temp.find(REPLACE_TRANSACTION_JSON, start_pos) != std::string::npos)
    {
        from_string = REPLACE_TRANSACTION_JSON;
        to_string.append(",");
    }

    start_pos = 0;
    while((start_pos = temp.find(from_string, start_pos)) != std::string::npos) {
        temp.replace(start_pos, from_string.length() - 1, to_string);
        start_pos += to_string.length();
    }

    return stringstream_t(temp);
}

web::json::value User::withdrawal(int amount)
{
    this->available -= amount;

    // Add new transaction
    time_t current_time;
    time(&current_time);
    auto trans = Transaction(this->name, amount, current_time);
    string_t added_transaction = trans.as_json_all_data().serialize();

    // Update json data and file
    ifstream_t f(DATA_FILE);
    stringstream_t s;
    web::json::value const v;

    if (f)
    {
        s << f.rdbuf();
        f.close();
        try {
            auto data = v.parse(U(add_transaction(s, string_t("\"transactions\":[").append(added_transaction)).str())).as_object();
            for (auto iter = data.begin(); iter != data.end(); ++iter)
            {
                auto &key = iter->first;
                if(key.compare(string_t(USERS)) == 0)
                {
                    auto &value = iter->second.as_array();
                    for (auto user_iter = value.begin(); user_iter != value.end(); ++user_iter)
                    {
                        const auto &name = user_iter->as_object().find(NAME);
                        if (name->second.as_string().compare(this->name) == 0)
                        {
                            for (auto user_detail_iter = user_iter->as_object().begin();
                                 user_detail_iter != user_iter->as_object().end(); ++user_detail_iter)
                            {
                                if (user_detail_iter->first.compare(AVAILABLE) == 0)
                                {
                                    user_detail_iter->second = web::json::value(this->available);
                                }
                            }
                        }
                    }
                }
            }
            if (this->available >= 0)
            {
                User::update_data(data);
            }
        }
        catch(...)
        {
            ucout << "ERROR: Cannot get users from data.json" << std::endl;
        }
    }
    return User::as_json();
}

web::json::value User::change_pin(string_t pin)
{
    this->password = pin;

    // Update json data and file
    ifstream_t f(DATA_FILE);
    stringstream_t s;
    web::json::value const v;

    if (f)
    {
        s << f.rdbuf();
        f.close();
        try {
            auto data = v.parse(U(s)).as_object();
            for (auto iter = data.begin(); iter != data.end(); ++iter)
            {
                auto &key = iter->first;
                if(key.compare(string_t(USERS)) == 0)
                {
                    auto &value = iter->second.as_array();
                    for (auto user_iter = value.begin(); user_iter != value.end(); ++user_iter)
                    {
                        const auto &name = user_iter->as_object().find(NAME);
                        if (name->second.as_string().compare(this->name) == 0)
                        {
                            for (auto user_detail_iter = user_iter->as_object().begin();
                                 user_detail_iter != user_iter->as_object().end(); ++user_detail_iter)
                            {
                                if (user_detail_iter->first.compare(PIN) == 0)
                                {
                                    user_detail_iter->second = web::json::value(this->password);
                                }
                            }
                        }
                    }
                }
            }
            User::update_data(data);
        }
        catch(...)
        {
            ucout << "ERROR: Cannot get users from data.json" << std::endl;
        }
    }
    return User::as_json();
}

string_t User::get_password()
{
    return this->password;
}

double_t User::get_available()
{
    return this->available;
}

User User::get_user(string_t user_name)
{
    ifstream_t f(DATA_FILE);
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
                if(key.compare(string_t(USERS)) == 0)
                {
                    auto &value = iter->second.as_array();
                    for (auto user_iter = value.begin(); user_iter != value.end(); ++user_iter)
                    {
                        const auto &name = user_iter->as_object().find(NAME);
                        if (name->second.as_string().compare(user_name) == 0)
                        {
                            username = name->second.as_string();
                            for (auto user_detail_iter = user_iter->as_object().begin();
                                 user_detail_iter != user_iter->as_object().end(); ++user_detail_iter)
                            {
                                if (user_detail_iter->first.compare(PIN) == 0)
                                {
                                    password = user_detail_iter->second.as_string();
                                }
                                if (user_detail_iter->first.compare(AVAILABLE) == 0)
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

void User::update_data(const web::json::object &data)
{
    size_t element_size = data.size();
    int loop = 0;
    ofstream_t ofs (DATA_FILE, ofstream_t::out);
    ofs << "{";
    for (auto iter = data.begin(); iter != data.end(); ++iter)
    {
        auto &key = iter->first;
        auto &value = iter->second;

        ofs << "\"";
        ofs << key;
        ofs << "\":";
        ofs << value.serialize();

        if (++loop < element_size)
        {
            ofs << ",";
        }
    }
    ofs << "}";
    ofs.close();
}
