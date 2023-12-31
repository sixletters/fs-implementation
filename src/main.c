#include <stdio.h>
#include "include/disk.h"
#include "include/sfs.h"
#include "include/log.h"

int main() {
    int harris = 1500;
    char data[10];
    info("HELLO");
    memcpy(data, &harris, sizeof(harris));
    info("HELLO");
    int other_harris;
    memcpy(&other_harris, data, sizeof(other_harris));
    info("VALUE OF HARRIS IS %zu", other_harris);
    return 0;
}