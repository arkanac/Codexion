/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rem <rem@student.42lyon.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/03 18:45:19 by rem               #+#    #+#             */
/*   Updated: 2026/08/04 10:01:34 by rem              ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	is_it_running(t_params *params)
{
	int	running;

	pthread_mutex_lock(&params->state_mutex);
	running = params->is_running;
	pthread_mutex_unlock(&params->state_mutex);
	return (running);
}

void *monitor(void *arg)
{
	t_params *params;
    int i;
    long long coder_time;
    
	params = (t_params *)arg;
    
    while(is_it_running(params))
    {
        i = 0;
		while (i < params->number_of_coders)
        {
            pthread_mutex_lock(&params->coders[i].mutex);
            coder_time = params->coders[i].last_compile_start;
			pthread_mutex_unlock(&params->coders[i].mutex);
            if ((get_time(params) - coder_time) > params->time_to_burnout)
            {
				pthread_mutex_lock(&params->state_mutex);
				params->is_running = 0;
				pthread_mutex_unlock(&params->state_mutex);
				print_log(params, params->coders[i].id, "burned out");
				return (NULL);
            }
            i++;
        }
	usleep(1000);
    }
	return (NULL);
}