#include "includes.h"

static const char* tag = "[redis-sub]"; 

RedisSubManager RedisSubManager::s_instance;

RedisSubManager::RedisSubManager():cfg(Config::getInstance()){
    sub_redis.status = REDIS_CON_DISCONNECTED;
}

RedisSubManager::~RedisSubManager(){
}

RedisSubManager *RedisSubManager::getInstance(){
    return &s_instance;
}

int RedisSubManager::init(){
    signal(SIGPIPE,SIG_IGN);
    sub_redis.conn_base = event_base_new();
    sub_redis.conn_context = redisAsyncConnect(REDIS_IP,REDIS_PORT);
    if((sub_redis.conn_context->err)||(sub_redis.conn_context == NULL)){
        std::cout<<tag<<" Error:"<<sub_redis.conn_context->errstr<<std::endl;
        sub_redis.status = REDIS_CON_DISCONNECTED;
        return 1;
    }else
        std::cout<<tag<<" Connection to "<< REDIS_IP<<"@"<<REDIS_PORT<<" is opened!"<<std::endl;
    redisLibeventAttach(sub_redis.conn_context,sub_redis.conn_base);
    redisAsyncSetConnectCallback(sub_redis.conn_context,connectCallback);
    redisAsyncSetDisconnectCallback(sub_redis.conn_context,disconnectCallback);
    redisAsyncCommand(sub_redis.conn_context,subCallback,(char *)"sub","SUBSCRIBE cfg_update_channel");
    sub_redis.status = REDIS_CON_INIT;
    return 0;
}

void RedisSubManager::start(){
    exitThread = false;
    this->Thread::start();
}

void RedisSubManager::end(){
    if(!exitThread){
        exitThread = true;
        this->Thread::join();
    }
    std::cout<<tag<<" Exited"<<std::endl;
}
void* RedisSubManager::run(){
    uint64_t now;
    int ret_status = NO_ERROR;
    struct timeval timeoutsec;
    timeoutsec.tv_sec = 1;
    timeoutsec.tv_usec = 0;
    now = Clock->currentTimeInMilliseconds();
    std::cout<<tag<<" Starting!"<<std::endl;
    while(!exitThread){
        switch(sub_redis.status){
            case REDIS_CON_INIT:
                sub_redis.status = REDIS_CON_CONNECTED;
                std::cout<<tag<<" Connected"<<std::endl;
            break;
            case REDIS_CON_CONNECTED:
                event_base_loopexit(sub_redis.conn_base,&timeoutsec);
                event_base_dispatch(sub_redis.conn_base);
            break;
            case REDIS_CON_DISCONNECTED:
                if((ret_status = init()) == NO_ERROR){
                    std::cout<<tag<<" init success"<<std::endl;
                }
            break;
            default:
                sleep(1);
                sub_redis.status = REDIS_CON_DISCONNECTED;
            break;    
        }
        sleep(1);
    }
    std::cout<<tag<<"Ended"<<std::endl;
    return nullptr;
}

void RedisSubManager::subCallback(redisAsyncContext *c,void *r,void *privdata){
    s_instance.subCallback_Handler(c,r,privdata);
}

void RedisSubManager::subCallback_Handler(redisAsyncContext *c,void *r,void *privdata){
    redisReply *reply = (redisReply*)r;
    if(reply == NULL) return;
    if((reply->type == REDIS_REPLY_ARRAY) && (reply->elements == 3)){
        if(strcmp(reply->element[0]->str,"subscribe") != 0){
            if (!strcmp(reply->element[1]->str,"cfg_update_channel")) {
                memcpy(&rx_cfg_update,reply->element[2]->str,sizeof(Config_Update_Params_S));
                if(!strcmp(rx_cfg_update.config_file_name,CONFIG_FILE_NAME)){
                    std::cout<<"Updating Configuration "<<rx_cfg_update.param_name<<"="<<rx_cfg_update.value<<std::endl;
                    cfg->updateConfiguration((const char *)rx_cfg_update.param_name,(const char *)rx_cfg_update.value);
    
                }else
                    std::cout<<"Ignoring Configuration update"<<rx_cfg_update.param_name<<"="<<rx_cfg_update.value<<std::endl;
            }
        }
    }
}

void RedisSubManager::connectCallback(const redisAsyncContext *c,int status){
    s_instance.connectCallback_Handler(c,status);
}

void RedisSubManager::connectCallback_Handler(const redisAsyncContext *c,int status){
    if(status!=REDIS_OK){
        std::cout<<tag<<" connectCallback Error:"<<c->errstr<<std::endl;
        sub_redis.status = REDIS_CON_DISCONNECTED;
    }
}

void RedisSubManager::disconnectCallback(const redisAsyncContext *c,int status){
    s_instance.disconnectCallback_Handler(c,status);
}

void RedisSubManager::disconnectCallback_Handler(const redisAsyncContext *c,int status){
    std::cout<<tag<<"Disconnected"<<std::endl;
    sub_redis.status = REDIS_CON_DISCONNECTED;
}

