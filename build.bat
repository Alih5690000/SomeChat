@echo off
g++ -g main.cpp -o main -lws2_32 -lhttplib
g++ -g client.cpp -o client -lws2_32 -lhttplib