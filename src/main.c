#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "ae.h"
#include "latency.h"

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

/**
 * it can use for many key, but each with different name;
 * for each key,  the history length is limited by LATENCY_TS_LEN(default 160)
 *
 * if latency step is one second, the latter will replace the previous sample,
 * when the latter and the previous sample are in the one second.
 */
void testLatency() {

    long long meter;
    latencyMonitorInit();

    //event1
    latencyStartMonitor(meter);
    latencyAddSample();
    latencyAddSampleIfNeeded("event1", 1000);
    latencyAddSampleIfNeeded("event1", 1000);
    latencyAddSampleIfNeeded("event1", 1001);
    latencyAddSampleIfNeeded("event1", 1002);

    for (int i =0; i < 170000; i++) {
        latencyAddSampleIfNeeded("event1", 1000 + i);
    }
    latencyEndMonitor(meter);
    printf("take time %ll ms", meter);

    //event2
    latencyStartMonitor(meter);
    for (int i =0; i < 170; i++) {
        latencyAddSampleIfNeeded("event2", 1000 + i * 2);
    }
    latencyEndMonitor(meter);
    printf("take time %ll ms", meter);


}

int main(int argc, char **argv) {
    test();
    testPoll();
}
