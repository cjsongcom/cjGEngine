
#ifdef __APPLE__

#include "cjPlatform_Apple.h"


bool IsDebuggerPresent()
{
    int mib[4];
    struct kinfo_proc info;
    size_t size;
    
    info.kp_proc.p_flag = 0;
    mib[0] = CTL_KERN;
    mib[1] = KERN_PROC;
    mib[2] = KERN_PROC_PID;
    mib[3] = getpid();
    
    size = sizeof(info);
    sysctl(mib, sizeof(mib) / sizeof(*mib), &info, &size, NULL, 0);
    
    return ((info.kp_proc.p_flag & P_TRACED) != 0);
}

void OutputDebugString(const char* fmt, ...)
{
    if(!IsDebuggerPresent())
        return;
    
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}


//
// functions that is not exist apple but linux
//

//
//
///*
// Ratings of alternatives to clock_gettime() to use with pthread timed waits:
// Solution 1 "gettimeofday":
// Complexity      : simple
// Portability     : POSIX 1
// timespec        : easy to convert from timeval to timespec
// granularity     : 1000 ns,
// call            : 120 ns,
// Rating          : the best.
// 
// Solution 2 "host_get_clock_service, clock_get_time":
// Complexity      : simple (error handling?)
// Portability     : Mac specific (is it always available?)
// timespec        : yes (struct timespec return)
// granularity     : 1000 ns (don't be fooled by timespec format)
// call time       : 6000 ns
// Rating          : the worst.
// 
// Solution 3 "mach_absolute_time + gettimeofday once":
// Complexity      : simple..average (requires initialisation)
// Portability     : Mac specific. Always available
// timespec        : system clock can be converted to timespec without cjFLOAT-math
// granularity     : 1 ns.
// call time       : 90 ns unoptimised.
// Rating          : not bad, but do we really need nanoseconds timeout?
// 
// References:
// - OS X is UNIX System 3 [U03] certified
// http://www.opengroup.org/homepage-items/c987.html
// 
// - UNIX System 3 <--> POSIX 1 <--> IEEE Std 1003.1-1988
// http://en.wikipedia.org/wiki/POSIX
// http://www.unix.org/version3/
// 
// - gettimeofday() is mandatory on U03,
// clock_..() functions are optional on U03,
// clock_..() are part of POSIX Realtime extensions
// http://www.unix.org/version3/inttables.pdf
// 
// - clock_gettime() is not available on MacMini OS X
// (Xcode > Preferences > Downloads > Command Line Tools = Installed)
// 
// - OS X recommends to use gettimeofday to calculate values for timespec
// https://developer.apple.com/library/mac/documentation/Darwin/Reference/ManPages/man3/pthread_cond_timedwait.3.html
// 
// - timeval holds microseconds, timespec - nanoseconds
// http://www.gnu.org/software/libc/manual/html_node/Elapsed-Time.html
// 
// - microtime() is used by kernel to implement gettimeofday()
// http://ftp.tw.freebsd.org/pub/branches/7.0-stable/src/sys/kern/kern_time.c
// 
// - mach_absolute_time() is really fast
// http://www.opensource.apple.com/source/Libc/Libc-320.1.3/i386/mach/mach_absolute_time.c
// 
// - Only 9 deciaml digits have meaning when int nanoseconds converted to double seconds
// Tutorial: Performance and Time post uses .12 precision for nanoseconds
// http://www.macresearch.org/tutorial_performance_and_time
// 
// Example:
// Three ways to prepare absolute time 1500 milliseconds in the future to use with pthread timed functions.
// 
// Output, N = 3, stock MacMini, OSX 10.7.5, 2.3GHz i5, 2GB 1333MHz DDR3:
// inittime.tv_sec = 1390659993
// inittime.tv_nsec = 361539000
// initclock = 76672695144136
// get_abs_future_time_0() : 1390659994.861599000
// get_abs_future_time_0() : 1390659994.861599000
// get_abs_future_time_0() : 1390659994.861599000
// get_abs_future_time_1() : 1390659994.861618000
// get_abs_future_time_1() : 1390659994.861634000
// get_abs_future_time_1() : 1390659994.861642000
// get_abs_future_time_2() : 1390659994.861643671
// get_abs_future_time_2() : 1390659994.861643877
// get_abs_future_time_2() : 1390659994.861643972
// */
//#include <stdio.h>
//#include <stdlib.h>
//#include <time.h>
//#include <sys/time.h>       /* gettimeofday */
//#include <mach/mach_time.h> /* mach_absolute_time */
//#include <mach/mach.h>      /* host_get_clock_service, mach_... */
//#include <mach/clock.h>     /* clock_get_time */
//
//#define BILLION 1000000000L
//#define MILLION 1000000L
//
//#define NORMALISE_TIMESPEC( ts, uint_milli )            \
//do {                                                \
//ts.tv_sec += uint_milli / 1000u;                \
//ts.tv_nsec += (uint_milli % 1000u) * MILLION;   \
//ts.tv_sec += ts.tv_nsec / BILLION;              \
//ts.tv_nsec = ts.tv_nsec % BILLION;              \
//} while (0)
//
//static mach_timebase_info_data_t timebase = { 0, 0 }; /* numer = 0, denom = 0 */
//static struct timespec           inittime = { 0, 0 }; /* nanoseconds since 1-Jan-1970 to init() */
//static uint64_t                  initclock;           /* ticks since boot to init() */
//
//void init()
//{
//    struct timeval  micro;      /* microseconds since 1 Jan 1970 */
//    
//    if (mach_timebase_info(&timebase) != 0)
//        abort();                            /* very unlikely error */
//    
//    if (gettimeofday(&micro, NULL) != 0)
//        abort();                            /* very unlikely error */
//    
//    initclock = mach_absolute_time();
//    
//    inittime.tv_sec = micro.tv_sec;
//    inittime.tv_nsec = micro.tv_usec * 1000;
//    printf("\tinittime.tv_sec = %ld\n", inittime.tv_sec);
//    printf("\tinittime.tv_nsec = %ld\n", inittime.tv_nsec);
//    printf("\tinitclock = %ld\n", (long)initclock);
//}
//
///*
// * Get absolute future time for pthread timed calls
// *  Solution 1: microseconds granularity
// */
//struct timespec get_abs_future_time_coarse(unsigned milli)
//{
//    struct timespec future;         /* ns since 1 Jan 1970 to 1500 ms in the future */
//    struct timeval  micro = {0, 0}; /* 1 Jan 1970 */
//    
//    (void) gettimeofday(&micro, NULL);
//    future.tv_sec = micro.tv_sec;
//    future.tv_nsec = micro.tv_usec * 1000;
//    NORMALISE_TIMESPEC( future, milli );
//    return future;
//}
//
///*
// * Solution 2: via clock service
// */
//struct timespec get_abs_future_time_served(unsigned milli)
//{
//    struct timespec     future;
//    clock_serv_t        cclock;
//    mach_timespec_t     mts;
//    
//    host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
//    clock_get_time(cclock, &mts);
//    mach_port_deallocate(mach_task_self(), cclock);
//    future.tv_sec = mts.tv_sec;
//    future.tv_nsec = mts.tv_nsec;
//    NORMALISE_TIMESPEC( future, milli );
//    return future;
//}
//
///*
// * Solution 3: nanosecond granularity
// */
//struct timespec get_abs_future_time_fine(unsigned milli)
//{
//    struct timespec future;     /* ns since 1 Jan 1970 to 1500 ms in future */
//    uint64_t        clock;      /* ticks since init */
//    uint64_t        nano;       /* nanoseconds since init */
//    
//    clock = mach_absolute_time() - initclock;
//    nano = clock * (uint64_t)timebase.numer / (uint64_t)timebase.denom;
//    future = inittime;
//    future.tv_sec += nano / BILLION;
//    future.tv_nsec += nano % BILLION;
//    NORMALISE_TIMESPEC( future, milli );
//    return future;
//}
//
//#define N 3
//
//int main()
//{
//    int                 i, j;
//    struct timespec     time[3][N];
//    struct timespec   (*get_abs_future_time[])(unsigned milli) =
//    {
//        &get_abs_future_time_coarse,
//        &get_abs_future_time_served,
//        &get_abs_future_time_fine
//    };
//    
//    init();
//    for (j = 0; j < 3; j++)
//        for (i = 0; i < N; i++)
//            time[j][i] = get_abs_future_time[j](1500);  /* now() + 1500 ms */
//    
//    for (j = 0; j < 3; j++)
//        for (i = 0; i < N; i++)
//            printf("get_abs_future_time_%d() : %10ld.%09ld\n",
//                   j, time[j][i].tv_sec, time[j][i].tv_nsec);
//    
//    return 0;
//}

int clock_gettime(clockid_t /*clk_id*/, struct timespec *t)
{
//    const int64_t kOneMillion = 1000 * 1000;
    static mach_timebase_info_data_t s_timebase;
    
    if (s_timebase.denom == 0)
        mach_timebase_info(&s_timebase);
    
    // mach_absolute_time() returns billionth of seconds,
    // so divide by one million to get milliseconds
    uint64_t time = mach_absolute_time();
    
    double nseconds = ((double)time * (double)s_timebase.numer)/((double)s_timebase.denom);
    double seconds = ((double)time * (double)s_timebase.numer)/((double)s_timebase.denom * 1e9);
    
    t->tv_sec = seconds;
    t->tv_nsec = nseconds;
    
    return 0;
}

#endif
