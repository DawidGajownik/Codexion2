/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dispatch.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgajowni <dgajowni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/31 12:32:00 by dgajowni          #+#    #+#             */
/*   Updated: 2026/11/01 17:15:10 by dgajowni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	blocked_by_higher_priority(t_args *args, int candidate)
{
	int		i;
	t_coder	*candidate_coder;
	t_coder	*waiting_coder;

	candidate_coder = args->requests[candidate].coder;
	i = 0;
	while (i < args->request_count)
	{
		waiting_coder = args->requests[i].coder;
		if (i != candidate
			&& request_has_higher_priority(args, &args->requests[i],
				&args->requests[candidate])
			&& (candidate_coder->dongle_l == waiting_coder->dongle_l
				|| candidate_coder->dongle_l == waiting_coder->dongle_r
				|| candidate_coder->dongle_r == waiting_coder->dongle_l
				|| candidate_coder->dongle_r == waiting_coder->dongle_r))
			return (1);
		i++;
	}
	return (0);
}

static int	candidate_ready(t_args *args, int index)
{
	t_coder	*coder;

	coder = args->requests[index].coder;
	if (!coder->dongle_l->free || !coder->dongle_r->free)
		return (0);
	if (!cool_enough(coder))
		return (0);
	return (!blocked_by_higher_priority(args, index));
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
