/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgajowni <dgajowni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/10 11:19:11 by dgajowni          #+#    #+#             */
/*   Updated: 2026/11/01 18:26:39 by dgajowni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	init_coders(t_args *args)
{
	int			counter;
	t_coder		**coder;
	t_params	*params;

	counter = 0;
	coder = args->coder;
	params = args->params;
	while (counter < params->number_of_coders)
	{
		coder[counter] = malloc(sizeof(t_coder));
		coder[counter]->id = counter + 1;
		coder[counter]->last_comp = 0;
		coder[counter]->args = args;
		coder[counter]->compiles_done = 0;
		coder[counter]->request_state = REQUEST_NONE;
		counter++;
	}
}

static void	init_dongles(t_args *args)
{
	int				counter;
	t_params		*params;
	t_dongle		*dongle;
	pthread_mutex_t	*mutex;

	params = args->params;
	pthread_mutex_init(&args->arbiter_mutex, NULL);
	pthread_mutex_init(&args->output_mutex, NULL);
	pthread_cond_init(&args->dongles_cond, NULL);
	mutex = malloc(sizeof(pthread_mutex_t) * (params->number_of_coders));
	dongle = malloc(sizeof(t_dongle) * (params->number_of_coders));
	counter = 0;
	while (counter < params->number_of_coders)
	{
		pthread_mutex_init(&mutex[counter], NULL);
		dongle[counter].id = counter + 1;
		pthread_cond_init(&dongle[counter].cond, NULL);
		dongle[counter].free = 1;
		dongle[counter].locktime = get_timestamp()
			- params->dongle_cooldown;
		dongle[counter].mutex = &mutex[counter];
		counter++;
	}
	args->mutex = mutex;
	args->dongle = dongle;
}

static void	start_coder_threads(t_args *args)
{
	int	counter;

	counter = 0;
	pthread_create(&args->monitor_thread, NULL, monitor_routine, args);
	while (counter < args->params->number_of_coders)
	{
		pthread_create(&args->coder[counter]->thread, NULL, routine,
			args->coder[counter]);
		counter++;
	}
}

void	threads_start(t_args *args)
{
	int	counter;

	init_coders(args);
	init_dongles(args);
	counter = 0;
	while (counter < args->params->number_of_coders)
		match_dongles(args->coder[counter++]);
	args->timestart = get_timestamp();
	args->stop = 0;
	counter = 0;
	while (counter < args->params->number_of_coders)
	{
		args->coder[counter]->last_comp = args->timestart;
		counter++;
	}
	start_coder_threads(args);
}

void	threads_finish(t_args *args)
{
	int	counter;

	counter = 0;
	while (counter < args->params->number_of_coders)
	{
		pthread_join(args->coder[counter]->thread, NULL);
		counter++;
	}
	pthread_join(args->monitor_thread, NULL);
}
