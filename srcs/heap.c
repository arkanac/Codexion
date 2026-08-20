/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: repichan <repichan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 16:26:21 by repichan          #+#    #+#             */
/*   Updated: 2026/08/20 17:46:05 by repichan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

void heap_push(t_dongle *dongle, int coder_id)
{
	int	size;
	
	size = dongle->heap_size;
    dongle->heap[size] = coder_id;
	sift_up(dongle, size);
	dongle->heap_size += 1;
}

int heap_pop(t_dongle *dongle)
{
	int	id;
	int	size;
	
	if (dongle->heap_size == 0)
		return (0);
	id = dongle->heap[0];
	dongle->heap_size -= 1;
	size = dongle->heap_size;
	dongle->heap[0] = dongle->heap[size];
	sift_down(dongle, 0);
	return (id);
}

int heap_peek(t_dongle *dongle)
{
	if (dongle->heap_size == 0)
		return (0);
	return (dongle->heap[0]);
}


int	heap_compare(t_params *params, int id_a, int id_b)
{
    t_coder *a = &params->coders[id_a - 1];
    t_coder *b = &params->coders[id_b - 1];
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
	if (a->compile_count != b->compile_count)
		return (a->compile_count < b->compile_count);
	return (a->id < b->id);
}

void heap_swap(t_coder *a, t_coder *b)
{
    t_coder tmp;

    tmp = a;
    a = b;
    b = tmp;
}