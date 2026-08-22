/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleaning.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: repichan <repichan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 16:26:33 by repichan          #+#    #+#             */
/*   Updated: 2026/08/22 12:29:43 by repichan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	clean_dongles(t_dongle *dongles, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		pthread_mutex_destroy(&dongles[i].mutex);
		pthread_mutex_destroy(&dongles[i].heap_mutex);
		i++;
	}
	free(dongles);
}

void	clean_coders(t_coder *coders, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		pthread_mutex_destroy(&coders[i].mutex);
		i++;
	}
	free(coders);
}

void	clean_global_mutex(t_params *params)
{
	pthread_mutex_destroy(&params->print_mutex);
	pthread_mutex_destroy(&params->state_mutex);
	pthread_mutex_destroy(&params->start_mutex);
	pthread_cond_destroy(&params->start_cond);
}

void	clean_all(t_params *params)
{
	clean_coders(params->coders, params->number_of_coders);
	clean_dongles(params->dongles, params->number_of_coders);
	clean_global_mutex(params);
}
