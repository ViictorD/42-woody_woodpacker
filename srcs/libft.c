/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vdarmaya <vdarmaya@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/13 19:57:22 by vdarmaya          #+#    #+#             */
/*   Updated: 2018/08/26 20:37:28 by vdarmaya         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody_woodpacker.h"

size_t	ft_strlen(char *str)
{
	char	*save;

	save = str;
	while ((*str | 0))
		++str;
	return (str - save);
}

void	ft_exit(char *msg, int ret)
{
	char	c;

	c = 10;
	write(2, msg, ft_strlen(msg));
	write(2, &c, 1);
	exit(ret);
}

void	*ft_memcpy(void *dest, const void *src, size_t n)
{
	char		*dest2;
	const char	*src2;

	if (dest && src)
	{
		dest2 = dest;
		src2 = src;
		while (n--)
			*dest2++ = *src2++;
		return (dest);
	}
	return (NULL);
}

int		ft_strcmp(const char *s1, const char *s2)
{
	if (s1 && s2)
	{
		while (*s1 && *s2 && *s1 == *s2)
		{
			s1++;
			s2++;
		}
		return (*(unsigned char *)s1 - *(unsigned char *)s2);
	}
	return (0);
}
