/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgajowni <dgajowni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/31 12:32:00 by dgajowni          #+#    #+#             */
/*   Updated: 2026/11/01 18:11:43 by dgajowni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	order_dongles(t_coder *coder, t_dongle **first,
		t_dongle **second)
{
	*first = coder->dongle_l;
	*second = coder->dongle_r;
	if ((*first)->id > (*second)->id)
	{
		*first = coder->dongle_r;
		*second = coder->dongle_l;
	}
}

void	lock_dongles(t_coder *coder)
{
	t_dongle	*first;
	t_dongle	*second;

	order_dongles(coder, &first, &second);
	pthread_mutex_lock(first->mutex);
	if (second != first)
		pthread_mutex_lock(second->mutex);
}

void	match_dongles(t_coder *coder)
{
	t_args	*args;
	int		left;
	int		right;

	args = coder->args;
	right = coder->id;
	if (coder->id != 1)
		left = coder->id - 1;
	else
		left = args->params->number_of_coders;
	coder->dongle_l = &args->dongle[left - 1];
	coder->dongle_r = &args->dongle[right - 1];
}

void	release_dongle_reservation(t_coder *coder)
{
	t_args	*args;
	long	now;

	args = coder->args;
	pthread_mutex_lock(&args->arbiter_mutex);
	now = get_timestamp();
	coder->dongle_l->free = 1;
	coder->dongle_r->free = 1;
	coder->dongle_l->locktime = now;
	coder->dongle_r->locktime = now;
	pthread_cond_broadcast(&args->dongles_cond);
	pthread_mutex_unlock(&args->arbiter_mutex);
}

void	unlock_dongles(t_coder *coder)
{
	t_dongle	*first;
	t_dongle	*second;

	order_dongles(coder, &first, &second);
	if (second != first)
		pthread_mutex_unlock(second->mutex);
	pthread_mutex_unlock(first->mutex);
	release_dongle_reservation(coder);
}
