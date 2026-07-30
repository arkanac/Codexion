#include "codexion.h"

void print_log(t_params *params, int id, char *str)
{
    long long now_time;

    now_time = get_time(params);
    pthread_mutex_lock(&params->print_mutex);
    fprintf(stdout, "%lld %d %s\n", now_time, id, str);
    pthread_mutex_unlock(&params->print_mutex);
}