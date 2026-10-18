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

void *monitor_routine(void *arg)
{
    t_args  *args;
    long    now;
    int     i;
    int     all_done;

    args = arg;
    while (1)
    {
        usleep(1000);
        pthread_mutex_lock(&args->arbiter_mutex);

        all_done = 1;
        i = 0;
        while (i < args->params->number_of_coders)
        {
            if (args->coder[i]->compiles_done
                < args->params->number_of_compiles_required)
                all_done = 0;
            i++;
        }
        if (all_done)
        {
            args->stop = 1;
            pthread_cond_broadcast(&args->dongles_cond);
            pthread_mutex_unlock(&args->arbiter_mutex);
            return (NULL);
        }

        now = get_timestamp();
        i = 0;
        while (i < args->params->number_of_coders)
        {
            /* BEGIN: Do not monitor coders that completed their required work. */
            if (args->coder[i]->compiles_done
                    < args->params->number_of_compiles_required
                && now - args->coder[i]->last_comp
                >= args->params->time_to_burnout)
            {
                args->stop = 1;
                printf("%ld %d burned out\n",
                    now - args->timestart, args->coder[i]->id);
                pthread_cond_broadcast(&args->dongles_cond);
                pthread_mutex_unlock(&args->arbiter_mutex);
                return (NULL);
            }
            /* END: Do not monitor coders that completed their required work. */
            i++;
        }
        pthread_mutex_unlock(&args->arbiter_mutex);
    }
}

void match_mutexes(t_coder *coder)
{
    //int number_of_coders;
    t_args *args;
    int l;
    int r;

    l = 0;
    r = coder->id;
    args = coder->args;
    //number_of_coders = args->params->number_of_coders;
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
    return (
        (get_timestamp()-coder->dongle_l->locktime >= args->params->dongle_cooldown
        && get_timestamp()-coder->dongle_r->locktime >= args->params->dongle_cooldown)
        );
}

void get_deadline(t_coder *coder, struct timespec *deadline)
{
    t_args *args;
    long left_ready_ms;
    long right_ready_ms;
    long ready_ms;

    args = coder->args;
    left_ready_ms = coder->dongle_l->locktime
        + args->params->dongle_cooldown;
    right_ready_ms = coder->dongle_r->locktime
        + args->params->dongle_cooldown;

    ready_ms = left_ready_ms;
    if (right_ready_ms > ready_ms)
        ready_ms = right_ready_ms;

    deadline->tv_sec = ready_ms / 1000;
    deadline->tv_nsec = (ready_ms % 1000) * 1000000L;
}

int lock_mutexes(t_coder *coder)
{
    t_args *args;
    long time;
    struct timespec deadline;

    args = coder->args;
    pthread_mutex_lock(&args->arbiter_mutex);
    while (1)
    {
        /* BEGIN: Stop waiting when the monitor ends the simulation. */
        if (args->stop)
        {
            pthread_mutex_unlock(&args->arbiter_mutex);
            return (0);
        }
        /* END: Stop waiting when the monitor ends the simulation. */
        if (coder->dongle_l->free && coder->dongle_r->free
            && cool_enough(coder))
            break;
        if (!coder->dongle_l->free || !coder->dongle_r->free)
            pthread_cond_wait(&args->dongles_cond, &args->arbiter_mutex);
        else
        {
            get_deadline(coder, &deadline);
            pthread_cond_timedwait(&args->dongles_cond, &args->arbiter_mutex, &deadline);
        }
    }

    coder->dongle_l->free = 0;
    coder->dongle_r->free = 0;
    pthread_mutex_unlock(&args->arbiter_mutex);
    time = get_timestamp() - args->timestart;

    pthread_mutex_lock(coder->dongle_l->mutex);
    pthread_mutex_lock(coder->dongle_r->mutex);
    printf("%ld %d has taken l dongle %d, locktime = %ld, now = %ld, cooldown = %d, diff = %ld\n", time, coder->id, coder->dongle_l->id, coder->dongle_l->locktime, get_timestamp(), args->params->dongle_cooldown, get_timestamp()-coder->dongle_l->locktime);
    printf("%ld %d has taken r dongle %d, locktime = %ld, now = %ld, cooldown = %d, diff = %ld\n", time, coder->id, coder->dongle_r->id, coder->dongle_r->locktime, get_timestamp(), args->params->dongle_cooldown, get_timestamp()-coder->dongle_r->locktime);
    return (1);

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

int compile(t_coder *coder)
{
    t_args *args;
    long time;

    args = coder->args;
    if (args->timestart == 0)
        args->timestart = get_timestamp();
    match_mutexes(coder);
    /* BEGIN: Propagate simulation stop instead of continuing to compile. */
    if (!lock_mutexes(coder))
        return (0);
    /* END: Propagate simulation stop instead of continuing to compile. */
    pthread_mutex_lock(&args->arbiter_mutex);
    /* BEGIN: Do not start a compile after the monitor has stopped the run. */
    if (args->stop)
    {
        pthread_mutex_unlock(&args->arbiter_mutex);
        unlock_dongles(coder);
        return (0);
    }
    /* END: Do not start a compile after the monitor has stopped the run. */
    coder->last_comp = get_timestamp();
    pthread_mutex_unlock(&args->arbiter_mutex);
    time = coder->last_comp - args->timestart;
    printf("%ld %d is compiling %d\n", time, coder->id,
        coder->compiles_done + 1);
    usleep(args->params->time_to_compile*1000);
    unlock_dongles(coder);
    pthread_mutex_lock(&args->arbiter_mutex);
    coder->compiles_done++;
    pthread_mutex_unlock(&args->arbiter_mutex);
    /* BEGIN: Report that this compile completed successfully. */
    return (1);
    /* END: Report that this compile completed successfully. */
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
    t_args *args;
    int stop;

    coder = (t_coder*)arg;
    args = coder->args;
    params = args->params;
    /* BEGIN: Check shared stop/completion state between coder phases. */
    while (1)
    {
        if (!compile(coder))
            break;
        pthread_mutex_lock(&args->arbiter_mutex);
        stop = args->stop
            || coder->compiles_done >= params->number_of_compiles_required;
        pthread_mutex_unlock(&args->arbiter_mutex);
        if (stop)
            break;
        debug(coder);
        pthread_mutex_lock(&args->arbiter_mutex);
        stop = args->stop;
        pthread_mutex_unlock(&args->arbiter_mutex);
        if (stop)
            break;
        refractor(coder);
    }
    /* END: Check shared stop/completion state between coder phases. */

    return NULL;
}