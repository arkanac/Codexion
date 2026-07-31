#include "codexion.h"

void print_log(t_params *params, int id, char *str)
{
    long long now_time;

    now_time = get_time(params);
    pthread_mutex_lock(&params->print_mutex);
    fprintf(stdout, "%lld %d %s\n", now_time, id, str);
    pthread_mutex_unlock(&params->print_mutex);
}

long long calculate_time()
{
    struct  timeval start_time;

    gettimeofday(&start_time, NULL);
    return ((long long)start_time.tv_sec * 1000 + start_time.tv_usec / 1000);
}

long long get_time(t_params *params)
{
    return(calculate_time() - params->start_time);
}

void sleep_until_ms(t_params *params, long long ms_duration)
{
    long long start;

    start = get_time(params);
    while (get_time(params) - start < ms_duration)
        usleep(500);
}