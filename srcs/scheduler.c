/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rem <rem@student.42lyon.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/04 15:42:42 by repichan          #+#    #+#             */
/*   Updated: 2026/08/21 10:50:09 by rem              ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	is_my_turn(t_coder *coder, t_dongle *dongle)
{
	return (heap_peek(dongle) == coder->id);
}
