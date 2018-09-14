/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vdarmaya <vdarmaya@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/13 21:53:22 by vdarmaya          #+#    #+#             */
/*   Updated: 2018/08/26 14:35:09 by vdarmaya         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody_woodpacker.h"

Elf64_Shdr		*get_crypt_section(Elf64_Ehdr *header)
{
	Elf64_Shdr	*section;
	uint16_t	count = 0;

	section = (void*)header + header->e_shoff;
	while (count < header->e_shnum)
	{
		if (header->e_entry < (section->sh_addr + section->sh_size) && \
			header->e_entry >= section->sh_addr)
			return (section);
		section = (void*)section + sizeof(Elf64_Shdr);
		++count;
	}
	return (NULL);
}

Elf64_Shdr	*get_section_by_type(Elf64_Ehdr *header, Elf64_Word type)
{
	Elf64_Shdr	*section;
	uint16_t	count = 0;

	section = (void*)header + header->e_shoff;
	while (count < header->e_shnum)
	{
		if (section->sh_type == type)
			return (section);
		section = (void*)section + sizeof(Elf64_Shdr);
		++count;
	}
	return (NULL);
}

Elf64_Phdr	*get_last_load_segment(Elf64_Ehdr *hdr)
{
	Elf64_Phdr		*last;
	Elf64_Phdr		*phdr;

	last = NULL;
	if (!hdr->e_phoff)
		return (NULL);
	phdr = (void *)hdr + hdr->e_phoff;
	for (int i = 0; i < hdr->e_phnum; i++)
	{
		if (phdr->p_type == PT_LOAD)
		{
			if (last == NULL)
				last = phdr;
			else if (last->p_offset <= phdr->p_offset)
				last = phdr;
		}
		phdr = (void *)phdr + sizeof(Elf64_Phdr);
	}
	return (last);
}

void	*get_decrypt_data(size_t *decrypt_size)
{
	int			fd;
	struct stat	buff;
	void		*ptr;

	if ((fd = open("./obj/decrypt.o", O_RDONLY)) < 0)
		ft_exit("woody_woodpacker: Fail to open ./obj/decrypt.o", 1);
	if (fstat(fd, &buff) < 0)
		ft_exit("woody_woodpacker: Fail to open stat of ./obj/decrypt.o", 1);
	if ((ptr = mmap(0, buff.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0)) < 0)
		ft_exit("woody_woodpacker: Fail to mmap ./obj/decrypt.o", 1);
	if (close(fd) < 0)
		ft_exit("woody_woodpacker: Fail to close ./obj/decrypt.o", 1);
	*decrypt_size = buff.st_size;
	return (ptr);
}