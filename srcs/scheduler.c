/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: repichan <repichan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/04 15:42:42 by repichan          #+#    #+#             */
/*   Updated: 2026/08/10 16:34:01 by repichan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	add_to_queue(t_coder *coder, t_dongle *dongle)
{
	if (pthread_mutex_lock(&dongle->mutex) != 0)
		return (1);
	if ((not_in_queue(coder, dongle) != 0))
	{
		pthread_mutex_unlock(&dongle->mutex);
		return (0);
	}
	if (dongle->queue[0] == 0)
		dongle->queue[0] = coder->id;
	else
		if (dongle->queue[1] == 0 && dongle->queue[1] != coder->id)
			dongle->queue[1] = coder->id;
	pthread_mutex_unlock(&dongle->mutex);
	return (0);
}

int	remove_from_queue(t_coder *coder, t_dongle *dongle)
{
	if (pthread_mutex_lock(&dongle->mutex) != 0)
		return (1);
	if (dongle->queue[1] != 0 && dongle->queue[1] != coder->id )
	{
		ft_swap(&dongle->queue[1], &dongle->queue[0]);
		dongle->queue[1] = 0;
	}
	else
		coder->params->dongles[dongle->id].queue[0] = 0;
	pthread_mutex_unlock(&dongle->mutex);
	return (0);
}

void	fifo_way(t_coder *coder)
{
	add_to_queue(coder, coder->left_dongle);
	add_to_queue(coder, coder->right_dongle);
	printf("Codeur %d inséré, left dongle %d queue: [%d, %d] | Right dongle %d queue: [%d, %d]\n",
    coder->id,
    coder->left_dongle->id, coder->left_dongle->queue[0], coder->left_dongle->queue[1],
    coder->right_dongle->id, coder->right_dongle->queue[0], coder->right_dongle->queue[1]);
	
}

int	scheduler(t_params *params, t_coder *coder)
{
	if (params->scheduler == FIFO)
	{
		fifo_way(coder);
	}
	return(0);
}