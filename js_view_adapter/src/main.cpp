#include "library.hpp"
#include "js_view_adapter.hpp"

#include <cstddef>
#include <unordered_set>

#include <vector>

#include <iostream>
#include <thread>

int main()
{
    mbd::lib my_lib("My Lib");
    mbd::lib my_otherlib("My Other Lib");
    
    mbd::view::js_view_adapter view(6006, {my_lib, my_otherlib});
    while(1){
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return 0;    
}