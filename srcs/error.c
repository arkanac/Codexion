#include "codexion.h"

int handle_error(t_error code)
{
    if (code == ERR_ARGS)
        fprintf(stderr, "Error: invalid arguments, must be a positive integer\n");
    else if (code == ERR_SCHEDULER)
        fprintf(stderr, "Error: scheduler must be fifo or edf\n");
    else if (code == ERR_MALLOC)
        fprintf(stderr, "Error: memory allocation failed\n");
    else if (code == ERR_MUTEX_INIT)
        fprintf(stderr, "Error: mutex initialization failed\n");
    else if (code == ERR_COND_INIT)
        fprintf(stderr, "Error: condition variable initialization failed\n");
    return (1);
}