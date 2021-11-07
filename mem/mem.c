#include <stdlib.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <fcntl.h>
#include "mem.h"

// int arr_size[14] = {8, 10, 12, 14, 16, 18, 20, 22, 24, 512 ,1024, 8192, 32768, 524288};
int arr_size[6] = {512 ,1024, 8192, 32768, 524288, 1048576};
// int stride_size[9] = {4, 16, 64, 128, 1024, 4096, 16384, 32768, 524288};
int stride_size[9] = {4, 16, 64, 128, 1024};

void access_time() 
{
    //back-to-back-load : the time that each load takes, assuming that the instructions before and after are also cachemissing loads. 
    for (int i = 0; i < sizeof(arr_size) / sizeof(int); i++){
        for (int j = 0; j < sizeof(stride_size) / sizeof(int); j++){
            cache_access_time(arr_size[i], stride_size[j]);
        }
    }
    
}

void cache_access_time(int array_size, int stride)
{
    struct timespec t_start, t_end;
    
    int raw_size = array_size * 1024;
    int* array = (int*)malloc(raw_size);
    srand(time(NULL));
    int elem_num = raw_size / sizeof(int);
    for (int i = 0; i < elem_num; i++) {
        array[i] = (i + stride) % elem_num;
    }

    int idx;
    clock_gettime(CLOCK_MONOTONIC, &t_start);
    for (int i = 0; i < ACCESS_TIMES; i++) {
        idx = array[idx];
    }
    clock_gettime(CLOCK_MONOTONIC, &t_end);
    double access_time = get_dur_time(t_start, t_end) / ((double)(ACCESS_TIMES/1000000));

    printf("array size: %d, stride size: %d, access time: %f\n", array_size, stride, access_time);

    free(array);
}

void bandwidth() 
{
    struct timespec t_start, t_end;
    int LEN = 6000000;
    // int raw_size = LEN * 4;
    uint8_t arr[LEN];
    // a hand-unrolled loop that loads and stores aligned 8-byte words.
    double write_sum = 0.0, read_sum = 0.0; 
    for (int i = 0; i < TEST_TIMES; i++) {
        // write bandwith
        clock_gettime(CLOCK_MONOTONIC, &t_start);
        for (int i = 0; i < LEN; i += 5)
        {
            arr[i] = 7;
            arr[i + 1] = 7;
            arr[i + 2] = 7;
            arr[i + 3] = 7;
            arr[i + 4] = 7;
        }
        clock_gettime(CLOCK_MONOTONIC, &t_end);
        write_sum += get_dur_time(t_start, t_end);
        // *ensure that the source and destination locations would not map to the same lines if the any of the caches were direct-mapped.*
        
    
        int x = 0;
        // read bandwith
        clock_gettime(CLOCK_MONOTONIC, &t_start);
        for (int j = 0; j < LEN; j += 5)
        {
            x = arr[i];
            x = arr[i + 1];
            x = arr[i + 2];
            x = arr[i + 3];
            x = arr[i + 4];
        }
        clock_gettime(CLOCK_MONOTONIC, &t_end);
        read_sum += get_dur_time(t_start, t_end);
    }

    printf("write bandwidth %f bytes/ms\n",  LEN / (write_sum / (double)TEST_TIMES));
    printf("read bandwidth %f bytes/ms\n", LEN / (read_sum / (double)TEST_TIMES));
}

void page_fault()
{
    int page_size = sysconf (_SC_PAGESIZE); // 4096
    int offset = 2 * page_size;

    long FILE_SIZE = 2147483648;

    // open file 
    int file = open("largeFile", O_RDWR);
    if (file < 0) {
        printf("open file error");
        exit(1);
    }
    char* map = (char *) mmap(NULL, FILE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE, file, 0);

    struct timespec t_start, t_end;
    clock_gettime(CLOCK_MONOTONIC, &t_start);

    for (int i = 0; i < TEST_TIMES; i++) {
        char c = map[(i * offset) % FILE_SIZE];
    }
    
    clock_gettime(CLOCK_MONOTONIC, &t_end);
    double avg_time = (double)(get_dur_time(t_start, t_end)/((double)TEST_TIMES));
    munmap(map, FILE_SIZE);
    close(file);

    printf("average page fault time is %f ms\n", avg_time);
}

double get_dur_time(struct timespec start, struct timespec end)
{
    double dur = 1000.0*end.tv_sec + 1e-6*end.tv_nsec
                    - (1000.0*start.tv_sec + 1e-6*start.tv_nsec);
    
    return dur;
}

