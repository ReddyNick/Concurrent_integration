#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <assert.h>

#define _END 2500

const double START = 0; 
const double END = _END;
const double h = 1e-6;
double SUM = 0;

pthread_mutex_t mutexsum;

double per_thread = 0;
int num_threads = 0;

double func(double x)
{
    return sqrt(x);
    
}

void* idle_routine(void* idx)
{
   
    double a = START;
    double end = a + per_thread;
    double res = 0;
    
    while (a < end) {
        res = h * func(a);
        a += h;
    }
    while(0);

    pthread_exit((void*)0);
}

void* routine(void* idx)
{
    double res = 0;
    double a = (START + (long int)idx * per_thread);
    double end = a + per_thread;

    while (a < end) {
        res += h * func(a);
        a += h;
    }
    
    pthread_mutex_lock (&mutexsum);
    SUM += res;
    pthread_mutex_unlock (&mutexsum);

    pthread_exit((void*)0);
}

int read_num_threads(int argc, char* argv[])
{
    int n_threads = 0;

    if (argc == 1) return 1;
    
    if (argc > 2){
        perror("Only one argument is required!\n");
        return -1;
    }

    char c = 0;
    int nsymbl = 0;
    nsymbl = sscanf(argv[1], "%d%c", &n_threads, &c);

    if (nsymbl != 1 || n_threads < 1){
        perror("Invalid argument!\n");
        return -1;
    }
    else 
        return n_threads;
}

int main(int argc, char* argv[])
{
    num_threads = read_num_threads(argc, argv);
    int n_cpus = get_nprocs();
    assert(n_cpus > 0);

    if (num_threads > n_cpus) {
        printf("Warning!\n"
               "The number of threads is bigger then the number of available CPUs (%d).\n", n_cpus);
               /*"Only %d threads will be created.\n", n_cpus, n_cpus);*/
        n_cpus = num_threads;
        // return 0;
    }
    
    if (num_threads < 0) exit(-1);

    pthread_t* threads = (pthread_t*)malloc(n_cpus * sizeof(pthread_t));
    if (threads == NULL) {
        printf("oops\n");
        exit(-1);
    }
    pthread_attr_t attr;
    
    void* status;
    int ret_code = 0;

    pthread_mutex_init(&mutexsum, NULL);
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
   
    per_thread = (END - START) / (double) num_threads;
    
    for (long int i = 0; i < num_threads; i++) {
        ret_code = pthread_create(&threads[i], &attr, routine, (void *)i); 
        
        if (ret_code){
            printf("ERROR; return code from pthread_create() is %d\n", ret_code);
            exit(-1);
        }
    }

    for (long int i = num_threads; i < n_cpus; i++) {

        ret_code = pthread_create(&threads[i], &attr, idle_routine, (void *)i); 
        
        if (ret_code){
            printf("ERROR; return code from pthread_create() is %d\n", ret_code);
            exit(-1);
        }
    }
    
    pthread_attr_destroy(&attr);
    
    for (long int i = 0; i < num_threads; i++) {
        ret_code = pthread_join(threads[i], &status);
        if (ret_code){
            printf("ERROR; return code from pthread_join() is %d\n", ret_code);
            exit(-1);
        }
    }
    
    pthread_mutex_destroy(&mutexsum);
    free(threads);
    
    printf("%lg\n", SUM);
    
    return 0;
}
