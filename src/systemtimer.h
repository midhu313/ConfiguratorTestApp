#ifndef SYSTEMTIMER_H
#define SYSTEMTIMER_H

#include <stddef.h>
#include <sys/time.h>

#define Clock SystemTimer::Instance()

class SystemTimer{
    private:
        SystemTimer(){

        }
        SystemTimer(const SystemTimer&);
        SystemTimer& operator=(const SystemTimer&);
    public:
        static SystemTimer* Instance(){
            static SystemTimer instance;
            return &instance;
        }
        unsigned long long currentTimeInMilliseconds(){
            struct timeval tp;
            gettimeofday(&tp,NULL);
            unsigned long long ms=((unsigned long long)tp.tv_sec*1000)+(tp.tv_usec/1000);
            return ms;
        }
};



#endif //SYSTEMTIMER_H