/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rem <rem@student.42lyon.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/03 09:27:28 by repichan          #+#    #+#             */
/*   Updated: 2026/08/19 00:41:53 by rem              ###   ########lyon.fr   */
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

static void	set_requesting(t_coder *coder, int value)
{
	pthread_mutex_lock(&coder->mutex);
	coder->requesting = value;
	pthread_mutex_unlock(&coder->mutex);
}

int	coder_action(t_coder *coder)
{
	if (coder->id % 2 == 0)
		usleep(1000);
	while (coder->compile_count < coder->params->number_of_compiles_required
		&& is_it_running(coder->params))
	{
		set_requesting(coder, 1);
		while (is_it_running(coder->params) && take_dongles(coder) != 0)
			usleep(50);
		set_requesting(coder, 0);
		if (!is_it_running(coder->params))
			return (0);
		do_cycle(coder);
	}
	return (0);
}