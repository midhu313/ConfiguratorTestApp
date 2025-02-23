#ifndef REDISDEFINES_H
#define REDISDEFINES_H

#define REDIS_IP   "127.0.0.1"
#define REDIS_PORT 6379

typedef enum{
    REDIS_CON_OFF = 0,
    REDIS_CON_INIT,
    REDIS_CON_CONNECTED,
    REDIS_CON_DISCONNECTED
}redis_conn_status_t;

typedef struct redis_t{
    redis_conn_status_t status;
    struct event_base *conn_base;
    redisAsyncContext *conn_context;
}redis_t;


#endif // REDISDEFINES_H
