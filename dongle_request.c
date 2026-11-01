/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_request.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgajowni <dgajowni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/10 11:20:22 by dgajowni          #+#    #+#             */
/*   Updated: 2026/11/01 15:29:22 by dgajowni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	wait_for_grant(t_args *args, t_coder *coder)
{
	t_coder			*head_coder;
	struct timespec	deadline;

	while (!args->stop && coder->request_state != REQUEST_GRANTED)
	{
		dispatch_locked(args);
		if (args->stop || coder->request_state == REQUEST_GRANTED)
			break ;
		head_coder = args->requests[0].coder;
		if (!head_coder->dongle_l->free || !head_coder->dongle_r->free)
			pthread_cond_wait(&args->dongles_cond, &args->arbiter_mutex);
		else
		{
			get_deadline(head_coder, &deadline);
			pthread_cond_timedwait(&args->dongles_cond,
				&args->arbiter_mutex, &deadline);
		}
	}
	return (coder->request_state == REQUEST_GRANTED);
}

int	request_dongles(t_coder *coder)
{
	t_args	*args;

	args = coder->args;
	pthread_mutex_lock(&args->arbiter_mutex);
	if (coder->request_state == REQUEST_NONE && !add_request(coder))
	{
		pthread_mutex_unlock(&args->arbiter_mutex);
		return (0);
	}
	if (!wait_for_grant(args, coder))
	{
		pthread_mutex_unlock(&args->arbiter_mutex);
		return (0);
	}
	coder->request_state = REQUEST_NONE;
	pthread_mutex_unlock(&args->arbiter_mutex);
	return (1);
}
