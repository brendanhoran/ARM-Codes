#include "lib.h"
#include "uart.h"
#include "nand.h"

#define LOAD_ADDR		0x22000000
#define SRAM_ADDR		0xD0020000

int linux(int argc, char * argv[])
{	
	void (*p)(void);	
		
	// load superboot.bin from flash 0M to iSRAM (0xD0020000)
	// size = 8K
	printf("read superboot.bin from nandflash... ok!\n");
	nand_read(0x0, (char *)SRAM_ADDR, 0x2000);
	
	// go 0xD0020000
	printf("superboot run, booting linux ... \n");
	p = (void (*)(void))SRAM_ADDR;	
	p();
	
	return 0;
}

int go(int argc, char * argv[])
{	
	void (*p)(void);	
	p = (void (*)(void))LOAD_ADDR;	
	p();
	return 0;
}

int load(int argc, char * argv[])
{
	int size;
	int i;
	char * p;
	
	// load 100	(size = 100)
	size = atoi(argv[1]);
	printf("load to address 0x%x, size %d \n", LOAD_ADDR, size);

	p = (char *)LOAD_ADDR;
	for (i = 0; i < size; i++)
		*p++ = uart_getchar();
	
	printf("load ok! \n");	
	md((char *)LOAD_ADDR, size);	
	
	return 0;
}

int shell_do(int argc, char * argv[])
{
	if (strcmp(argv[0], "load") == 0)
		load(argc, argv);

	if (strcmp(argv[0], "go") == 0)
		go(argc, argv);
		
	if (strcmp(argv[0], "linux") == 0)
		linux(argc, argv);
		
	return 0;
}

int shell_parse(char * s, char * argv[])
{
	int argc = 0;
	char * p = s;
	int state = 0;

	while (*p)
	{
		if (state == 0 && *p != ' ')
		{
			argv[argc++] = p;
			state = 1;
		}
		
		if (state == 1 && *p == ' ')
		{
			*p = '\0';
			state = 0;
		}
	
		p++;
	}

	return argc;
}