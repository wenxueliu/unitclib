#ifndef __COMMON_H
#define __COMMON_H

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

void sleep_usec(int usec) {
    struct timeval t;
    t.tv_sec = 0;
    t.tv_usec = usec;
    select(0, NULL, NULL, NULL, &t);
}

void sleep_sec(int sec) {
    struct timeval t;
    t.tv_sec = sec;
    t.tv_usec = 0;
    select(0, NULL, NULL, NULL, &t);
}




#endif
