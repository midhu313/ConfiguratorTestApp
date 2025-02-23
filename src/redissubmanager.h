#ifndef REDISSUBMANAGER_H
#define REDISSUBMANAGER_H

typedef struct{
    char config_file_name[80];
    char param_name[40];
    char value[40];
}Config_Update_Params_S;

class RedisSubManager:Thread{
    private:
        static RedisSubManager s_instance;
        bool exitThread;

        Config *cfg;

        Config_Update_Params_S rx_cfg_update;

        redis_t sub_redis;
        RedisSubManager();
        ~RedisSubManager();
        static void subCallback(redisAsyncContext *c,void *r,void *privdata);
        void subCallback_Handler(redisAsyncContext *c,void *r,void *privdata);
        static void connectCallback(const redisAsyncContext *c,int status);
        void connectCallback_Handler(const redisAsyncContext *c,int status);
        static void disconnectCallback(const redisAsyncContext *c,int status);
        void disconnectCallback_Handler(const redisAsyncContext *c,int status);
    public:
        static RedisSubManager *getInstance();
        int init();
        void start();
        void end();
        void* run();

};

#endif // REDISSUBMANAGER_H
