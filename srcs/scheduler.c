/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rem <rem@student.42lyon.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/04 15:42:42 by repichan          #+#    #+#             */
/*   Updated: 2026/08/19 18:14:49 by rem              ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	has_priority(t_params *params, t_coder *a, t_coder *b)
{
	long long	dl_a;
	long long	dl_b;

	if (params->scheduler == FIFO)
	{
		if (a->request_time != b->request_time)
			return (a->request_time < b->request_time);
		return (a->id < b->id);
	}
	dl_a = a->last_compile_start + params->time_to_burnout;
	dl_b = b->last_compile_start + params->time_to_burnout;
	if (dl_a != dl_b)
		return (dl_a < dl_b);
	return (1);
}

static t_coder	*neighbor(t_coder *coder, t_dongle *dongle)
{
	t_params	*p;
	int			i;

	p = coder->params;
	if (dongle == coder->left_dongle)
	{
		if (coder->id == 1)
			return (&p->coders[p->number_of_coders - 1]);
		return (&p->coders[coder->id - 2]);
	}
	if (coder->id == p->number_of_coders)
		return (&p->coders[0]);
	i = coder->id;
	return (&p->coders[i]);
}

int	is_my_turn(t_coder *coder, t_dongle *dongle)
{
	t_coder	*other;
	int		other_requesting;

	other = neighbor(coder, dongle);
	pthread_mutex_lock(&other->mutex);
	other_requesting = other->requesting;
	pthread_mutex_unlock(&other->mutex);
	if (other_requesting == 0)
		return (1);
	if (has_priority(coder->params, coder, other))
		return (1);
	return (0);
}
