 #include "codexion.h"

 void clean_dongles(t_dongle *dongles, int count)
 {
    int i;
    
    i = 0;
    while (i < count)
    {
        pthread_cond_destroy(&dongles[i].cond);
        pthread_mutex_destroy(&dongles[i].mutex);
        i++;
    }
    free(dongles);
}
