#include "codexion.h"

int init_params(int ac, char *av[], t_params *params)
{
    if (validation(ac, **av) != 0)
        return (1);
    params->number_of_coders = atoi(av[1]);
    params->time_to_burnout = atoi(av[2]);
    params->time_to_compile = atoi(av[3]);
    params->time_to_debug = atoi(av[4]);
    params->time_to_refactor = atoi(av[5]);
    params->number_of_compiles_required = atoi(av[6]);
    params->dongle_cooldown = atoi(av[7]);
    return (0);
}   