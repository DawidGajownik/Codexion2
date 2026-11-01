/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   priority_queue.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgajowni <dgajowni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/31 12:32:00 by dgajowni          #+#    #+#             */
/*   Updated: 2026/11/01 15:29:47 by dgajowni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	request_has_higher_priority(t_args *args, t_request *a, t_request *b)
{
	if (args->params->scheduler == FIFO)
		return (a->arrival_seq < b->arrival_seq);
	if (a->deadline != b->deadline)
		return (a->deadline < b->deadline);
	return (a->coder->id < b->coder->id);
}

static void	swap_requests(t_request *a, t_request *b)
{
	t_request	temp;

	temp = *a;
	*a = *b;
	*b = temp;
}

void	heap_sift_up(t_args *args, int index)
{
	int	parent;

	while (index > 0)
	{
		parent = (index - 1) / 2;
		if (!request_has_higher_priority(args, &args->requests[index],
				&args->requests[parent]))
			break ;
		swap_requests(&args->requests[index], &args->requests[parent]);
		index = parent;
	}
}

int	add_request(t_coder *coder)
{
	t_args		*args;
	t_request	*request;
	int			index;

	args = coder->args;
	index = args->request_count;
	if (index >= args->request_capacity)
		return (0);
	request = &args->requests[index];
	request->arrival_seq = args->next_arrival_seq++;
	request->coder = coder;
	request->deadline = coder->last_comp
		+ args->params->time_to_burnout;
	args->request_count++;
	coder->request_state = REQUEST_PENDING;
	heap_sift_up(args, index);
	return (1);
}
