/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rem <rem@student.42lyon.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/03 09:27:39 by repichan          #+#    #+#             */
/*   Updated: 2026/08/19 18:15:47 by rem              ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	lock_both(t_dongle *a, t_dongle *b)
{
	if (a->id < b->id)
	{
		pthread_mutex_lock(&a->mutex);
		pthread_mutex_lock(&b->mutex);
	}
	else
	{
		pthread_mutex_lock(&b->mutex);
		pthread_mutex_lock(&a->mutex);
	}
}

static void	unlock_both(t_dongle *a, t_dongle *b)
{
	pthread_mutex_unlock(&a->mutex);
	pthread_mutex_unlock(&b->mutex);
}

static int	dongle_ready(t_coder *coder, t_dongle *dongle)
{
	if (dongle->owner != -1)
		return (0);
	if (get_time(coder->params) < dongle->available_at)
		return (0);
	if (is_my_turn(coder, dongle) == 0)
		return (0);
	return (1);
}

int	take_dongles(t_coder *coder)
{
	t_dongle	*left;
	t_dongle	*right;

	left = coder->left_dongle;
	right = coder->right_dongle;
	if (left == right)
		return (1);
	lock_both(left, right);
	if (dongle_ready(coder, left) && dongle_ready(coder, right))
	{
		left->owner = coder->id;
		right->owner = coder->id;
		unlock_both(left, right);
		print_log(coder->params, coder->id, "has taken a dongle");
		print_log(coder->params, coder->id, "has taken a dongle");
		return (0);
	}
	unlock_both(left, right);
	return (1);
}

void	drop_dongles(t_coder *coder)
{
	t_dongle	*left;
	t_dongle	*right;
	long long	now;

	left = coder->left_dongle;
	right = coder->right_dongle;
	lock_both(left, right);
	now = get_time(coder->params);
	left->owner = -1;
	left->available_at = now + coder->params->dongle_cooldown;
	right->owner = -1;
	right->available_at = now + coder->params->dongle_cooldown;
	unlock_both(left, right);
}
