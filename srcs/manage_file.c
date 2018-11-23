#include "woody_woodpacker.h"

static void	create_woody(void *ptr, size_t size)
{
	int	fd;

	if ((fd = open("woody", O_RDWR | O_CREAT | O_TRUNC, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH )) < 0)
		ft_exit("woody_woodpacker: Fail on woody creation", 1);
	write(fd, ptr, size);
	if (close(fd) < 0)
		ft_exit("woody_woodpacker: Fail on close woody", 1);
}

static void	reindex_sections(Elf64_Ehdr *header, Elf64_Phdr *last, size_t woody_size, size_t my_code_offset, size_t ori_size)
{
	Elf64_Shdr	*section;
	size_t		count = 0;
	char		first = 0;
	long		bss_len = last->p_filesz - last->p_memsz;

	bss_len = bss_len < 0 ? bss_len * -1 : bss_len;
	header->e_shoff += woody_size + bss_len;
	section = (void*)header + header->e_shoff;
	while (count < header->e_shnum)
	{
		if (section->sh_offset >= (last->p_offset + last->p_filesz))
		{
			if (!first)
				first = 1;
			else
				section->sh_offset += woody_size + bss_len;
		}
		++section;
		++count;
	}
	section->sh_name = 0;
	section->sh_type = SHT_PROGBITS;
	section->sh_flags = SHF_EXECINSTR | SHF_ALLOC;
	section->sh_offset = my_code_offset + bss_len;
	section->sh_addr = section->sh_offset + (last->p_vaddr - last->p_offset);
	section->sh_size = woody_size;
	section->sh_addralign = 16;
	++header->e_shnum;
}

static void	update_segments_flag(Elf64_Ehdr *hdr)
{
	Elf64_Phdr		*phdr;

	phdr = (void *)hdr + hdr->e_phoff;
	for (int i = 0; i < hdr->e_phnum; i++)
	{
		if (phdr->p_type == PT_LOAD)
			phdr->p_flags = PF_X | PF_W | PF_R;
		++phdr;
	}
}

static void	manage_elf(void *ptr, size_t ori_size, char *key)
{
	void		*woody_ptr;
	void		*decrypt_ptr;
	size_t		woody_size;
	size_t		my_code_offset;
	size_t		decrypt_size;
	Elf64_Phdr	*load_segment;
	Elf64_Shdr	*tmp;
	long		bss_len;

	// 1. recup le code du decrypt puis sa section .txt
	decrypt_ptr = get_decrypt_data(&decrypt_size);
	tmp = get_section_by_type(decrypt_ptr, SHT_PROGBITS);
	woody_size = tmp->sh_size;					/* size of section .text */

	// 2. get le dernier segment PT_LOAD pour y injecter notre code apres
	load_segment = get_last_load_segment(ptr);
	
	bss_len = load_segment->p_filesz - load_segment->p_memsz;
	bss_len = bss_len < 0 ? bss_len * -1 : bss_len;
	// 3. malloc du nouveau fichier (ori_size + 1 section + inject.asm + bss_len)
	if (!(woody_ptr = malloc(ori_size + sizeof(Elf64_Shdr) + woody_size + bss_len)))
		ft_exit("woody_woodpacker: Malloc failed", 1);
	
	// 4. Copy le fichier d'ori
	my_code_offset = load_segment->p_offset + load_segment->p_filesz;
	ft_memcpy(woody_ptr, ptr, my_code_offset);	/* copy until end of load segment */
	ft_memcpy(woody_ptr + my_code_offset + woody_size + bss_len, ptr + my_code_offset, \
		ori_size - my_code_offset);				/* copy to the end after our code */

	// 5. Reindex les sections + rajout section
	reindex_sections(woody_ptr, load_segment, woody_size, my_code_offset, ori_size);

	// 6. Update les flags des segment qui ce charge en memoir pour avoir les droits d'ecriture	
	update_segments_flag(woody_ptr);
	load_segment = get_last_load_segment(woody_ptr);
	load_segment->p_filesz += woody_size + bss_len;
	load_segment->p_memsz = load_segment->p_filesz;

	encrypt_code(woody_ptr, decrypt_ptr, \
		my_code_offset + (load_segment->p_vaddr - load_segment->p_offset) + bss_len, \
		my_code_offset, key);
	
	// 7. On copy le code qui va decrypter
	ft_memcpy(woody_ptr + my_code_offset + bss_len, decrypt_ptr + tmp->sh_offset, woody_size);

	// 8. On pointe le debut du program sur notre section
	((Elf64_Ehdr*)woody_ptr)->e_entry = my_code_offset + (load_segment->p_vaddr - load_segment->p_offset) + bss_len;

	// 9. Creer le fichier woody
	create_woody(woody_ptr, ori_size + sizeof(Elf64_Shdr) + woody_size + bss_len);
	if ((munmap(decrypt_ptr, decrypt_size)) < 0)
		ft_exit("woody_woodpacker: Munmap failed", 1);
}

static void	handle_binary(int fd, struct stat buff, char *key)
{
	void	*ptr;

	if (S_ISDIR(buff.st_mode))
		ft_exit("woody_woodpacker: Is a directory\n", 1);
	else if (!S_ISREG(buff.st_mode))
		ft_exit("woody_woodpacker: Is not a regular file\n", 1);
	else if ((ptr = mmap(NULL, buff.st_size, PROT_READ, MAP_PRIVATE, fd, 0)) < 0)
		ft_exit("woody_woodpacker: Mmap failed", 1);
	if (*(int*)ptr == ELFMAG)
	{
		if (((Elf64_Ehdr*)ptr)->e_ident[4] != 2)	// check if 64bits
			ft_exit("woody_woodpacker: File architecture not suported. x64 only", 1);
		manage_elf(ptr, buff.st_size, key);
		if ((munmap(ptr, buff.st_size)) < 0)
			ft_exit("woody_woodpacker: Munmap failed", 1);
	}
	else
		ft_exit("woody_woodpacker: Not an elf file", 1);
}

void	manage_file(char *path, char *key)
{
	int				fd;
	struct stat		buff;

	if ((fd = open(path, O_RDONLY)) < 0)
		ft_exit("woody_woodpacker: No such file or directory.\n", 1);
	if (fstat(fd, &buff) < 0)
		ft_exit("woody_woodpacker: Open file failed\n", 1);
	handle_binary(fd, buff, key);
}
