#include <iostream>
#include "httplib.h"

int main(){
    httplib::Client cli("localhost", 8000);
    httplib::Request req;
    req.body=R"({
        "name":"Guy"
    })";
    auto res = cli.Post("/join", req.body, "application/json");
    cli.Post("/send", R"({
        "id":0,
        "msg":"Hello"
    })", "application/json");
    auto res2 = cli.Post("/GetUpdates", R"({
        "id":0
    })", "application/json");
    std::cout<<res2->body<<std::endl;
}