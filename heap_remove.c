/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_remove.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgajowni <dgajowni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/31 12:32:00 by dgajowni          #+#    #+#             */
/*   Updated: 2026/11/01 15:29:45 by dgajowni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	heap_sift_down(t_args *args, int index)
{
	int			left;
	int			right;
	int			best;
	t_request	temp;

	while (1)
	{
		left = 2 * index + 1;
		right = left + 1;
		if (left >= args->request_count)
			break ;
		best = left;
		if (right < args->request_count
			&& request_has_higher_priority(args, &args->requests[right],
				&args->requests[left]))
			best = right;
		if (!request_has_higher_priority(args, &args->requests[best],
				&args->requests[index]))
			break ;
		temp = args->requests[index];
		args->requests[index] = args->requests[best];
		args->requests[best] = temp;
		index = best;
	}
}

void	remove_request_at(t_args *args, int index)
{
	int	last;

	if (index < 0 || index >= args->request_count)
		return ;
	last = args->request_count - 1;
	args->request_count--;
	if (index == last)
		return ;
	args->requests[index] = args->requests[last];
	if (index > 0 && request_has_higher_priority(args,
			&args->requests[index],
			&args->requests[(index - 1) / 2]))
		heap_sift_up(args, index);
	else
		heap_sift_down(args, index);
}
