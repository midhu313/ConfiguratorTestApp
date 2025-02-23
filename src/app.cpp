#include "includes.h"

static const char* tag="[App]";

App App::s_instance;

App::App():cfg(Config::getInstance()),sub(RedisSubManager::getInstance()){

}

App *App::getInstance(){
    return &s_instance;
}

void App::start(){
    exitThread = false;
    this->Thread::start();
}

void App::end(){
    if(!exitThread){
        exitThread = true;
        this->Thread::join();
    }
}

void *App::run(){
    uint64_t timeNow = 0,last_pub_time=0;
    cfg->initializeConfig();
    sub->start();
    timeNow = Clock->currentTimeInMilliseconds();
    last_pub_time = timeNow;
    
    while(!exitThread){
        timeNow = Clock->currentTimeInMilliseconds();
        if((timeNow-last_pub_time)>=10000){
            std::cout<<tag<<" I am running"<<std::endl; 
            last_pub_time = timeNow;
        }
        usleep(25000);
    }
    std::cout<<tag<<" Application Ended!"<<std::endl;
    sub->end();
    return nullptr;
}