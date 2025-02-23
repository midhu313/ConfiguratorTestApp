#ifndef APP_H
#define APP_H

#include "includes.h"

#ifdef __cplusplus
extern  "C"{
#endif

extern bool g_exit;

class App:Thread{
    private:
        static App s_instance;

        Config *cfg;
        RedisSubManager *sub;
        
        bool exitThread;
        App();
    public:
        static App *getInstance();
        void start();
        void end();
        void* run();
};


#ifdef __cplusplus
}
#endif

#endif // APP_H
