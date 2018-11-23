/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vdarmaya <vdarmaya@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/13 18:34:42 by vdarmaya          #+#    #+#             */
/*   Updated: 2018/11/23 16:15:52 by vdarmaya         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody_woodpacker.h"

int		main(int argc, char **argv)
{
	char	key[17];

	if (argc < 2 || argc > 3)
	{
		printf("usage:\t./woody_woodpacker <binary> [key]\n\n");
		printf("\t The binary have to be an ELF x64.\n");
		printf("\t The key have to be 16 characters long.\n");
		return (1);
	}
	if (argc == 3)
	{
		if (ft_strlen(argv[2]) != 16)
		{
			printf("The key is not 16 characters long.\n");
			return (1);
		}
		else
		{
			ft_memcpy(key, argv[2], 16);
			key[16] = '\0';
		}
		manage_file(argv[1], key);
	}
	else
		manage_file(argv[1], NULL);
	return (0);
}
