#include "main.h"

int main(int argc, char *argv[])
{	
	/*
	 * Initial
	 */
	ly_core_init(&argc, &argv);
	ly_ui_init();
	
	/*
	 * Finalzie
	 */
	ly_ui_finalize();
	ly_core_finalize();
	
	return 0;
}
