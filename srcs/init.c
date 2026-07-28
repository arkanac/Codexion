#include "codexion.h"

int init_dongle(int id, t_dongle *dongle)
{
    dongle->id = id;
    dongle->owner = -1;
    dongle->available_at = 0;
    if (pthread_mutex_init(&dongle->mutex, NULL) != 0)
        return(1);
    if (pthread_cond_init(&dongle->cond, NULL) != 0)
	{
		pthread_mutex_destroy(&dongle->mutex);
        return(1);
	}
    return (0);
}

t_dongle *create_dongles(int nb)
{
    int i;
    int j;
    t_dongle *dongles;

    i = 0;
    dongles = malloc(sizeof(t_dongle) * (nb));
    if (!dongles)
        return (NULL);
    while (i < nb)
    {
        if (init_dongle(i, &dongles[i]) != 0)
            clean_dongles(dongles, i);
            return(NULL);
        i++;
    }
   return (dongles);
}

int init_coder(number)
{

}