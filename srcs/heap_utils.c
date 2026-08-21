/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rem <rem@student.42lyon.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/21 10:31:20 by rem               #+#    #+#             */
/*   Updated: 2026/08/21 10:56:45 by rem              ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	heap_swap(int *a, int *b)
{
	int	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

void	sift_up(t_params *params, t_dongle *dongle, int index)
{
	int	parent_index;

	while (index > 0)
	{
		parent_index = (index - 1) / 2;
		if (heap_compare(params, dongle->heap[index],
				dongle->heap[parent_index]))
		{
			heap_swap(&dongle->heap[index], &dongle->heap[parent_index]);
			index = parent_index;
		}
		else
			break ;
	}
}

void	sift_down(t_params *params, t_dongle *dongle, int index)
{
	int	left_child_index;
	int	right_child_index;
	int	smallest_index;

	while (1)
	{
		left_child_index = 2 * index + 1;
		right_child_index = 2 * index + 2;
		smallest_index = index;
		if (left_child_index < dongle->heap_size
			&& heap_compare(params, dongle->heap[left_child_index],
				dongle->heap[smallest_index]))
			smallest_index = left_child_index;
		if (right_child_index < dongle->heap_size
			&& heap_compare(params, dongle->heap[right_child_index],
				dongle->heap[smallest_index]))
			smallest_index = right_child_index;
		if (smallest_index != index)
		{
			heap_swap(&dongle->heap[index], &dongle->heap[smallest_index]);
			index = smallest_index;
		}
		else
			break ;
	}
}

int	heap_compare(t_params *params, int id_a, int id_b)
{
	t_coder		*a;
	t_coder		*b;
	long long	dl_a;
	long long	dl_b;

	a = &params->coders[id_a - 1];
	b = &params->coders[id_b - 1];
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
	if (a->compile_count != b->compile_count)
		return (a->compile_count < b->compile_count);
	return (a->id < b->id);
}
