/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: repichan <repichan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/03 09:27:39 by repichan          #+#    #+#             */
/*   Updated: 2026/08/12 11:18:32 by repichan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	take_dongle(t_coder *coder, t_dongle *dongle)
{
	struct timespec	ts;

	if (pthread_mutex_lock(&dongle->mutex) != 0)
		return (1);
	while ((dongle->owner != -1 || get_time(coder->params) < dongle->available_at || dongle->queue[0].id != coder->id) && is_it_running(coder->params))
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
	if (dongle->queue[0].id != -1)
	{
		dongle->owner = coder->id;
		remove_from_queue(coder, dongle);
		pthread_cond_broadcast(&dongle->cond);
	}
	pthread_mutex_unlock(&dongle->mutex);
	// print_log(coder->params, coder->id, "has taken a dongle");
	printf("Coder %d a taken dongle %d\n", coder->id, dongle->id);
	return (0);
}

int	drop_dongle(t_coder *coder, t_dongle *dongle)
{
	if (pthread_mutex_lock(&dongle->mutex) != 0)
		return (1);
	dongle->available_at = (get_time(coder->params) 
			+ coder->params->dongle_cooldown);
	dongle->owner = -1;
	printf("Coder %d a dropped dongle %d\n", coder->id, dongle->id);
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mutex);
	return (0);
}
