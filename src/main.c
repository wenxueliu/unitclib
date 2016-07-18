#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "ae.h"

void test() {
    printf("hello test\n");
}

int timeEvent(struct aeEventLoop *eventLoop, long long id, void *clientData) {

    struct timeval tv;
    gettimeofday(&tv, NULL);

    printf("time event id=%lld time=%lld\n", id, tv.tv_sec);
    aeDeleteTimeEvent(eventLoop, id);
    return 1;
}

void aeBefore(struct aeEventLoop *eventLoop) {
    printf("before event\n");
}

void testPoll() {
    printf("test epoll\n");
    int size = 10;
    aeEventLoop *ae = aeCreateEventLoop(size);
    if (aeCreateTimeEvent(ae, 1000, timeEvent, NULL, NULL) == AE_ERR) {
        exit(1);
    }
    if (aeCreateTimeEvent(ae, 5000, timeEvent, NULL, NULL) == AE_ERR) {
        exit(1);
    }
    if (aeCreateTimeEvent(ae, 10000, timeEvent, NULL, NULL) == AE_ERR) {
        exit(1);
    }

    for (int i = 0; i < 16; i++) {
        aeProcessEvents(ae, AE_TIME_EVENTS);
    }
    //aeMain(ae);
    //aeDeleteEventLoop(ae);
}

int main(int argc, char **argv) {
    test();
    testPoll();
}
