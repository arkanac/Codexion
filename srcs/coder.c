/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rem <rem@student.42lyon.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/03 09:27:28 by repichan          #+#    #+#             */
/*   Updated: 2026/08/14 21:21:17 by rem              ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	coder_take_dongle(t_coder *coder)
{
	scheduler(coder->params, coder, coder->left_dongle);
	scheduler(coder->params, coder, coder->right_dongle);
	if (coder->left_dongle->id < coder->right_dongle->id)
	{
		if (take_dongle(coder, coder->left_dongle) != 0)
			return (1);
		if (take_dongle(coder, coder->right_dongle) != 0)
			return (1);
	}
	else
	{
		if (take_dongle(coder, coder->right_dongle) != 0)
			return (1);

		if (take_dongle(coder, coder->left_dongle) != 0)
			return (1);
	}
	return (0);
}

int	coder_drop_dongle(t_coder *coder)
{
	if (drop_dongle(coder, coder->left_dongle) != 0)
		return (1);
	if (drop_dongle(coder, coder->right_dongle) != 0)
		return (1);
	return (0);
}

int	coder_action(t_coder *coder)
{
	while (coder->compile_count < coder->params->number_of_compiles_required
		&& is_it_running(coder->params))
	{
		if (coder_take_dongle(coder) != 0)
			return (1);
		print_log(coder->params, coder->id, "is compiling");
		if (pthread_mutex_lock(&coder->mutex) != 0)
			return (1);
		coder->last_compile_start = get_time(coder->params);
		pthread_mutex_unlock(&coder->mutex);
		sleep_until_ms(coder->params, coder->params->time_to_compile);
		if (coder_drop_dongle(coder) != 0)
			return (1);
		print_log(coder->params, coder->id, "is debugging");
		sleep_until_ms(coder->params, coder->params->time_to_debug);
		print_log(coder->params, coder->id, "is refactoring");
		sleep_until_ms(coder->params, coder->params->time_to_refactor);
		pthread_mutex_lock(&coder->mutex);
		coder->compile_count++;
		pthread_mutex_unlock(&coder->mutex);
	}
	return (0);
}
