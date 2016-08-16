//
//  client.cpp
//  Server
//
//  Created by Thinh Hung Ly on 6/3/16.
//  Copyright © 2016 Thinh Hung Ly. All rights reserved.
//

#include "client.hpp"

//// bool want_login:
////    true: updating the user is logging
////    false: updating the user logged out
void Client::update_login_user(string_t name, bool want_login)
{
    ifstream_t f(LOGIN_USER_FILE);
    stringstream_t ss;
    if (f)
    {
        ss << f.rdbuf();
        f.close();
    }
    ofstream_t ofs (LOGIN_USER_FILE, ofstream_t::out);
    if (want_login) {
        ofs << name << std::endl;
        ofs << ss.str() << std::endl;
    }
    else
    {
        string_t temp = string_t(ss.str());
        size_t start_pos = 0;
        while((start_pos = temp.find(name, start_pos)) != std::string::npos)
        {
            temp.replace(start_pos, name.length(), "");
            start_pos++;
        }
        ofs << temp << std::endl;
    }
    ofs.close();
}

bool Client::check_login_user(string_t name)
{
    ifstream_t f(LOGIN_USER_FILE);
    stringstream_t ss;
    if (f)
    {
        ss << f.rdbuf();
        f.close();
        string_t temp = string_t(ss.str());
        return (temp.find(name, 0) != std::string::npos) ? true : false;
    }
    else
    {
        Client::update_login_user(name, true);
        return true;
    }
}

Client::Client(string_t name, string_t password)
{
    this->name = name;
    this->password = password;
}

Client Client::get_client(string_t clientname)
{
    ifstream_t f(DATA_FILE);
    stringstream_t s;
    web::json::value const v;
    
    string_t username;
    string_t password;
    
    if (f)
    {
        s << f.rdbuf();
        f.close();
        try {
            auto data = v.parse(U(s)).as_object();
            for (auto iter = data.begin(); iter != data.end(); ++iter)
            {
                auto &key = iter->first;
                if(key.compare(string_t(CLIENTS_JSON_TAG)) == 0)
                {
                    auto &value = iter->second.as_array();
                    for (auto user_iter = value.begin(); user_iter != value.end(); ++user_iter)
                    {
                        const auto &name = user_iter->as_object().find(NAME);
                        if (name->second.as_string().compare(clientname) == 0)
                        {
                            username = name->second.as_string();
                            for (auto user_detail_iter = user_iter->as_object().begin();
                                 user_detail_iter != user_iter->as_object().end(); ++user_detail_iter)
                            {
                                if (user_detail_iter->first.compare(CLIENT_PASSWORD) == 0)
                                {
                                    password = user_detail_iter->second.as_string();
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
    return *new Client(username, password);
}