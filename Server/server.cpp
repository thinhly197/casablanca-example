//
//  server.cpp
//  Server
//
//  Created by Thinh Hung Ly on 6/2/16.
//  Copyright © 2016 Thinh Hung Ly. All rights reserved.
//

#include "server.hpp"
#include "md5.hpp"

ATMDealer::ATMDealer(string_t url) : m_listener(url)
{
    m_listener.support(methods::GET, std::bind(&ATMDealer::handle_get, this, std::placeholders::_1));
    m_listener.support(methods::POST, std::bind(&ATMDealer::handle_post, this, std::placeholders::_1));
    //m_listener.support(methods::PUT, std::bind(&ATMDealer::handle_put, this, std::placeholders::_1));
    //m_listener.support(methods::DEL, std::bind(&ATMDealer::handle_delete, this, std::placeholders::_1));
}

void ATMDealer::handle_get(http_request message)
{
    ucout << message.to_string() << std::endl;
    
    auto paths = http::uri::split_path(http::uri::decode(message.relative_uri().path()));
    
    if (paths.empty())
    {
        message.reply(status_codes::ExpectationFailed);
        return;
    }
    
    string_t service = paths[0];
    if (service.compare(string_t("login")) == 0)
    {
        string_t username = paths[1];
        User user = User::get_user(username);
        message.reply(status_codes::OK, user.as_json());
    }
    else if (service.compare(string_t("client_login")) == 0)
    {
        string_t clientname = paths[1];
        string_t password = paths[2];
        Client client = Client::get_client(clientname);
        if (client.password.compare(password) != 0) {
            message.reply(status_codes::NonAuthInfo);
        }
        else if (Client::check_login_user(clientname)) {
            message.reply(status_codes::Conflict);
        }
        else{
            Client::update_login_user(clientname, true);
            message.reply(status_codes::OK);
        }
    }
    else if (service.compare(string_t("client_logout")) == 0)
    {
        string_t clientname = paths[1];
        Client::update_login_user(clientname, false);
        message.reply(status_codes::OK);
    }
    else
    {
        message.reply(status_codes::Forbidden);
    }
}

void ATMDealer::handle_post(http_request message)
{
    ucout <<  message.relative_uri().to_string() << std::endl;
    
    auto paths = http::uri::split_path(http::uri::decode(message.relative_uri().path()));
    if (paths.empty())
    {
        message.reply(status_codes::ExpectationFailed);
        return;
    }
    
    string_t service = paths[0];
    string_t username = paths[1];
    User user = User::get_user(username);
    
    string_t temp = string_t(U("")).append(username).append(":").append(user.get_password());
    string_t secret_key = md5(temp);
    auto query = http::uri::split_query(http::uri::decode(message.relative_uri().query()));
    for (auto iter = query.begin(); iter!= query.end(); ++iter)
    {
        if (iter->first.compare(string_t("secret")) == 0)
        {
            string_t value = iter->second;
            if (secret_key.compare(value) != 0)
            {
                message.reply(status_codes::NonAuthInfo);
                return;
            }
        }
    }
    
    if (service.compare(string_t("change_pin")) == 0)
    {
        string_t hash_pin = paths[2];
        message.reply(status_codes::OK, user.change_pin(hash_pin));
    }
    else if (service.compare(string_t("balance")) == 0)
    {
        message.reply(status_codes::OK, user.get_available_amount());
    }
    else if (service.compare(string_t("statement")) == 0)
    {
        message.reply(status_codes::OK, Transaction::get_all_transactions(username));
    }
    else if (service.compare(string_t("withdrawal")) == 0)
    {
        int amount = atoi(paths[2].c_str());
        message.reply(status_codes::OK, user.withdrawal(amount));
    }
    else
    {
        message.reply(status_codes::Forbidden);
    }
}
