//
//  server.hpp
//  Server
//
//  Created by Thinh Hung Ly on 6/2/16.
//  Copyright © 2016 Thinh Hung Ly. All rights reserved.
//

#ifndef server_hpp
#define server_hpp

#include <stdio.h>
#include <cpprest/http_listener.h>
#include <cpprest/json.h>

#include "user.hpp"
#include "client.hpp"

using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;
using namespace utility;

class ATMDealer
{
public:
    ATMDealer() { }
    ATMDealer(string_t url);
    
    pplx::task<void> open() { return m_listener.open(); }
    pplx::task<void> close() { return m_listener.close(); }
    
    
private:
    void handle_get(http_request message);
    void handle_put(http_request message);
    void handle_post(http_request message);
    void handle_delete(http_request message);
    
    http_listener m_listener;
};

#endif /* server_hpp */
