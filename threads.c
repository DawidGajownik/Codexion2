/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgajowni <dgajowni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/10 11:19:11 by dgajowni          #+#    #+#             */
/*   Updated: 2026/10/11 18:22:53 by dgajowni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void malloc_coders(t_args *args)
{
    int counter;
    t_coder **coder;
    t_params *params;

    counter = 0;
    coder = args->coder;
    params = args->params;
    while (counter < params->number_of_coders)
    {
        coder[counter] = malloc(sizeof(t_coder));
        coder[counter]->id = counter+1;
        coder[counter]->last_comp = 0;
        coder[counter]->args = args;
        coder[counter]->compiles_done = 0;
        counter++;
    }
}

void    init_dongles(t_args *args)
{
    int counter;
    t_params *params;
    t_dongle    *dongle;
    pthread_mutex_t *mutex;

    params = args->params;
    pthread_mutex_init(&args->arbiter_mutex, NULL);
    pthread_cond_init(&args->dongles_cond, NULL);
    mutex = malloc(sizeof(pthread_mutex_t) * (params->number_of_coders));
    dongle = malloc(sizeof(t_dongle) * (params->number_of_coders));
    counter = 0;
    while (counter < params->number_of_coders)
    {
        pthread_mutex_init(&(mutex[counter]), NULL);
        dongle[counter].id = counter+1;
        pthread_cond_init(&(dongle[counter].cond), NULL);
        dongle[counter].free = 1;
        dongle[counter].mutex = &(mutex[counter]);
        counter++;
    }
    args->mutex = mutex;
    args->dongle = dongle;
}

void    threads_start(t_args *args)
{
    int counter;
    t_coder **coder;
    t_params *params;

    counter = 0;
    coder = args->coder;
    params = args->params;
    malloc_coders(args);
    init_dongles(args);
    args->timestart = get_timestamp();
    while (counter < params->number_of_coders)
    {
        pthread_create(&(coder[counter]->thread), NULL, routine, coder[counter]);
        counter++;
    }
}

void    threads_finnish(t_args *args)
{
    int counter;
    t_coder **coder;
    t_params *params;

    counter = 0;
    coder = args->coder;
    params = args->params;
    while (counter < params->number_of_coders)
        pthread_join(coder[counter++]->thread, NULL);
}