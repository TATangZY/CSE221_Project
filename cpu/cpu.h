#ifndef CPU_H
#define CPU_H

#include <time.h>

#define TEST_TIMES 10000000

void get_read_time();
void get_loop_time();
void get_proc_time();
void get_sys_time();
void get_thread_time();
void get_switch_time();

double get_dur_time(struct timespec start, struct timespec end);

void* f();
void proc_0();
void proc_1(int a);
void proc_2(int a, int b);
void proc_3(int a, int b, int c);
void proc_4(int a, int b, int c, int d);
void proc_5(int a, int b, int c, int d, int e);
void proc_6(int a, int b, int c, int d, int e, int f);
void proc_7(int a, int b, int c, int d, int e, int f, int g);

#endif