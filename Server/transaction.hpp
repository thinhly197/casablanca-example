//
//  transaction.hpp
//  Server
//
//  Created by Thinh Hung Ly on 6/2/16.
//  Copyright © 2016 Thinh Hung Ly. All rights reserved.
//

#ifndef transaction_hpp
#define transaction_hpp

#include <time.h>
#include <stdio.h>
#include <vector>
#include <cpprest/json.h>

#include "user.hpp"

using namespace utility;

#define TRANS_USERNAME U("user")
#define TRANS_DATE U("date")
#define TRANS_AMOUNT U("amount")
#define TRANS_JSON_TAG U("transactions")

class Transaction
{
public:
    string_t username;
    time_t date;
    string_t json_date;
    int amount;
    
    Transaction(string_t username, int amount, time_t date);
    Transaction(string_t json_date, int amount);
    
    web::json::value as_json_all_data() const
    {
        web::json::value result = web::json::value::object();
        result[TRANS_USERNAME] = web::json::value::string(username);
        
        char s[1000];
        struct tm * p = localtime(&date);
        strftime(s, 1000, "%c", p);
        string_t datetime = string_t(s);
        result[TRANS_DATE] = web::json::value::string(datetime);
        
        result[TRANS_AMOUNT] = web::json::value::number(amount);
        return result;
    }
    
    web::json::value as_json_client() const
    {
        web::json::value result = web::json::value::object();
        result[TRANS_DATE] = web::json::value::string(json_date);
        result[TRANS_AMOUNT] = web::json::value::number(amount);
        return result;
    }

    static web::json::object update_transaction_info(string_t user);
    static web::json::value get_all_transactions(string_t user);
};

class Transactions
{
public:
    std::vector<Transaction> all;
    
    Transactions(std::vector<Transaction> transactions);
    
    web::json::value as_json() const
    {
        web::json::value result = web::json::value::object();
        web::json::value jTransactions = web::json::value::array(all.size());
        
        size_t idx = 0;
        for (auto iter = all.begin(); iter != all.end(); ++iter)
        {
            jTransactions[idx++] = iter->as_json_client();
        }
        result[TRANS_JSON_TAG] = jTransactions;
        
        return result;
    }
};

#endif /* transaction_hpp */
