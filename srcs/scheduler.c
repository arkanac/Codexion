/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rem <rem@student.42lyon.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/04 15:42:42 by repichan          #+#    #+#             */
/*   Updated: 2026/08/11 17:52:03 by rem              ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	remove_from_queue(t_coder *coder, t_dongle *dongle)
{
	if (dongle->queue[0].id == coder->id)
	{
		dongle->queue[0] = dongle->queue[1];
		memset(&dongle->queue[1], 0, sizeof(t_queue));
	}
	else if (dongle->queue[1].id == coder->id)
		memset(&dongle->queue[1], 0, sizeof(t_queue));
	return (0);
}

int	fifo_way(t_coder *coder, t_dongle *dongle)
{
	if (pthread_mutex_lock(&dongle->mutex) != 0)
		return (1);
	if ((not_in_queue(coder, dongle) != 0))
	{
		pthread_mutex_unlock(&dongle->mutex);
		return (0);
	}
	if (dongle->queue[0].id == 0)
		dongle->queue[0].id = coder->id;
	else
		if (dongle->queue[1].id == 0 && dongle->queue[1].id != coder->id)
			dongle->queue[1].id = coder->id;
	pthread_mutex_unlock(&dongle->mutex);
	return (0);
}

int	edf_way(t_params *params, t_coder *coder, t_dongle *dongle)
{
	long long deadline;

	if (pthread_mutex_lock(&coder->mutex) != 0)
		return (1);
	deadline = coder->last_compile_start + params->time_to_burnout;
	pthread_mutex_unlock(&coder->mutex);
	if (pthread_mutex_lock(&dongle->mutex) != 0)
	return (1);
	if ((not_in_queue(coder, dongle) != 0))
	{
		pthread_mutex_unlock(&dongle->mutex);
		return (0);
	}
	if (dongle->queue[0].id == 0)
	{
		dongle->queue[0].id = coder->id;
		dongle->queue[0].last_compile_start = deadline;
	}
	else if (deadline < dongle->queue[0].last_compile_start)
	{
		dongle->queue[1] = dongle->queue[0];
		dongle->queue[0].id = coder->id;
		dongle->queue[0].last_compile_start = deadline;
	}
	else
	{
		dongle->queue[1].id = coder->id;
		dongle->queue[1].last_compile_start = deadline;
	}
	pthread_mutex_unlock(&dongle->mutex);
	return(0);
}

int	scheduler(t_params *params, t_coder *coder, t_dongle *dongle)
{
	if (params->scheduler == FIFO)
		fifo_way(coder, dongle);
	else if (params->scheduler == EDF)
		edf_way(params, coder, dongle);
	return(0);
}