#include <stdio.h>

#include "ae.h"

void test() {
    printf("hello test\n");
}

void testPoll() {
    printf("test epoll\n");
    int size = 10;
    aeEventLoop *ae = aeCreateEventLoop(size);
}

int main(int argc, char **argv) {
    test();
    testPoll();
}
