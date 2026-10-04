/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgajowni <dgajowni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/04 17:36:17 by dgajowni          #+#    #+#             */
/*   Updated: 2026/10/04 17:42:22 by dgajowni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long get_timestamp()
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return (tv.tv_sec*1000+tv.tv_usec/1000);
}