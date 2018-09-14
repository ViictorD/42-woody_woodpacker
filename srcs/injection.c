/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   injection.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vdarmaya <vdarmaya@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/26 14:38:09 by vdarmaya          #+#    #+#             */
/*   Updated: 2018/09/02 18:16:26 by vdarmaya         ###   ########.fr       */
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

static void		edit_decrypt(Elf64_Ehdr *woody_header, Elf64_Shdr *text_sec, Elf64_Ehdr *decrypt_ptr, char *key)
{
	Elf64_Shdr	*decrypt_text_sec;
	void		*edit_data;

	decrypt_text_sec = (void*)decrypt_ptr + decrypt_ptr->e_shoff + sizeof(Elf64_Shdr);	// .text section
	edit_data = (void*)decrypt_ptr + decrypt_text_sec->sh_offset + decrypt_text_sec->sh_size;	// on set le ptr sur la fin de la section .text
	edit_data -= 40;														// on set le ptr sur start_address
	reverse_write(edit_data, woody_header->e_entry);
	edit_data += 8;															// on set le ptr sur l'address a decrypter
	reverse_write(edit_data, text_sec->sh_addr);
	edit_data += 8;															// on set le ptr sur la key
	ft_memcpy(edit_data, key, 16);
	edit_data += 16;														// on set le ptr sur la taille a decrypter
	reverse_write(edit_data, text_sec->sh_size);
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

void	test_sym(Elf64_Ehdr *woody_header, void *decrypt_ptr)
{
	Elf64_Sym	*sym;
	Elf64_Shdr	*symtab;
	void		*strtab;
	int			to_write = 0;
	int			inst_offset = 0;


	symtab = get_section_by_type(decrypt_ptr, 2);
	strtab = get_strtab(decrypt_ptr);
	sym = decrypt_ptr + symtab->sh_offset;
	while ((void *)sym < (decrypt_ptr + symtab->sh_offset + symtab->sh_size))
	{
		if (!ft_strcmp(strtab + sym->st_name, "start_address"))
		{
			printf("st_value: %lu\n", sym->st_value);
			inst_offset = 0x208220 + sym->st_value;
			printf("inst_offset: %d\n", inst_offset);
			to_write = 0x1c50 - inst_offset;
			to_write = to_write < 0 ? -to_write : to_write;
			printf("to_write: %d\n", to_write);
		}
		++sym;
	}
}

void			encrypt_code(Elf64_Ehdr *woody_header, void *decrypt_ptr)
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
		// encrypt_me(inject_ptr, key);
		inject_ptr += 16;
		count += 16;
	}
	test_sym(woody_header, decrypt_ptr);
	edit_decrypt(woody_header, text_sec, decrypt_ptr, key);
}

// les binaire qui marche pas sont des 