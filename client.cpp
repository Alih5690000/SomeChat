#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include "httplib.h"
#include "json.hpp"

int alreadyPressed[256];

std::vector<int> GetPressedKeys(){
    std::vector<int> pressed_keys;
    for (int key=0;key<256;key++){
        if ((GetAsyncKeyState(key)&0x8000) && !alreadyPressed[key]){
            pressed_keys.push_back(key);
            alreadyPressed[key] = 1;
        }
        else if (!(GetAsyncKeyState(key)&0x8000)){
            alreadyPressed[key] = 0;
        }
    }
    return pressed_keys;
}

void reset_console_cursor() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = {0, 0};
    SetConsoleCursorPosition(hConsole, pos);
}

int main(){
    reset_console_cursor();
    httplib::Client cli("localhost", 8000);
    std::string name;
    std::cout<<"Enter your name: ";
    std::cin>>name;
    auto res1=cli.Post("/join",R"({"name":")" + name + R"("})","application/json");
    if (!res1){
        std::cout<<"Error code: "<<res1.error()<<std::endl;
        return 1;
    }
    Object came=ParseJson(res1->body);
    long long id=came["id"].get<long long>();
    std::string mes;
    std::string chat;
    while (true){
        auto ks=GetPressedKeys();
        for (int key:ks){
            if (isalpha(key)){
                mes+=(char)key;
            }
            if (key==VK_RETURN){
                auto res2=cli.Post("/send",R"({"id":)" + std::to_string(id) + R"(,"msg":")" + mes + R"("})","application/json");
                if (res2){
                    std::cout<<res2->body<<std::endl;
                }
                else{
                    std::cout<<"Error code: "<<res2.error()<<std::endl;
                }
                mes.clear();
            }
        }
        auto res3=cli.Post("/GetUpdates",R"({"id":)" + std::to_string(id) + R"(})","application/json");
        if (res3){
            Object updates=ParseJson(res3->body);
            Array arr=updates["msgs"].get<Array>();
            for (Val v:arr){
                Object msg_obj=v.get<Object>();
                std::string name=msg_obj["name"].get<std::string>();
                std::string msg=msg_obj["msg"].get<std::string>();
                chat+=name + ": " + msg + "\n";
            }
        }
        else{
            std::cout<<"Error code: "<<res3.error()<<std::endl;
        }
        reset_console_cursor();
        std::cout<<chat;
        std::cout<<"> " << mes;
        Sleep(100);
    }
}