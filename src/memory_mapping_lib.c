
/*******************************************************************************
************************* - MEMORY_MAPPING_LIBRARY - ***************************
********************************************************************************
*
*	DESCRIPTION		src/memory_mapping_library.c
*	AUTHOR 			Liad Raz
*	DATE			28 Dec 2021
*
*******************************************************************************/

/* Load Implicity */
void ImplicitSharedLib(void);

/* Load Explicity */


int m_const_global = 100;
int m_const_global_uninit;

char *m_global_str_liter = "Biscuit";
char m_global_str_arr[] = "Biscuit";

int main(void)
{
	
	int  const m_const_loc = 100;
	int  const m_const_loc_uninit;
	int const static m_static_const_loc = 200;
	
	int m_loc = 200;
	int static m_static_loc = 200;
	
	char *m_str_loc = "Biscuit";
	char m_str_arr_loc[] = "Biscuit";
	

	return 0;
}



