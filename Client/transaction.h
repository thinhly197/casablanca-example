//
//  transaction.h
//  Client
//
//  Created by Thinh Hung Ly on 6/2/16.
//  Copyright © 2016 Thinh Hung Ly. All rights reserved.
//

#ifndef transaction_h
#define transaction_h

#include <ctime>
#include <stdio.h>
#include <cpprest/json.h>

using namespace utility;

class Transaction
{
public:
    string_t username;
    time_t date;
    int amount;
    
    Transaction(string_t username, int amount, time_t date);
};

#endif /* transaction_h */
