#include <iostream>
#include <vector>
#include <utility>
#include <unordered_map>
#include "json.hpp"
#include <ctime>
#include "httplib.h"

int main(){
    srand(time(NULL));
    httplib::Server svr;
    unsigned long long user_count = 0;
    std::vector<unsigned long long> user_ids;
    std::unordered_map<unsigned long long, std::string> user_names;
    std::unordered_map<unsigned long long, unsigned long long> user_updates;
    std::vector<std::pair<unsigned long long, std::string>> msgs;
    svr.Post("/join",[&](auto& req, auto& res){
        try{
            Object obj=ParseJson(req.body);
            Object out;
            long long user_id=user_count++;
            out["id"]=Val{Type::Int, static_cast<long long>(user_id)};
            user_names[user_id]=obj["name"].get<std::string>();
            res.body=JsonToString(out);
        }
        catch(const std::exception& e){
            res.status=400;
            res.body=e.what();
        }
    });
    svr.Post("/send",[&](auto& req, auto& res){
        try{
            Object obj=ParseJson(req.body);
            long long user_id=obj["id"].get<long long>();
            std::string msg=obj["msg"].get<std::string>();
            msgs.push_back({user_id,msg});
            res.status=200;
        }
        catch(const std::exception& e){
            res.status=400;
            res.body=e.what();
        }
    });
    svr.Post("/GetUpdates",[&](auto& req, auto& res){
        try{
            Object obj=ParseJson(req.body);
            long long user_id=obj["id"].get<long long>();
            unsigned long long last_update=user_updates[user_id];
            Object out;
            Array arr;
            for (unsigned long long i=last_update;i<msgs.size();i++){
                Object msg_obj;
                msg_obj["id"]=Val{Type::Int, static_cast<long long>(msgs[i].first)};
                msg_obj["name"]=Val{Type::String, user_names[msgs[i].first]};
                msg_obj["msg"]=Val{Type::String, msgs[i].second};
                arr.push_back(Val{Type::Obj,msg_obj});
            }
            out["msgs"]=Val{Type::Array,arr};
            user_updates[user_id]=msgs.size();
            res.body=JsonToString(out);
        }
        catch(const std::exception& e){
            res.status=400;
            res.body=e.what();
        }
    });
    svr.listen("localhost", 8000);
    return 0;
}