#include "library.hpp"
#include "js_view_adapter.hpp"

#include <cstddef>
#include <unordered_set>
// #include <zmq.hpp>

#include <vector>

#include <iostream>

int main()
{
    mbd::lib my_lib("My Lib");
    mbd::lib my_otherlib("My Other Lib");
    
    mbd::view::js_view_adapter view(6006, {my_lib, my_otherlib});
   // view.register_param<bool>("not_bool");
    // zmq::context_t ctx;
    // zmq::socket_t sock(ctx, zmq::socket_type::stream);

    // sock.connect("tcp://127.0.0.1:3000");

    // if(sock){
    //     std::cout <<  "connected \n"  ;
    // }


    // std::string id =
    //     sock.get(zmq::sockopt::routing_id);

    // // send the id frame
    // sock.send(zmq::const_buffer(id.data(), id.size()), zmq::send_flags::sndmore);

    // sock.send(zmq::str_buffer("GET / HTTP/1.1\r\nHost: 127.0.0.1:3000\r\nConnection: keep-alive\r\n\r\n"), zmq::send_flags::none);

    // zmq::message_t msg;
    // auto rcv = sock.recv(msg);

    // std::cout << std::string(static_cast<char*>(msg.data()), msg.size()) << "\n";

    // sock.recv(msg);

    // std::cout << std::string(static_cast<char*>(msg.data()), msg.size()) << "\n";

    // sock.disconnect("tcp://127.0.0.1:3000");


/*
    zmq::context_t ctx;
    zmq::socket_t sock(ctx, zmq::socket_type::pull);

    sock.connect("tcp://127.0.0.1:3333");

    if(sock){
        std::cout <<  "connected \n"  ;
    }

std::size_t counter = 0;
    while(1){
    zmq::message_t msg;
    auto rcv = sock.recv(msg);

    std::cout << counter++ << ": " << std::string(static_cast<char*>(msg.data()), msg.size()) << "\n";

    }

    return 5;

    */
    
}