/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rem <rem@student.42lyon.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/03 09:27:39 by repichan          #+#    #+#             */
/*   Updated: 2026/08/03 19:16:22 by rem              ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	take_dongle(t_coder *coder, t_dongle *dongle)
{
	struct timespec	ts;

	if (pthread_mutex_lock(&dongle->mutex) != 0)
		return (1);
	while (dongle->owner != -1 || get_time(coder->params) < dongle->available_at)
	{
		if (dongle->owner == -1 && get_time(coder->params) < dongle->available_at)
		{
			ts = get_future_timespec(dongle->available_at - get_time(coder->params));
			pthread_cond_timedwait(&dongle->cond, &dongle->mutex, &ts);
		}
		else
			pthread_cond_wait(&dongle->cond, &dongle->mutex);
	}
	dongle->owner = coder->id;
	pthread_mutex_unlock(&dongle->mutex);
	return (0);
}

int	drop_dongle(t_coder *coder, t_dongle *dongle)
{
	if (pthread_mutex_lock(&dongle->mutex) != 0)
		return (1);
	dongle->owner = -1;
	dongle->available_at = (get_time(coder->params)
			+ coder->params->dongle_cooldown);
	print_log(coder->params, coder->id, "has dropped a dongle");
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mutex);
	return (0);
}
