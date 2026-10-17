/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgajowni <dgajowni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/10 11:20:22 by dgajowni          #+#    #+#             */
/*   Updated: 2026/10/17 13:52:20 by dgajowni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void match_mutexes(t_coder *coder)
{
    int number_of_coders;
    t_args *args;
    int l;
    int r;

    l = 0;
    r = coder->id;
    args = coder->args;
    number_of_coders = args->params->number_of_coders;
    if (coder->id != 1)
        l = coder->id-1;
    if (coder->id == 1)
        l = args->params->number_of_coders;
    coder->dongle_l = &(args->dongle[l-1]);
    coder->dongle_r = &(args->dongle[r-1]);
}

int cool_enough(t_coder *coder)
{
    t_args *args;

    args = coder->args;
    return (get_timestamp()-coder->dongle_l->locktime >= args->params->dongle_cooldown && get_timestamp()-coder->dongle_r->locktime >= args->params->dongle_cooldown);
}

void lock_mutexes(t_coder *coder)
{
    t_args *args;
    long time;

    args = coder->args;
    pthread_mutex_lock(&args->arbiter_mutex);
    while (!coder->dongle_l->free || !coder->dongle_r->free || !cool_enough(coder))
        pthread_cond_wait(&args->dongles_cond, &args->arbiter_mutex);
    coder->dongle_l->free = 0;
    coder->dongle_r->free = 0;
    pthread_mutex_unlock(&args->arbiter_mutex);
    time = get_timestamp() - args->timestart;

    pthread_mutex_lock(coder->dongle_l->mutex);
    pthread_mutex_lock(coder->dongle_r->mutex);
    printf("%ld %d has taken l dongle %d, locktime = %ld, now = %ld, cooldown = %d, diff = %ld\n", time, coder->id, coder->dongle_l->id, coder->dongle_l->locktime, get_timestamp(), args->params->dongle_cooldown, get_timestamp()-coder->dongle_l->locktime);
    printf("%ld %d has taken r dongle %d, locktime = %ld, now = %ld, cooldown = %d, diff = %ld\n", time, coder->id, coder->dongle_r->id, coder->dongle_r->locktime, get_timestamp(), args->params->dongle_cooldown, get_timestamp()-coder->dongle_r->locktime);
    
    // if (coder->dongle_l->free == 1 && coder->dongle_r->free == 1)
    // {
    //     pthread_mutex_lock(coder->dongle_l->mutex);
    //     coder->dongle_l->free = 0;
    //     time = get_timestamp() - args->timestart;
    //     printf("%ld %d has taken l dongle %d\n", time, coder->id, coder->dongle_l->id);
    //     pthread_mutex_lock(coder->dongle_r->mutex);
    //     coder->dongle_r->free = 0;
    //     time = get_timestamp() - args->timestart;
    //     printf("%ld %d has taken r dongle %d\n", time, coder->id, coder->dongle_r->id);
    // }
}

void unlock_dongles(t_coder *coder)
{
    t_args *args;

    args = coder->args;
    
    pthread_mutex_unlock(coder->dongle_r->mutex);
    pthread_mutex_unlock(coder->dongle_l->mutex);

    pthread_mutex_lock(&args->arbiter_mutex);
    coder->dongle_l->free = 1;
    coder->dongle_r->free = 1;
    coder->dongle_l->locktime = get_timestamp();
    coder->dongle_r->locktime = get_timestamp();
    pthread_cond_broadcast(&args->dongles_cond);
    pthread_mutex_unlock(&args->arbiter_mutex);

    // pthread_mutex_unlock(coder->dongle_l->mutex);
    // coder->dongle_l->free = 1;
    // pthread_mutex_unlock(coder->dongle_r->mutex);
    // coder->dongle_r->free = 1;
}

void compile(t_coder *coder)
{
    t_args *args;
    long time;

    args = coder->args;
    if (args->timestart == 0)
        args->timestart = get_timestamp();
    match_mutexes(coder);
    lock_mutexes(coder);
    coder->last_comp = get_timestamp();
    time = coder->last_comp - args->timestart;
    coder->compiles_done++;
    printf("%ld %d is compiling %d\n", time, coder->id, coder->compiles_done);
    usleep(args->params->time_to_compile*1000);
    unlock_dongles(coder);
}

void debug(t_coder *coder)
{
    t_args *args;
    long time;

    args = coder->args;
    time = get_timestamp() - args->timestart;
    printf("%ld %d is debugging\n", time, coder->id);
    usleep(args->params->time_to_debug*1000);
}

void refractor(t_coder *coder)
{
    t_args *args;
    long time;

    args = coder->args;
    time = get_timestamp() - args->timestart;
    printf("%ld %d is refractoring\n", time, coder->id);
    usleep(args->params->time_to_refactor*1000);
}

void* routine(void* arg) {
    t_coder *coder;
    t_params *params;
    int counter;

    counter = 0;
    coder = (t_coder*)arg;
    params = ((t_args*)coder->args)->params;
    while (counter++ < params->number_of_coders)
    {
        compile(coder);
        debug(coder);
        refractor(coder);
    }

    return NULL;
}