/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   compile.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgajowni <dgajowni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/31 12:32:00 by dgajowni          #+#    #+#             */
/*   Updated: 2026/11/01 18:24:42 by dgajowni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	start_compile(t_coder *coder, long *elapsed)
{
	t_args	*args;

	args = coder->args;
	pthread_mutex_lock(&args->arbiter_mutex);
	if (args->stop)
	{
		pthread_mutex_unlock(&args->arbiter_mutex);
		return (0);
	}
	coder->last_comp = get_timestamp();
	*elapsed = coder->last_comp - args->timestart;
	pthread_mutex_unlock(&args->arbiter_mutex);
	return (1);
}

int	compile(t_coder *coder)
{
	t_args	*args;
	long	elapsed;

	args = coder->args;
	if (!request_dongles(coder))
		return (0);
	if (!start_compile(coder, &elapsed))
	{
		release_dongle_reservation(coder);
		return (0);
	}
	lock_dongles(coder);
	pthread_mutex_lock(&args->output_mutex);
	printf("%ld %d is compiling %d\n", elapsed, coder->id,
		coder->compiles_done + 1);
	pthread_mutex_unlock(&args->output_mutex);
	sleep_ms(args->params->time_to_compile);
	unlock_dongles(coder);
	pthread_mutex_lock(&args->arbiter_mutex);
	coder->compiles_done++;
	pthread_mutex_unlock(&args->arbiter_mutex);
	return (1);
}
