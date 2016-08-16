//
//  main.cpp
//  Server
//
//  Created by Thinh Hung Ly on 6/2/16.
//  Copyright © 2016 Thinh Hung Ly. All rights reserved.
//

#include "server.hpp"

std::unique_ptr<ATMDealer> g_httpDealer;

void on_initialize(const string_t& address)
{
    // Build our listener's URI from the configured address and the hard-coded path "/dealer"
    
    uri_builder uri(address);
    uri.append_path(U("dealer"));
    
    auto addr = uri.to_uri().to_string();
    g_httpDealer = std::unique_ptr<ATMDealer>(new ATMDealer(addr));
    g_httpDealer->open().wait();
    
    ucout << utility::string_t(U("Listening for requests at: ")) << addr << std::endl;
    
    return;
}

void on_shutdown()
{
    g_httpDealer->close().wait();
    return;
}


// To start the server, run the below command with admin privileges:
// <executed binary file> <port>
// If port is not specified, will listen on 34567
#ifdef _WIN32
int wmain(int argc, wchar_t *argv[])
#else
int main(int argc, char *argv[])
#endif
{
    
    utility::string_t port = U("34567");
    if(argc == 2)
    {
        port = argv[1];
    }
    
    utility::string_t address = U("http://localhost:");
    address.append(port);
    
    on_initialize(address);
    std::cout << "Press ENTER to exit." << std::endl;
    std::string line;
    std::getline(std::cin, line);
    
    on_shutdown();
    return 0;
}


