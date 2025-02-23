#include "includes.h"

bool m_exit;
App *m_app;

void signalHandler(int signum){
    m_exit = true;
}

int main(int, char**){
    signal(SIGINT,signalHandler);
    m_app = App::getInstance();
    m_exit = false;
    m_app->start();
    while(!m_exit){
        usleep(1000000);
    }
    if(m_app)
        m_app->end();

    return 0;
}