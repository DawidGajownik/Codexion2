/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   checkers.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgajowni <dgajowni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/04 17:45:55 by dgajowni          #+#    #+#             */
/*   Updated: 2026/10/04 17:47:02 by dgajowni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void print_args(char **argv)
{
    printf("number_of_coders               %s\n", argv[1]);
    printf("time_to_burnout                %s\n", argv[2]);
    printf("time_to_compile                %s\n", argv[3]);
    printf("time_to_debug                  %s\n", argv[4]);
    printf("time_to_refactor               %s\n", argv[5]);
    printf("number_of_compiles_required    %s\n", argv[6]);
    printf("dongle_cooldown                %s\n", argv[7]);
    printf("scheduler                      %s\n", argv[8]);
}


static void    print_timestamps(int argc, char **argv, long timestart)
{
    int c;
    long last_time;

    c = 1;
    last_time = timestart;
    while (c < argc-1)
    {
        usleep(1000*atoi(argv[c]));
        printf("%ld\n", get_timestamp() - last_time);
        last_time = get_timestamp();
        c++;
    }
}

void checkers(int argc, char **argv, long timestart)
{
    printf("Valid = %d\n", args_valid(argc, argv));
    print_args(argv);
    printf("Timestamp                      %ld\n", timestart);
    print_timestamps(argc, argv, timestart);
}