#define TSU_PRELOAD_FRAME
#include "tsu.h"
#include <stdio.h>

#define XFIB_THRESHOLD 10

void adder(void)
{
    df_ldframe();
    uint64_t n1 = df_frame(1); // receive n1
    uint64_t n2 = df_frame(2); // receive n2
    df_writeN(df_frame(0),n1+n2); // send n1+n2
    df_destroy();
}

//uint64_t xfib(int n);

// scalar version
uint64_t xfib(int n) { return n < 2 ? (uint64_t)n : xfib(n-1) + xfib(n-2); }

void fib(void)
{
    df_ldframe();
    int n = df_frame(1); // receive n
//    printf("n = df_frame(1) = %d\n",n);
//    fflush(stdout);
	if (n < XFIB_THRESHOLD) {
        df_writeN(df_frame(0),xfib(n)); // send n
    }
#if 0 
    if (n < 2)
        df_writeN(df_frame(0),n); // send n
#endif
    else {
        uint64_t tfib1 = df_tschedule(&fib,2);  // spawn fib1
        uint64_t tfib2 = df_tschedule(&fib,2);  // spawn fib2

        df_write(tfib1,1, n-1); // send fib1, n-1
        df_write(tfib2,1, n-2); // send fib2, n-2
        uint64_t tadd = df_tschedule(&adder,3); // spawn adder
        df_write(tadd,0, df_frame(0)); // edge: adder.out -> self.out
        df_write(tfib1,0, _TLOC(tadd,1)); // edge: fib1.out -> adder[1]
        df_write(tfib2,0, _TLOC(tadd,2)); // edge: fib2.out -> adder[2]
    }
    df_destroy();
}


int nn=0; // input, for checking purposes

// stat reporting
//uint64_t tt;
//uint64_t ts0[100],ts1[100];

extern uint64_t df_ts0[100],df_ts1[100]; // timestamp buffers
extern uint64_t df_tt;


void report(void)
{
    df_ldframe();
    printf("++report\n");
    fflush(stdout);
    int n=df_frame(0);
    //tt = df_tstamp(ts1) - tt;
    printf("df fib= %d\n",n);
    printf("*** SUCCESS ***\n");
    fflush(stdout);
    df_destroy();
    df_exit();

    //uint64_t t1 = df_tstamp(ts0);
    //uint64_t nx = xfib(nn);
    //t1 = df_tstamp(ts1) - t1;

    //printf("ref fib= %lu\n",nx);
    //df_printstats(ts0,ts1,t1);

    //printf((uint32_t)n==nx?"*** SUCCESS\n":"***FAILURE\n");
}

int main(int argc, char **argv)
{
    nn = 10;
    if (argc > 1)
        nn = atoi(argv[1]);
    printf("computing fibonacci(%d)\n",nn);
	df_tt = df_tstamp(df_ts0);
    //tt = df_tstamp(ts0);
    uint64_t trep = df_tschedule(&report,1); // spawn reporter
    uint64_t tfib = df_tschedule(&fib, 2);  // spawn fib
    df_write(tfib,0, _TLOC(trep,0)); // edge: fib.out -> report[0]
    df_write(tfib,1, nn); // send fib, nn
    printf("++main\n");
    df_destroy();
	return 0;
}

