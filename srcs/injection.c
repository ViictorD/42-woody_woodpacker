/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   injection.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vdarmaya <vdarmaya@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/26 14:38:09 by vdarmaya          #+#    #+#             */
/*   Updated: 2018/11/23 15:29:14 by vdarmaya         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody_woodpacker.h"

static void 		rand_key(char *str)
{
	const char	charset[] = "abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	srand(time(NULL));
	for (char n = 0; n < 16; n++)
		str[n] = charset[rand() % 62];
	str[16] = '\0';
}

static void		reverse_write(unsigned char *ptr, uint64_t nb)
{
	char	c;

	for (int n = 0; n < 8; n++)
	{
		*ptr = (unsigned char)((nb >> (8 * n)) & 0xFF);
		++ptr;
	}
}

static Elf64_Shdr	*get_text_section(Elf64_Ehdr *woody_header)
{
	Elf64_Shdr	*sec;
	Elf64_Shdr	*strtab;
	void		*string_table;
	Elf64_Sym	*sym;

	sec = (void*)woody_header + woody_header->e_shoff;
	for (int i = 0; i < woody_header->e_shnum; i++)
	{
		if (sec->sh_type == SHT_STRTAB)
			strtab = sec;
		++sec;
	}
	
	string_table = (void*)woody_header + strtab->sh_offset;
	sec = (void*)woody_header + woody_header->e_shoff;
	
	for (int i = 0; i < woody_header->e_shnum; i++)
	{
		if (!ft_strcmp(string_table + sec->sh_name, ".text"))
			return (sec);
		++sec;
	}
	ft_exit("No \".text\" section found !", 1);
	return (NULL);
}

static void			*get_strtab(Elf64_Ehdr *hdr)
{
	Elf64_Shdr		*sec_strtab = NULL;
	Elf64_Shdr		*shdr = (void *)hdr + hdr->e_shoff;

	for (int i = 0; i < hdr->e_shnum; ++i)
	{
		if (i != hdr->e_shstrndx && shdr->sh_type == SHT_STRTAB)
		{
			sec_strtab = shdr;
			break;
		}
		shdr = (void *)shdr + sizeof(Elf64_Shdr);
	}
	if (!sec_strtab)
		return (NULL);
	return ((void *)hdr + sec_strtab->sh_offset);
}

static void		edit_decrypt(Elf64_Ehdr *woody_header, Elf64_Ehdr *decrypt_ptr, \
	Elf64_Addr new_entry_point, size_t my_code_offset, Elf64_Shdr *text_sec, char *key)
{
	Elf64_Sym	*sym;
	Elf64_Shdr	*symtab;
	void		*strtab;
	int			to_write = 0;
	int			inst_offset = 0;
	Elf64_Shdr	*text_shdr = (void*)decrypt_ptr + decrypt_ptr->e_shoff + sizeof(Elf64_Shdr);	// .text section
	void		*edit_data = (void*)decrypt_ptr + text_shdr->sh_offset + text_shdr->sh_size;


	symtab = get_section_by_type(decrypt_ptr, 2);
	strtab = get_strtab(decrypt_ptr);
	sym = (void*)decrypt_ptr + symtab->sh_offset;
	while ((void *)sym < ((void*)decrypt_ptr + symtab->sh_offset + symtab->sh_size))
	{
		if (!ft_strcmp(strtab + sym->st_name, "start_address"))
		{
			inst_offset = new_entry_point + sym->st_value;
			to_write = woody_header->e_entry - inst_offset;
			to_write = to_write < 0 ? -to_write : to_write;
			edit_data -= 40;
			reverse_write(edit_data, to_write);
			edit_data += 40;
		}
		else if (!ft_strcmp(strtab + sym->st_name, "crypted_address"))
		{
			inst_offset = new_entry_point + sym->st_value;
			to_write = text_sec->sh_addr - inst_offset;
			to_write = to_write < 0 ? -to_write : to_write;
			edit_data -= 32;
			reverse_write(edit_data, to_write);
			edit_data += 32;
		}
		else if (!ft_strcmp(strtab + sym->st_name, "key"))
		{
			edit_data -= 24;
			ft_memcpy(edit_data, key, 16);
			edit_data += 24;
		}
		else if (!ft_strcmp(strtab + sym->st_name, "len"))
		{
			edit_data -= 8;
			reverse_write(edit_data, text_sec->sh_size);
			edit_data += 8;
		}
		++sym;
	}
}

void			encrypt_code(Elf64_Ehdr *woody_header, void *decrypt_ptr, \
					Elf64_Addr new_entry_point, size_t my_code_offset)
{
	void		*inject_ptr;
	Elf64_Shdr	*text_sec;
	size_t		count = 0;
	char		key[17];

	rand_key(key);
	printf("KEY: %s\n", key);
	text_sec = get_text_section(woody_header);	// .text section
	inject_ptr = (void*)woody_header + text_sec->sh_offset;
	while (count < text_sec->sh_size)
	{
		encrypt_me(inject_ptr, key);
		inject_ptr += 16;
		count += 16;
	}
	edit_decrypt(woody_header, decrypt_ptr, new_entry_point, my_code_offset, text_sec, key);
}
