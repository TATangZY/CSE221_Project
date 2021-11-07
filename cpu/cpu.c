#include "cpu.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

void get_read_time()
{
    double sum = 0;

    struct timespec t_start, t_end;
    for (int i = 0; i < TEST_TIMES; i++) {
        clock_gettime(CLOCK_MONOTONIC, &t_start);
        clock_gettime(CLOCK_MONOTONIC, &t_end);

        double dur = get_dur_time(t_start, t_end);
        sum += dur;
    }

    printf("read time is %f\n", (double)(sum / (double)(TEST_TIMES)));
}

void get_loop_time()
{
    struct timespec t_start, t_end;
    clock_gettime(CLOCK_MONOTONIC, &t_start);
    for (int i = 0; i < TEST_TIMES * 10; i++) {}
    clock_gettime(CLOCK_MONOTONIC, &t_end);

    printf("loop overhead is %f\n", (double)(get_dur_time(t_start, t_end) / (double)(TEST_TIMES)));
}

void get_proc_time()
{
    struct timespec t_start, t_end;
    double sum = 0;

    for (int i = 0; i < TEST_TIMES; i++) {
        clock_gettime(CLOCK_MONOTONIC, &t_start);
        proc_0();
        clock_gettime(CLOCK_MONOTONIC, &t_end);
        
        sum += get_dur_time(t_start, t_end);
    }
    printf("proc_0() duration: %f ms\n", (double)(sum / (double)(TEST_TIMES)));

    for (int i = 0; i < TEST_TIMES; i++) {
        clock_gettime(CLOCK_MONOTONIC, &t_start);
        proc_1(1);
        clock_gettime(CLOCK_MONOTONIC, &t_end);
        
        sum += get_dur_time(t_start, t_end);
    }
    printf("proc_1() duration: %f ms\n", (double)(sum / (double)(TEST_TIMES)));

    sum = 0.0;
    for (int i = 0; i < TEST_TIMES; i++) {
        clock_gettime(CLOCK_MONOTONIC, &t_start);
        proc_2(1, 2);
        clock_gettime(CLOCK_MONOTONIC, &t_end);
        
        sum += get_dur_time(t_start, t_end);
    }
    printf("proc_2() duration: %f ms\n", (double)(sum / (double)(TEST_TIMES)));
    
    sum = 0.0;
    for (int i = 0; i < TEST_TIMES; i++) {
        clock_gettime(CLOCK_MONOTONIC, &t_start);
        proc_3(1, 2, 3);
        clock_gettime(CLOCK_MONOTONIC, &t_end);
        
        sum += get_dur_time(t_start, t_end);
    }
    printf("proc_3() duration: %f ms\n", (double)(sum / (double)(TEST_TIMES)));

    sum = 0.0;
    for (int i = 0; i < TEST_TIMES; i++) {
        clock_gettime(CLOCK_MONOTONIC, &t_start);
        proc_4(1, 2, 3, 4);
        clock_gettime(CLOCK_MONOTONIC, &t_end);
        
        sum += get_dur_time(t_start, t_end);
    }
    printf("proc_4() duration: %f ms\n", (double)(sum / (double)(TEST_TIMES)));

    sum = 0.0;
    for (int i = 0; i < TEST_TIMES; i++) {
        clock_gettime(CLOCK_MONOTONIC, &t_start);
        proc_5(1, 2, 3, 4, 5);
        clock_gettime(CLOCK_MONOTONIC, &t_end);
        
        sum += get_dur_time(t_start, t_end);
    }
    printf("proc_5() duration: %f ms\n", (double)(sum / (double)(TEST_TIMES)));

    sum = 0.0;
    for (int i = 0; i < TEST_TIMES; i++) {
        clock_gettime(CLOCK_MONOTONIC, &t_start);
        proc_6(1, 2, 3, 4, 5, 6);
        clock_gettime(CLOCK_MONOTONIC, &t_end);
        
        sum += get_dur_time(t_start, t_end);
    }
    printf("proc_6() duration: %f ms\n", (double)(sum / (double)(TEST_TIMES)));

    sum = 0.0;
    for (int i = 0; i < TEST_TIMES; i++) {
        clock_gettime(CLOCK_MONOTONIC, &t_start);
        proc_7(1, 2, 3, 4, 5, 6, 7);
        clock_gettime(CLOCK_MONOTONIC, &t_end);
        
        sum += get_dur_time(t_start, t_end);
    }
    printf("proc_7() duration: %f ms\n", (double)(sum / (double)(TEST_TIMES)));
}

double get_dur_time(struct timespec start, struct timespec end){
    double dur = 1000.0*end.tv_sec + 1e-6*end.tv_nsec
                    - (1000.0*start.tv_sec + 1e-6*start.tv_nsec);
    
    return dur;
}

void get_sys_time()
{
    double sum = 0;

    struct timespec t_start, t_end;
    int dev_null = open("/dev/null", O_WRONLY);
    char buf[1];
    buf[0] = '1';
    
    for (int i = 0; i < TEST_TIMES; i++) {
        clock_gettime(CLOCK_MONOTONIC, &t_start);
        write(dev_null, buf, sizeof(buf));
        clock_gettime(CLOCK_MONOTONIC, &t_end);

        double dur = get_dur_time(t_start, t_end);
        sum += dur;
    }
    close(dev_null);
    printf("system call time %f\n", (double)(sum / (double)(TEST_TIMES)));
}

void get_thread_time()
{
    struct timespec t_start, t_end;
    double sum = 0.0;
    int status;

    for (int i = 0; i < TEST_TIMES; i++) {
        // process creation
        clock_gettime(CLOCK_MONOTONIC, &t_start);
    
        if (fork() == 0) {
            exit(0);
        }
        clock_gettime(CLOCK_MONOTONIC, &t_end);

        wait(&status);
        sum += get_dur_time(t_start, t_end);    
    }
    printf("creating process overhead: %f\n", (double)(sum / (double)(TEST_TIMES)));

    sum = 0.0;
    for (int i = 0; i < TEST_TIMES; i++) {
        // thread creation
        pthread_t ptid;
        clock_gettime(CLOCK_MONOTONIC, &t_start);
        pthread_create(&ptid, NULL, f, NULL);
        clock_gettime(CLOCK_MONOTONIC, &t_end);

        pthread_join(ptid, NULL);
        sum += get_dur_time(t_start, t_end);    
    }

    printf("creating kernel thread overhead: %f\n", (double)(sum / (double)(TEST_TIMES)));
}

void get_switch_time()
{
    struct timespec t_start, t_end;
    double sum = 0.0;
    int status;
    int p[2];
    char* msg = "a";
    int pid = 0;
    
    if ((pid=fork()) == 0) { // child
        close(p[1]);

        char inbuf[2];
        for (int i = 0; i < TEST_TIMES; i++) {
            read(p[0], inbuf, sizeof(inbuf));
        }
        exit(0);
    } else if (pid > 0){ //  parent
        close(p[0]);
        for (int i = 0; i < TEST_TIMES; i++) {
            clock_gettime(CLOCK_MONOTONIC, &t_start);
            write(p[1], msg, sizeof(msg));
            clock_gettime(CLOCK_MONOTONIC, &t_end);

            sum += get_dur_time(t_start, t_end);
        }
        close(p[1]);
        printf("creating context switch overhead: %f\n", (double)(sum / (double)(TEST_TIMES)));
    }
}

void* f(){}

void proc_0(){}
void proc_1(int a){}
void proc_2(int a, int b){}
void proc_3(int a, int b, int c){}
void proc_4(int a, int b, int c, int d){}
void proc_5(int a, int b, int c, int d, int e){}
void proc_6(int a, int b, int c, int d, int e, int f){}
void proc_7(int a, int b, int c, int d, int e, int f, int g){}