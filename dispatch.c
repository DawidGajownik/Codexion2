/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dispatch.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgajowni <dgajowni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/31 12:32:00 by dgajowni          #+#    #+#             */
/*   Updated: 2026/11/11 21:32:01 by dgajowni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	candidate_ready(t_args *args, int index)
{
	t_coder	*coder;

	coder = args->requests[index].coder;
	return (
		coder->dongle_l->free
		&& coder->dongle_r->free
		&& cool_enough(coder));
}

static int	find_best_candidate(t_args *args)
{
	int	i;
	int	best;

	i = 0;
	best = -1;
	while (i < args->request_count)
	{
		if (candidate_ready(args, i)
			&& (best == -1 || request_has_higher_priority(args,
					&args->requests[i], &args->requests[best])))
			best = i;
		i++;
	}
	return (best);
}

static void	grant_request(t_args *args, int index)
{
	t_coder	*coder;
	long	now;
	long	elapsed;

	coder = args->requests[index].coder;
	coder->dongle_l->free = 0;
	coder->dongle_r->free = 0;
	now = get_timestamp();
	elapsed = now - args->timestart;
	pthread_mutex_lock(&args->output_mutex);
	printf("%ld %d has taken a dongle\n", elapsed, coder->id);
	if (coder->dongle_r != coder->dongle_l)
		printf("%ld %d has taken a dongle\n", elapsed, coder->id);
	pthread_mutex_unlock(&args->output_mutex);
	coder->request_state = REQUEST_GRANTED;
	remove_request_at(args, index);
	pthread_cond_broadcast(&args->dongles_cond);
}

void	dispatch_locked(t_args *args)
{
	int	best;

	while (!args->stop && args->request_count > 0)
	{
		best = find_best_candidate(args);
		if (best < 0)
			return ;
		grant_request(args, best);
	}
}
