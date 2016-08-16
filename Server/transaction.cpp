//
//  transaction.cpp
//  Server
//
//  Created by Thinh Hung Ly on 6/2/16.
//  Copyright © 2016 Thinh Hung Ly. All rights reserved.
//

#include "transaction.hpp"

Transaction::Transaction(string_t username, int amount, time_t date)
{
    this->username = username;
    this->amount = amount;
    this->date = date;
}

Transaction::Transaction(string_t json_date, int amount)
{
    this->json_date = json_date;
    this->amount = amount;
}

Transactions::Transactions(std::vector<Transaction> transactions)
{
    this->all = transactions;
}

web::json::object Transaction::update_transaction_info(string_t user)
{
    ifstream_t f(DATA_FILE);
    stringstream_t s;
    web::json::value const v;
    
    string_t username;
    string_t str_date;
    int amount;
    
    web::json::object result = web::json::value("").as_object();
    
    if (f)
    {
        s << f.rdbuf();
        f.close();
        try {
            auto data = v.parse(U(s)).as_object();
            for (auto iter = data.begin(); iter != data.end(); ++iter)
            {
                auto &key = iter->first;
                if(key.compare(string_t(TRANS_JSON_TAG)) == 0)
                {
                    auto &value = iter->second.as_array();
                    for (auto user_iter = value.begin(); user_iter != value.end(); ++user_iter)
                    {
                        const auto &name = user_iter->as_object().find(TRANS_USERNAME);
                        if (name->second.as_string().compare(user) == 0)
                        {
                            username = name->second.as_string();
                            for (auto user_detail_iter = user_iter->as_object().begin();
                                 user_detail_iter != user_iter->as_object().end(); ++user_detail_iter)
                            {
                                if (user_detail_iter->first.compare(TRANS_DATE) == 0)
                                {
                                    str_date = user_detail_iter->second.as_string();
                                }
                                if (user_detail_iter->first.compare(TRANS_AMOUNT) == 0)
                                {
                                    amount = user_detail_iter->second.as_integer();
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
    
    return result;
}

web::json::value Transaction::get_all_transactions(string_t user)
{
    ifstream_t f(DATA_FILE);
    stringstream_t s;
    web::json::value const v;
    
    std::vector<Transaction> transactions;
    
    if (f)
    {
        s << f.rdbuf();
        f.close();
        try {
            auto data = v.parse(U(s)).as_object();
            for (auto iter = data.begin(); iter != data.end(); ++iter)
            {
                auto &key = iter->first;
                if(key.compare(string_t(TRANS_JSON_TAG)) == 0)
                {
                    auto &value = iter->second.as_array();
                    for (auto user_iter = value.begin(); user_iter != value.end(); ++user_iter)
                    {
                        const auto &name = user_iter->as_object().find(TRANS_USERNAME);
                        if (name->second.as_string().compare(user) == 0)
                        {
                            string_t date;
                            int amount = 0;
                            for (auto user_detail_iter = user_iter->as_object().begin();
                                 user_detail_iter != user_iter->as_object().end(); ++user_detail_iter)
                            {
                                if (user_detail_iter->first.compare(TRANS_DATE) == 0)
                                {
                                    date = user_detail_iter->second.as_string();
                                }
                                if (user_detail_iter->first.compare(TRANS_AMOUNT) == 0)
                                {
                                    amount = user_detail_iter->second.as_integer();
                                }
                            }
                            transactions.push_back(Transaction(date, amount));
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
    
    
    return Transactions(transactions).as_json();
}
