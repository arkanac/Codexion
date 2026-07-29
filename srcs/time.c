#include "codexion.h"

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