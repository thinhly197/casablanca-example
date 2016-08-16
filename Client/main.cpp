//
//  main.cpp
//  Client
//
//  Created by Thinh Hung Ly on 6/2/16.
//  Copyright © 2016 Thinh Hung Ly. All rights reserved.
//

#include "client.h"


// To start the client, run the below command with admin privileges:
// <executed binary file> <port>
// If port is not specified, will send requests on 34567

#ifdef _WIN32
int wmain(int argc, wchar_t *argv[])
#else
int main(int argc, char *argv[])
#endif
{
    string_t port = U("34567");
    if(argc == 2)
    {
        port = argv[1];
    }
    string_t address = U("http://localhost:");
    address.append(port);
    http::uri uri = http::uri(address);
    http_client client(http::uri_builder(uri).to_uri());
    
    // Verify Client Machine in the background
    if (login_client_machine(client) == false)
    {
        wcout << L"Sorry, our service is under contruction... Please come back later." << endl;
        return status_codes::Forbidden;
    }

    // Application Menu
    string_t username;
    while (true) {
        wcout << L"Please input your ID (4 digits): ";
        cin >> username;
        if (username.length() < 5) {
            break;
        }
    }
    User user = get_user_info(client.request(methods::GET, string_t(U("/dealer/login/")).append(username)).get());

    string_t pin;
    bool auth = false;
    string_t password;
    wcout << L"Please input your PIN: ";
    int retry = 0;
    while (retry <= 3 && auth == false)
    {
        set_stdin_echo(false);
        cin >> pin;
        password = md5(pin);
        auth = user.authenicate(username, password);
        set_stdin_echo(true);
        wcout << endl;
        if (auth == false) {
            if (retry > 3) {
                wcout << L"BYE. Please contact the human." << endl;
                return status_codes::Forbidden;
            }
            wcout << L"Wrong PIN, maximum is 3 times. Please input your PIN (" << ++retry << L"): ";
        }
    }

    string_t temp = string_t(U("")).append(username).append(":").append(password);
    string_t secret_key = md5(temp);
    bool cont = true;
    while (cont == true)
    {
        int select = 0;
        string_t is_continue;

        wcout << L"=========== CROSSOVER ATM MACHINE ===========" << endl;
        wcout << L" 1. Balance check" << endl;
        wcout << L" 2. Cash withdrawal" << endl;
        wcout << L" 3. PIN change" << endl;
        wcout << L" 4. Mini statement" << endl;
        wcout << L" * Which service would you like to use [1-4]: ";
        while (true) {
            string_t input;
            cin >> input;
            stringstream myStream(input);
            if (myStream >> select)
                break;
            cout << "--> Invalid number, please try again... : ";
        }

        string_t service;
        string_t slash("/");
        switch (select) {
            case 1:
            {
                service = string_t(U("/dealer/balance/")).append(username)
                                            .append(U("?secret=")).append(secret_key);
                double_t balance = check_balance(client.request(methods::POST, service).get());
                wcout << L" --> Your balance is: " << balance << endl;
                break;
            }
            case 2:
            {
                wcout << L" * How many money do you want to widthdrawal: ";
                int amount;
                string_t input;
                while (true) {
                    cin >> input;
                    stringstream myStream(input);
                    if (myStream >> amount)
                        break;
                    cout << "--> Invalid number, please try again... : ";
                }
                service = string_t(U("/dealer/withdrawal/")).append(username)
                                    .append(slash).append(input).append(U("?secret=")).append(secret_key);
                double_t balance = check_balance(client.request(methods::POST, service).get());
                if (balance < 0)
                {
                    wcout << L"Transaction failed. The withdrawal amount is lager than your available." << endl;
                }
                else
                {
                    wcout << L"Transaction successed. Available amount: " << balance << endl;
                }
                break;
            }
            case 3:
            {
                string_t new_pin("1"), conf_pin("2");
                while (new_pin.compare(conf_pin) != 0) {
                    wcout << L"Please input your new PIN: ";
                    set_stdin_echo(false);
                    cin >> new_pin;
                    set_stdin_echo(true);
                    wcout << endl;
                    wcout << L"Please confirm your new PIN: ";
                    set_stdin_echo(false);
                    cin >> conf_pin;
                    set_stdin_echo(true);
                    wcout << endl;
                }
                string_t hash_pin = md5(new_pin);
                service = string_t(U("/dealer/change_pin/")).append(username)
                                    .append(slash).append(hash_pin).append(U("?secret=")).append(secret_key);
                string_t returned_pin = check_changing_pin(client.request(methods::POST, service).get());
                if (hash_pin.compare(returned_pin) != 0) {
                    wcout << L"Something wrong... Cannot change your new pin. Please come back later." << endl;
                }
                break;
            }
            case 4:
            {
                wcout << L"*_* YOUR HISTORY TRANSACTION *_*" << endl;
                service = string_t(U("/dealer/statement/")).append(username).append(U("?secret=")).append(secret_key);
                print_user_statement(client.request(methods::POST, service).get());
                wcout << L"================================" << endl;
                break;
            }
            default:
                wcout << L"*** Don't have any service. Sorry!" << endl;
                break;
        }
        wcout << L" * Do you want to continue? [y/n]: ";
        cin >> is_continue;
        cont = string_t("y").compare(is_continue) == 0 ? true : false;
    }

    logout_client_machine(client);
    ucout << "Thanks and Goodbye, Casablanca!\n";
    return 0;
}

void print_user_statement(const http_response &response)
{
    if (response.status_code() == status_codes::OK)
    {
        auto answer = response.extract_json().get().as_object();
        for (auto iter = answer.begin(); iter != answer.end(); ++iter)
        {
            auto &key = iter->first;
            if (key.compare(string_t("transactions")) == 0) {
                
                auto &value = iter->second.as_array();
                for (auto user_iter = value.begin(); user_iter != value.end(); ++user_iter)
                {
                    //const auto &name = user_iter->as_object().find(string_t("amount"));
                    string_t date;
                    int amount = 0;
                    for (auto user_detail_iter = user_iter->as_object().begin();
                              user_detail_iter != user_iter->as_object().end(); ++user_detail_iter)
                    {
                        if (user_detail_iter->first.compare(string_t("date")) == 0)
                        {
                            date = user_detail_iter->second.as_string();
                        }
                        if (user_detail_iter->first.compare(string_t("amount")) == 0)
                        {
                            amount = user_detail_iter->second.as_integer();
                        }
                    }
                    ucout << amount << "     ---     " << date << endl;
                }
            }
        }
    }
}

string_t check_changing_pin(const http_response &response)
{
    string_t rs("");
    if (response.status_code() == status_codes::OK)
    {
        auto answer = response.extract_json().get().as_object();
        for (auto iter = answer.begin(); iter != answer.end(); ++iter)
        {
            auto &key = iter->first;
            if (key.compare(string_t("pin")) == 0) {
                rs = iter->second.as_string();
            }
        }
    }
    return rs;
}

int check_withdrawal(const http_response &response)
{
    return (check_balance(response) < 0) ? status_codes::NotAcceptable : status_codes::OK;
}

bool login_client_machine(http_client &client)
{
    ifstream_t f(AUTH_FILE);
    stringstream_t s;
    web::json::value const v;
    if (f)
    {
        s << f.rdbuf();
        f.close();
        
        string_t name, password;
        auto data = v.parse(U(s)).as_object();
        for (auto iter = data.begin(); iter != data.end(); ++iter)
        {
            auto &key = iter->first;
            if(key.compare(string_t(CLIENT_NAME_JSON)) == 0)
            {
                name = iter->second.as_string();
            }
            if(key.compare(string_t(CLIENT_PASSWORD_JSON)) == 0)
            {
                password = iter->second.as_string();
            }
        }

        string_t service = string_t(U("/dealer/client_login/")).append(name).append("/").append(password);
        http_response response = client.request(methods::GET, service).get();
        return (response.status_code() == status_codes::OK) ? true : false;
    }
    return false;
}

void logout_client_machine(http_client &client)
{
    ifstream_t f(AUTH_FILE);
    stringstream_t s;
    web::json::value const v;
    if (f)
    {
        s << f.rdbuf();
        f.close();
        
        string_t name, password;
        auto data = v.parse(U(s)).as_object();
        for (auto iter = data.begin(); iter != data.end(); ++iter)
        {
            auto &key = iter->first;
            if(key.compare(string_t(CLIENT_NAME_JSON)) == 0)
            {
                name = iter->second.as_string();
            }
            if(key.compare(string_t(CLIENT_PASSWORD_JSON)) == 0)
            {
                password = iter->second.as_string();
            }
        }
        string_t service = string_t(U("/dealer/client_logout/")).append(name);
        client.request(methods::GET, service).get();
    }
}

User get_user_info(const http_response &response)
{
    json::value answer;
    if (response.status_code() == status_codes::OK)
    {
        answer = response.extract_json().get();
    }
    if (response.status_code() == status_codes::Conflict){}

    return User::from_json(answer.as_object());
}

double_t check_balance(const http_response &response)
{
    double_t balance = 0.0;
    if (response.status_code() == status_codes::OK)
    {
        auto answer = response.extract_json().get().as_object();
        for (auto iter = answer.begin(); iter != answer.end(); ++iter)
        {
            auto &key = iter->first;
            if (key.compare(string_t("available")) == 0) {
                balance = iter->second.as_double();
            }
        }
    }
    else
    {
        balance = -999.0;
    }
    return balance;
}

void set_stdin_echo(bool enable)
{
#ifdef WIN32
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hStdin, &mode);
    
    if( !enable )
        mode &= ~ENABLE_ECHO_INPUT;
    else
        mode |= ENABLE_ECHO_INPUT;
    
    SetConsoleMode(hStdin, mode );
    
#else
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    if( !enable )
        tty.c_lflag &= ~ECHO;
    else
        tty.c_lflag |= ECHO;
    
    (void) tcsetattr(STDIN_FILENO, TCSANOW, &tty);
#endif
}
