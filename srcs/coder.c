/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rem <rem@student.42lyon.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/03 09:27:28 by repichan          #+#    #+#             */
/*   Updated: 2026/08/21 12:19:50 by rem              ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	set_last_compile(t_coder *coder)
{
	pthread_mutex_lock(&coder->mutex);
	coder->last_compile_start = get_time(coder->params);
	pthread_mutex_unlock(&coder->mutex);
}

static void	do_cycle(t_coder *coder)
{
	print_log(coder->params, coder->id, "is compiling");
	set_last_compile(coder);
	sleep_until_ms(coder->params, coder->params->time_to_compile);
	drop_dongles(coder);
	if (!is_it_running(coder->params))
		return ;
	print_log(coder->params, coder->id, "is debugging");
	sleep_until_ms(coder->params, coder->params->time_to_debug);
	if (!is_it_running(coder->params))
		return ;
	print_log(coder->params, coder->id, "is refactoring");
	sleep_until_ms(coder->params, coder->params->time_to_refactor);
	pthread_mutex_lock(&coder->mutex);
	coder->compile_count++;
	pthread_mutex_unlock(&coder->mutex);
}

static void	request_dongles(t_coder *coder)
{
	t_dongle	*left;
	t_dongle	*right;

	left = coder->left_dongle;
	right = coder->right_dongle;
	pthread_mutex_lock(&coder->mutex);
	coder->request_time = get_time(coder->params);
	pthread_mutex_unlock(&coder->mutex);
	pthread_mutex_lock(&left->mutex);
	heap_push(coder->params, left, coder->id);
	pthread_mutex_unlock(&left->mutex);
	pthread_mutex_lock(&right->mutex);
	heap_push(coder->params, right, coder->id);
	pthread_mutex_unlock(&right->mutex);
}

static void	unrequest_dongles(t_coder *coder)
{
	t_dongle	*left;
	t_dongle	*right;

	left = coder->left_dongle;
	right = coder->right_dongle;
	pthread_mutex_lock(&left->mutex);
	heap_remove(coder->params, left, coder->id);
	pthread_mutex_unlock(&left->mutex);
	pthread_mutex_lock(&right->mutex);
	heap_remove(coder->params, right, coder->id);
	pthread_mutex_unlock(&right->mutex);
}

int	coder_action(t_coder *coder)
{
	while (coder->compile_count < coder->params->number_of_compiles_required
		&& is_it_running(coder->params))
	{
		request_dongles(coder);
		while (is_it_running(coder->params) && take_dongles(coder) != 0)
			usleep(50);
		if (!is_it_running(coder->params))
		{
			unrequest_dongles(coder);
			return (0);
		}
		do_cycle(coder);
	}
	return (0);
}
