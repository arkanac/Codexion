/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: repichan <repichan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/04 13:16:05 by repichan          #+#    #+#             */
/*   Updated: 2026/08/04 14:47:59 by repichan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "codexion.h"

int	is_it_running(t_params *params)
{
	int	running;

	if (pthread_mutex_lock(&params->state_mutex) != 0)
		return (0);
	running = params->is_running;
	pthread_mutex_unlock(&params->state_mutex);
	return (running);
}

int compile_count_checker(t_params *params, int nb)
{
	int count;
	
	count = params->coders[nb].compile_count;
	if (count >= params->number_of_compiles_required)
		return (1);
	else return (0);
}

void *monitor(void *arg)
{
	t_params *params;
    int i;
	int j;
    long long coder_time;
	int enough_compilation;
    
	params = (t_params *)arg;
    
    while(is_it_running(params))
    {
        i = 0;
		enough_compilation = 0;
		while (i < params->number_of_coders)
        {
            if (pthread_mutex_lock(&params->coders[i].mutex) != 0)
				return (NULL);
            coder_time = params->coders[i].last_compile_start;
			enough_compilation += compile_count_checker(params, i);
			pthread_mutex_unlock(&params->coders[i].mutex);
            if ((get_time(params) - coder_time) > params->time_to_burnout)
            {
				if (pthread_mutex_lock(&params->state_mutex) != 0)
					return (NULL);
				params->is_running = 0;
				pthread_mutex_unlock(&params->state_mutex);
				j = 0;
				while (j < params->number_of_coders)
				{
					pthread_cond_broadcast(&params->dongles[j].cond);
					j++;
				}
				print_log(params, params->coders[i].id, "burned out");
				return (NULL);
            }
            i++;
        }
		j = 0;
		if (enough_compilation == params->number_of_coders)
		{
			pthread_mutex_lock(&params->state_mutex);
			params->is_running = 0;
			pthread_mutex_unlock(&params->state_mutex);
			while (j < params->number_of_coders)
			{
				pthread_mutex_lock(&params->dongles[j].mutex);
				pthread_cond_broadcast(&params->dongles[j].cond);
				pthread_mutex_unlock(&params->dongles[j].mutex);
				j++;
			}
			return (NULL);
		}
		usleep(1000);
    }
	return (NULL);
}
