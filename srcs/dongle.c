/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: repichan <repichan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/03 09:27:39 by repichan          #+#    #+#             */
/*   Updated: 2026/08/10 11:51:47 by repichan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	take_dongle(t_coder *coder, t_dongle *dongle)
{
	struct timespec	ts;

	if (pthread_mutex_lock(&dongle->mutex) != 0)
		return (1);
	while ((dongle->owner != -1 || get_time(coder->params) < dongle->available_at) && is_it_running(coder->params))
	{
		if (dongle->owner == -1 && get_time(coder->params) < dongle->available_at)
		{
			ts = get_future_timespec(dongle->available_at - get_time(coder->params));
			pthread_cond_timedwait(&dongle->cond, &dongle->mutex, &ts);
		}
		else
			pthread_cond_wait(&dongle->cond, &dongle->mutex);
	}
	if (!is_it_running(coder->params))
	{
    	pthread_mutex_unlock(&dongle->mutex);
    	return (1);
	}
	dongle->owner = dongle->queue[0];
	pthread_mutex_unlock(&dongle->mutex);
	print_log(coder->params, coder->id, "has taken a dongle");
	return (0);
}

int	drop_dongle(t_coder *coder, t_dongle *dongle)
{
	if (pthread_mutex_lock(&dongle->mutex) != 0)
		return (1);
	remove_from_queue(coder);
	dongle->available_at = (get_time(coder->params)
			+ coder->params->dongle_cooldown);
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mutex);
	return (0);
}
