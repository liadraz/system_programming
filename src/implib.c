
/*******************************************************************************
************************* - MEMORY_MAPPING_LIBRARY - ***************************
********************************************************************************
*
*	DESCRIPTION		Implicit Shared Library
*	AUTHOR 			Liad Raz
*	DATE			28 Dec 2021
*
*******************************************************************************/

int const_global = 100;
int const_global_uninit;

char *global_str_liter = "Biscuit";
char global_str_arr[] = "Biscuit";


void ImplicitShLib(void)
{
	int  const const_loc = 100;
	int  const const_loc_uninit;
	int const static static_const_loc = 200;
	
	int loc = 200;
	int static static_loc = 200;
	
	char *str_loc = "Biscuit";
	char str_arr_loc[] = "Biscuit";

}

