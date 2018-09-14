/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   woody_woodpacker.h                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vdarmaya <vdarmaya@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/13 18:58:54 by vdarmaya          #+#    #+#             */
/*   Updated: 2018/08/30 17:26:19 by vdarmaya         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef	WOODY_WOODPACKER_H
# define WOODY_WOODPACKER_H

# include <unistd.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <sys/mman.h>
# include <sys/syscall.h>
# include <fcntl.h>
# include <stdlib.h>
# include <stdio.h>
# include <inttypes.h>
# include <time.h>

/*
* --------------- ELF -----------------
*/

typedef uint16_t Elf64_Half;
typedef uint32_t Elf64_Word;
// typedef	int32_t  Elf64_Sword;
typedef uint64_t Elf64_Xword;
typedef	int64_t  Elf64_Sxword;
typedef uint64_t Elf64_Addr;
typedef uint64_t Elf64_Off;
typedef uint16_t Elf64_Section;
// typedef Elf64_Half Elf64_Versym;

# define EI_NIDENT (16)
# define ELFMAG		0x464c457f	/* Magic number */

typedef struct
{
	unsigned char	e_ident[EI_NIDENT];	/* Magic number and other info */
	Elf64_Half		e_type;				/* Object file type */
	Elf64_Half		e_machine;			/* Architecture */
	Elf64_Word		e_version;			/* Object file version */
	Elf64_Addr		e_entry;			/* Entry point virtual address */
	Elf64_Off		e_phoff;			/* Program header table file offset */
	Elf64_Off		e_shoff;			/* Section header table file offset */
	Elf64_Word		e_flags;			/* Processor-specific flags */
	Elf64_Half		e_ehsize;			/* ELF header size in bytes */
	Elf64_Half		e_phentsize;		/* Program header table entry size */
	Elf64_Half		e_phnum;			/* Program header table entry count */
	Elf64_Half		e_shentsize;		/* Section header table entry size */
	Elf64_Half		e_shnum;			/* Section header table entry count */
	Elf64_Half		e_shstrndx;			/* Section header string table index */
} Elf64_Ehdr;

typedef struct
{
	Elf64_Word	sh_name;		/* Section name (string tbl index) */
	Elf64_Word	sh_type;		/* Section type */
	Elf64_Xword	sh_flags;		/* Section flags */
	Elf64_Addr	sh_addr;		/* Section virtual addr at execution */
	Elf64_Off	sh_offset;		/* Section file offset */
	Elf64_Xword	sh_size;		/* Section size in bytes */
	Elf64_Word	sh_link;		/* Link to another section */
	Elf64_Word	sh_info;		/* Additional section information */
	Elf64_Xword	sh_addralign;	/* Section alignment */
	Elf64_Xword	sh_entsize;		/* Entry size if section holds table */
} Elf64_Shdr;

# define SHT_PROGBITS	1			/* Program data */
# define SHT_STRTAB		3			/* String table */
# define DT_SYMTAB		6			/* Address of symbol table */
# define SHF_ALLOC		(1 << 1)	/* Occupies memory during execution */
# define SHF_EXECINSTR	(1 << 2)	/* Executable */

typedef struct
{
	Elf64_Word	p_type;			/* Segment type */
	Elf64_Word	p_flags;		/* Segment flags */
	Elf64_Off	p_offset;		/* Segment file offset */
	Elf64_Addr	p_vaddr;		/* Segment virtual address */
	Elf64_Addr	p_paddr;		/* Segment physical address */
	Elf64_Xword	p_filesz;		/* Segment size in file */
	Elf64_Xword	p_memsz;		/* Segment size in memory */
	Elf64_Xword	p_align;		/* Segment alignment */
} Elf64_Phdr;

# define PT_LOAD	1			/* Loadable program segment */
# define PF_X		(1 << 0)	/* Segment is executable */
# define PF_W		(1 << 1)	/* Segment is writable */
# define PF_R		(1 << 2)	/* Segment is readable */

typedef struct
{
  Elf64_Word	st_name;		/* Symbol name (string tbl index) */
  unsigned char	st_info;		/* Symbol type and binding */
  unsigned char st_other;		/* Symbol visibility */
  Elf64_Section	st_shndx;		/* Section index */
  Elf64_Addr	st_value;		/* Symbol value */
  Elf64_Xword	st_size;		/* Symbol size */
} Elf64_Sym;

/*
* ------------- END ELF ---------------
*/

/*
* --------- Woody_woodpacker ----------
*/

void			manage_file(char *path);
void			*get_decrypt_data(size_t *decrypt_size);
Elf64_Shdr		*get_crypt_section(Elf64_Ehdr *header);
Elf64_Phdr		*get_last_load_segment(Elf64_Ehdr *hdr);
Elf64_Shdr		*get_section_by_type(Elf64_Ehdr *header, Elf64_Word type);
void			encrypt_code(Elf64_Ehdr *woody_header, void *decrypt_ptr);
void			encrypt_me(void *ptr, char *key);

/*
* ------------- Library ---------------
*/

void	ft_exit(char *msg, int ret);
void	*ft_memcpy(void *dest, const void *src, size_t n);
int		ft_strcmp(const char *s1, const char *s2);

#endif