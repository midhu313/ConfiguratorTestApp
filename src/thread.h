#ifndef THREAD_H
#define THREAD_H

class Thread{
    public:
        Thread();
        virtual ~Thread();

        int start();
        int join();
        int detach();
        pthread_t getId();
        
        virtual void* run() = 0;
    private:
        pthread_t m_tid;
        int m_running;
        int m_detached;
};


#endif // THREAD_H
