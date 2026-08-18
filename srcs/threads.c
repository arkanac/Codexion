/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rem <rem@student.42lyon.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/03 09:27:50 by repichan          #+#    #+#             */
/*   Updated: 2026/08/19 00:01:09 by rem              ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	wait_for_start(t_coder *coder)
{
	pthread_mutex_lock(&coder->params->start_mutex);
	while (coder->params->is_ready == 0)
		pthread_cond_wait(&coder->params->start_cond,
			&coder->params->start_mutex);
	pthread_mutex_unlock(&coder->params->start_mutex);
}

void	*routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	wait_for_start(coder);
	if (coder->id % 2 == 0)
		usleep(1000);
	coder_action(coder);
	return (NULL);
}

int thread_failsafe(t_params *params, int nb)
{
	int i;

	i = 0;
	stop_all_coders(params);
	while (i < nb)
	{
		if (pthread_join(params->coders[i].thread, NULL) != 0)
			return (1);
		i++;
	}
	return(0);
}

int	make_threads(t_params *params)
{
	int	i;
	int	j;

	i = 0;
	while (i < params->number_of_coders)
	{
		if (pthread_create(&params->coders[i].thread,
				NULL, &routine, &params->coders[i]) != 0)
		{
			thread_failsafe(params, i);
			return (handle_error(ERR_THREAD_INIT));
		}	
		i++;
	}
	if (pthread_create(&params->monitor, NULL, monitor, params) != 0)
		return(1);
	params->start_time = calculate_time();
	pthread_mutex_lock(&params->start_mutex);
	params->is_ready = 1;
	pthread_cond_broadcast(&params->start_cond);
	pthread_mutex_unlock(&params->start_mutex);
	j = 0;
	while (j < params->number_of_coders)
	{
		if (pthread_join(params->coders[j].thread, NULL) != 0)
			return (handle_error(ERR_THREAD_JOIN));
		j++;
	}
	if (pthread_join(params->monitor, NULL) != 0)
		return (1);
	return (0);
}
