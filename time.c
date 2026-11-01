/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgajowni <dgajowni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/04 17:36:17 by dgajowni          #+#    #+#             */
/*   Updated: 2026/10/31 12:24:53 by dgajowni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long	get_timestamp(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

void	sleep_ms(int milliseconds)
{
	long long	remaining;
	useconds_t	chunk;

	remaining = (long long)milliseconds * 1000;
	while (remaining > 0)
	{
		if (remaining > 999000)
			chunk = 999000;
		else
			chunk = (useconds_t)remaining;
		usleep(chunk);
		remaining -= chunk;
	}
}
