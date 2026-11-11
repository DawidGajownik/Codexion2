/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgajowni <dgajowni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/31 12:32:00 by dgajowni          #+#    #+#             */
/*   Updated: 2026/11/11 21:21:53 by dgajowni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	simulation_stopped(t_args *args)
{
	int	stop;

	pthread_mutex_lock(&args->arbiter_mutex);
	stop = args->stop;
	pthread_mutex_unlock(&args->arbiter_mutex);
	return (stop);
}

void	*routine(void *arg)
{
	t_coder	*coder;
	t_args	*args;

	coder = arg;
	args = coder->args;
	while (1)
	{
		if (!compile(coder) || simulation_stopped(args))
			break ;
		debug(coder);
		if (simulation_stopped(args))
			break ;
		refactor(coder);
	}
	return (NULL);
}
