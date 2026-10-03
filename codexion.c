/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgajowni <dgajowni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/03 14:24:36 by dgajowni          #+#    #+#             */
/*   Updated: 2026/10/03 18:10:46 by dgajowni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long get_timestamp()
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return (tv.tv_sec*1000+tv.tv_usec/1000);
}

void print_args(char **argv)
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

int main(int argc, char **argv)
{
    long timestart;
    long last_time;
    int c;

    c = 1;
    timestart = get_timestamp();
    last_time = timestart;
    printf("Valid = %d\n", args_valid(argc, argv));
    print_args(argv);
    printf("Timestamp                      %ld\n", timestart);
    while (c < argc-1)
    {
        usleep(1000*atoi(argv[c]));
        printf("%ld\n", get_timestamp() - last_time);
        last_time = get_timestamp();
        c++;
    }
    return (0);
}