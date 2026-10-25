/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgajowni <dgajowni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/10 11:20:22 by dgajowni          #+#    #+#             */
/*   Updated: 2026/10/25 16:53:54 by dgajowni         ###   ########.fr       */
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

int up(t_request *req, int i)
{
    t_request temp;

    temp = req[i];
    req[i] = req[(i - 1) / 2];
    req[(i - 1) / 2] = temp;
    return (i - 1) / 2;
}

int should_be_upper(t_args *args, t_request *req, int i)
{
    if (args->params->scheduler == FIFO)
        return (req[i].arrival_seq < req[(i - 1) / 2].arrival_seq);
    if (req[i].deadline != req[(i - 1) / 2].deadline)
        return (req[i].deadline < req[(i - 1) / 2].deadline);
    return (req[i].coder->id < req[(i - 1) / 2].coder->id);
}

int should_be_downer(t_args *args, t_request *a, t_request *b)
{
    if (args->params->scheduler == FIFO)
        return (b->arrival_seq > a->arrival_seq);
    if (b->deadline != a->deadline)
        return (b->deadline > a->deadline);
    return (b->coder->id > a->coder->id);
}

int add_request(t_coder *coder)
{
    t_args *args;
    int i;
    t_request *req;

    args = coder->args;
    req = args->requests;
    i = args->request_count;
    if (i >= args->request_capacity)
        return (0);
    req[i].arrival_seq = args->next_arrival_seq++;
    req[i].coder = coder;
    req[i].deadline = coder->last_comp + args->params->time_to_burnout;
    args->request_count++;
    coder->request_state = REQUEST_PENDING;

    while (i > 0)
    {
        if (should_be_upper(args, req, i))
            i = up(req, i);
        else
            break;
    }
    return (1);
}

int swap(t_args *args, int i, int best)
{
    t_request temp;

    temp = args->requests[i];
    args->requests[i] = args->requests[best];
    args->requests[best] = temp;
    return (best);
}

void repair_tree(t_args *args)
{
    int best;
    int left;
    int right;
    int i;

    i = 0;
    while (1)
    {
        left = 2 * i + 1;
        right = 2 * i + 2;
        if (left >= args->request_count)
            break;
        best = left;
        if (right < args->request_count
            && should_be_downer(args, &args->requests[right],
                &args->requests[left]))
            best = right;
        if (!should_be_downer(args, &args->requests[best],
                &args->requests[i]))
            break;
        i = swap(args, i, best);
    }
}

void remove_root(t_args *args)
{
    if (args->request_count == 0)
        return;
    args->request_count--;
    if (args->request_count == 0)
        return;
    args->requests[0] = args->requests[args->request_count];
}

static void sift_down_from(t_args *args, int i)
{
    int best;
    int left;
    int right;

    while (1)
    {
        left = 2 * i + 1;
        right = 2 * i + 2;
        if (left >= args->request_count)
            break;
        best = left;
        if (right < args->request_count
            && should_be_downer(args, &args->requests[right],
                &args->requests[left]))
            best = right;
        if (!should_be_downer(args, &args->requests[best],
                &args->requests[i]))
            break;
        i = swap(args, i, best);
    }
}

static int share_dongle(t_coder *a, t_coder *b)
{
    return (a->dongle_l == b->dongle_l
        || a->dongle_l == b->dongle_r
        || a->dongle_r == b->dongle_l
        || a->dongle_r == b->dongle_r);
}

static int blocked_by_higher_priority(t_args *args, int candidate)
{
    int     i;
    t_coder *candidate_coder;
    t_coder *waiting_coder;

    candidate_coder = args->requests[candidate].coder;
    i = 0;
    while (i < args->request_count)
    {
        if (i != candidate
            && should_be_downer(args, &args->requests[i],
                &args->requests[candidate]))
        {
            waiting_coder = args->requests[i].coder;
            if (share_dongle(candidate_coder, waiting_coder))
                return (1);
        }
        i++;
    }
    return (0);
}

static void remove_at(t_args *args, int index)
{
    int last;

    if (index < 0 || index >= args->request_count)
        return;
    last = args->request_count - 1;
    args->request_count--;
    if (index == last)
        return;
    args->requests[index] = args->requests[last];
    if (index > 0 && should_be_upper(args, args->requests, index))
    {
        while (index > 0 && should_be_upper(args, args->requests, index))
            index = up(args->requests, index);
    }
    else
        sift_down_from(args, index);
}

void dispatch_locked(t_args *args)
{
    int     i;
    int     best;
    t_coder *coder;
    long    now;
    long    time;

    while (!args->stop && args->request_count > 0)
    {
        best = -1;
        i = 0;
        while (i < args->request_count)
        {
            coder = args->requests[i].coder;
            if (coder->dongle_l->free && coder->dongle_r->free
                && cool_enough(coder)
                && !blocked_by_higher_priority(args, i))
            {
                if (best == -1
                    || should_be_downer(args, &args->requests[i],
                        &args->requests[best]))
                    best = i;
            }
            i++;
        }
        if (best == -1)
            return;

        coder = args->requests[best].coder;
        coder->dongle_l->free = 0;
        coder->dongle_r->free = 0;
        pthread_mutex_lock(coder->dongle_l->mutex);
        pthread_mutex_lock(coder->dongle_r->mutex);

        now = get_timestamp();
        time = now - args->timestart;
        printf("%ld %d has taken l dongle %d\n",
            time, coder->id, coder->dongle_l->id);
        printf("%ld %d has taken r dongle %d\n",
            time, coder->id, coder->dongle_r->id);

        coder->request_state = REQUEST_GRANTED;
        remove_at(args, best);
        pthread_cond_broadcast(&args->dongles_cond);
    }
}

// void dispatch_locked(t_args *args)
// {
//     t_request req;
//     t_coder *coder;
//     long time;
//     int i;
//     int best;

//     i = 0;
//     best = -1;
//     if (args->stop || args->request_count == 0)
//         return;
//     while (i < args->request_count)
//     {
//         req = args->requests[i];
//         coder = req.coder;
//         if (coder->dongle_l->free && coder->dongle_r->free
//             && cool_enough(coder))
//         {
//             if (best == -1
//                 || should_be_downer(args, &args->requests[i],
//                 &args->requests[best]))
//             best = i;
//         }
//         i++;
//     }
//     if (!coder->dongle_l->free || !coder->dongle_r->free || !cool_enough(coder))
//         return;
//     coder->dongle_l->free = 0;
//     coder->dongle_r->free = 0;
//     pthread_mutex_lock(coder->dongle_l->mutex);
//     pthread_mutex_lock(coder->dongle_r->mutex);
//     time = get_timestamp() - args->timestart;
//     printf("%ld %d has taken l dongle %d, locktime = %ld, now = %ld, cooldown = %d, diff = %ld\n", time, coder->id, coder->dongle_l->id, coder->dongle_l->locktime, get_timestamp(), args->params->dongle_cooldown, get_timestamp()-coder->dongle_l->locktime);
//     printf("%ld %d has taken r dongle %d, locktime = %ld, now = %ld, cooldown = %d, diff = %ld\n", time, coder->id, coder->dongle_r->id, coder->dongle_r->locktime, get_timestamp(), args->params->dongle_cooldown, get_timestamp()-coder->dongle_r->locktime);
//     coder->request_state = REQUEST_GRANTED;
//     remove_root(args);
//     repair_tree(args);
//     pthread_cond_broadcast(&args->dongles_cond);
// }

int lock_mutexes(t_coder *coder)
{
    t_args          *args;
    t_coder         *head_coder;
    struct timespec deadline;

    args = coder->args;
    pthread_mutex_lock(&args->arbiter_mutex);

    if (coder->request_state == REQUEST_NONE && !add_request(coder))
    {
        pthread_mutex_unlock(&args->arbiter_mutex);
        return (0);
    }

    while (!args->stop && coder->request_state != REQUEST_GRANTED)
    {
        dispatch_locked(args);

        if (args->stop || coder->request_state == REQUEST_GRANTED)
            break;

        /*
         * dispatch_locked próbuje przyznać zasoby requestowi
         * z korzenia kopca, więc czekamy na warunek dotyczący
         * właśnie tego codera.
         */
        head_coder = args->requests[0].coder;

        if (!head_coder->dongle_l->free || !head_coder->dongle_r->free)
        {
            pthread_cond_wait(&args->dongles_cond, &args->arbiter_mutex);
        }
        else
        {
            get_deadline(head_coder, &deadline);
            pthread_cond_timedwait(
                &args->dongles_cond, &args->arbiter_mutex, &deadline);
        }
    }

    if (args->stop && coder->request_state != REQUEST_GRANTED)
    {
        pthread_mutex_unlock(&args->arbiter_mutex);
        return (0);
    }

    coder->request_state = REQUEST_NONE;
    pthread_mutex_unlock(&args->arbiter_mutex);
    return (1);
}

// int lock_mutexes(t_coder *coder)
// {
//     t_args *args;

//     args = coder->args;
//     pthread_mutex_lock(&args->arbiter_mutex);

//     if (coder->request_state == REQUEST_NONE)
//         if (!add_request(coder))
//         {
//             pthread_mutex_unlock(&args->arbiter_mutex);
//             return (0);
//         }
//     dispatch_locked(args);
//     while (!args->stop && coder->request_state != REQUEST_GRANTED)
//     {
//         pthread_cond_wait(&args->dongles_cond, &args->arbiter_mutex);
//         dispatch_locked(args);   
//     }
//     if (args->stop && coder->request_state != REQUEST_GRANTED)
//     {
//         pthread_mutex_unlock(&args->arbiter_mutex);
//         return (0);
//     }
//     coder->request_state = REQUEST_NONE;
//     pthread_mutex_unlock(&args->arbiter_mutex);

//     return(1);
// }

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
}

int compile(t_coder *coder)
{
    t_args *args;
    long time;

    args = coder->args;
    if (args->timestart == 0)
        args->timestart = get_timestamp();
    match_mutexes(coder);
    if (!lock_mutexes(coder))
        return (0);
    pthread_mutex_lock(&args->arbiter_mutex);
    if (args->stop)
    {
        pthread_mutex_unlock(&args->arbiter_mutex);
        unlock_dongles(coder);
        return (0);
    }
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
    return (1);
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