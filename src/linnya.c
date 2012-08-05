#include "linnya.h"

/**
 * main:
 * @argc:			the args count.
 * @argv:			the args container array.
 *
 * Code entry.
 *
 * Returns:			0 for success, others fail.
 */
int main(int argc, char *argv[])
{	
	/*
	 * Initial
	 */
	ly_cox_init(&argc, &argv);
	ly_uix_init(&argc, &argv);

	/*
	 * Finalzie
	 */
	ly_uix_fina();
	ly_cox_fina();
	
	return 0;
}
