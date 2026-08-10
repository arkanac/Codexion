/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: repichan <repichan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/04 15:42:42 by repichan          #+#    #+#             */
/*   Updated: 2026/08/10 11:51:36 by repichan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	add_to_queue(t_coder *coder)
{
	if (coder->left_dongle->queue[0] == 0)
		coder->left_dongle->queue[0] = coder->id;
	else
		coder->left_dongle->queue[1] = coder->id;
	if (coder->right_dongle->queue[0] == 0)
		coder->right_dongle->queue[0] = coder->id;
	else
		coder->right_dongle->queue[1] = coder->id;
}

void	remove_from_queue(t_coder *coder)
{
	if (coder->left_dongle->queue[1] != 0)
	{
		ft_swap(coder->left_dongle->queue[1], coder->left_dongle->queue[0]);
		coder->left_dongle->queue[1] = 0;
	}
	else
		coder->left_dongle->queue[0] = 0;
	if (coder->right_dongle->queue[1] != 0)
	{
		ft_swap(coder->right_dongle->queue[1], coder->right_dongle->queue[0]);
		coder->right_dongle->queue[1] = 0;
	}
	else
		coder->right_dongle->queue[0] = 0;
}

void	fifo_way(t_coder *coder)
{
	add_to_queue(coder);
}

int	scheduler(t_params *params, t_coder *coder)
{
	if (params->scheduler == FIFO)
	{
		fifo_way(coder);
	}
	return(0);
}