/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rem <rem@student.42lyon.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/03 09:27:53 by repichan          #+#    #+#             */
/*   Updated: 2026/08/03 18:14:10 by rem              ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	print_log(t_params *params, int id, char *str)
{
	long long	now_time;

	now_time = get_time(params);
	pthread_mutex_lock(&params->print_mutex);
	fprintf(stdout, "%lld %d %s\n", now_time, id, str);
	pthread_mutex_unlock(&params->print_mutex);
}

long long	calculate_time(void)
{
	struct timeval	start_time;

	gettimeofday(&start_time, NULL);
	return ((long long)start_time.tv_sec * 1000 + start_time.tv_usec / 1000);
}

struct timespec	get_future_timespec(long milliseconds)
{
	struct timeval	now;
	struct timespec	ts;

	gettimeofday(&now, NULL);

	ts.tv_sec = now.tv_sec + (milliseconds / 1000);
	ts.tv_nsec = (long)now.tv_usec * 1000 + (milliseconds % 1000) * 1000000;
	ts.tv_sec += ts.tv_nsec / 1000000000;
	ts.tv_nsec %= 1000000000;

	return ts;
}

long long	get_time(t_params *params)
{
	return (calculate_time() - params->start_time);
}

void	sleep_until_ms(t_params *params, long long ms_duration)
{
	long long	start;

	start = get_time(params);
	while (get_time(params) - start < ms_duration)
		usleep(500);
}

