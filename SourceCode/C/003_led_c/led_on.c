

int  main()
{
	unsigned int *pGPF4CON = (unsigned int *)0x56000050;
	unsigned int *pGPFDAT = (unsigned int *)0x56000054;

	*pGPF4CON = 0x100;
	*pGPFDAT = 0x0;

	return 0;
}
