#include "codexion.h"

void ft_usleep(t_params *params, long long duree_ms)
{
    long long start;

    start = get_time(params);
    while (get_time(params) - start < duree_ms)
        usleep(500);
}

void *routine(void *arg)
{
    t_coder *coder;

    coder = (t_coder *)arg;
    print_log(coder->params, coder->id, "is compiling");
    ft_usleep(coder->params, coder->params->time_to_compile);
    print_log(coder->params, coder->id, "is debugging");
    ft_usleep(coder->params, coder->params->time_to_debug);
    print_log(coder->params, coder->id, "is refactoring");
    ft_usleep(coder->params, coder->params->time_to_refactor);
    return (NULL);
}

int make_threads(t_params *params)
{
    int i;
    int j;

    i = 0;
    while (i < params->number_of_coders)
    {
        if (pthread_create(&params->coders[i].thread, NULL, &routine, &params->coders[i]) != 0)
            return (1);
        i++; 
    }
    j = 0;
    while (j < params->number_of_coders)
    {
        if (pthread_join(params->coders[j].thread, NULL) != 0)
            return (1);
        j++; 
    }
return (0);
}

