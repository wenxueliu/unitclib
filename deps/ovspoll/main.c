
#include <stdio.h>
//#include "poll-loop.h"
#include "dynamic-string.h"
#include "util.h"

void test_util(void) {
    ROUND_UP(100, 10);
}

void test_ds(void) {
    struct ds string;
    ds_init(&string);
    ds_put_cstr(&string, "hello ");
    ds_put_cstr(&string, "hello ");
    ds_put_format(&string, "fstat failed (%s)", " string");
    printf("dynamic-string:%s\n", ds_cstr(&string));
    ds_destroy(&string);
}

int main(int argc, char *argv[]) {
    if (argc != 0) {
        int i;
        for(i = 0; i < argc; i++) {
            printf("argv[%d]=%s\n", i, argv[i]);
        }
    }

    test_ds();
    test_util();
    //poll_block();
}
