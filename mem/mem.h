#ifndef MEM_H
#define MEM_H

#include <time.h>

#define ACCESS_TIMES 10000000
#define TEST_TIMES 1000

int get_page_size(void);
void cache_access_time(int arry_size, int stride);
void access_time();
void bandwidth();
void page_fault();

double get_dur_time(struct timespec start, struct timespec end);
#endif