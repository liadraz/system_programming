
/*******************************************************************************
**************************** System Programming ********************************
********************************************************************************
*
*	DESCRIPTION		Exercise Memory Mapping Basics
*	AUTHOR 			Liad Raz
*	DATE			24 Dec 2021
*
*******************************************************************************/

#include <stdio.h>		/* printf, puts */
#include <stdlib.h>		/* malloc, free */


/* Static Function */
static void StaticFunc(void);

/* Extern Function */
void ExternFunc(void);


/* Const Global Variables */
const int const_global_a = 10;
const int const_global_uninit;

/* Const Global Static Variables */
static const int static_const_global_a = 100;
static const int static_const_global_uninit;

static void StaticFunc1(void);
static void StaticFunc2(void);


/* Non-Const Global Variables */
int global_a = 10;
int global_uninit;

/* Non-Const Static Global Variables */
static int static_global_a = 10;
static int static_global_uninit;


int main(void)
{
	/* Heap Variables */
	int *malloc_ptr1 = NULL;
	void *malloc_ptr2 = NULL;
	void *malloc_ptr3 = NULL;
	void *large_block = NULL;
	
	/* Const Local Variables */
	const int const_local_a = 10;
	const int const_local_uninit;

	/* Const Local Static Variables */
	static const int static_const_local_a = 100;
	static const int static_const_local_uninit;
	
	
	/* Non-Const Local Variables */
	int local_a = 10;
	int local_uninit;
	
	/* Non-Const Static Local Variables */
	static int static_local_a = 10;
	static int static_local_uninit;
	
	int *static_local_ptr = &static_local_a;
	
	/* String Literals */
	char string_literal[] = "Biscuit The Cat";
	char *string_ptr = "Biscuit The Cat";
	
	
	/* Environment Variables */ 
/*	printf("Environment => %s\n", envp[5]);*/
	
	
	/* Command line arguments */
/*	printf("CL Arguments NO.%d=> %s\n", argc, argv[0]);*/
	
	StaticFunc();
	ExternFunc();
	StaticFunc1();
	StaticFunc2();
	
	malloc_ptr1 =(int *)malloc(sizeof(int) * 10);
	malloc_ptr2 =malloc(sizeof(double) * 10);
	large_block =malloc(sizeof(int) * 500 * 1000);

	malloc_ptr3 =malloc(sizeof(double) * 10);


	free(malloc_ptr1);
	free(malloc_ptr2);
	free(large_block);
	
	return 0;
}



static void StaticFunc(void)
{
	/* Non-Const Local Variables */
	int func_local = 10;
}

static void StaticFunc1(void)
{
	/* Non-Const Local Variables */
	int var1 = 10;
	int var2 = 10;
}

static void StaticFunc2(void)
{
	/* Non-Const Local Variables */
	int var1 = 10;
}




/* proc [PID]
81988:   /home/liad/git/system_programming/a.out
Address           Kbytes     RSS   Dirty Mode  Mapping
0000555555554000       4       4       0 r---- a.out
0000555555555000       4       4       4 r-x-- a.out
0000555555556000       4       4       0 r---- a.out
0000555555557000       4       4       4 r---- a.out
0000555555558000       4       4       4 rw--- a.out
0000555555559000     132       4       4 rw---   [ anon ]
00007ffff7dc1000     148     148       0 r---- libc-2.31.so
00007ffff7de6000    1504     936       8 r-x-- libc-2.31.so
00007ffff7f5e000     296      64       0 r---- libc-2.31.so
00007ffff7fa8000       4       0       0 ----- libc-2.31.so
00007ffff7fa9000      12      12      12 r---- libc-2.31.so
00007ffff7fac000      12      12      12 rw--- libc-2.31.so
00007ffff7faf000      24      24      24 rw---   [ anon ]
00007ffff7fc9000      16       0       0 r----   [ anon ]
00007ffff7fcd000       8       8       0 r-x--   [ anon ]
00007ffff7fcf000       4       4       0 r---- ld-2.31.so
00007ffff7fd0000     140     140      24 r-x-- ld-2.31.so
00007ffff7ff3000      32      32       0 r---- ld-2.31.so
00007ffff7ffc000       4       4       4 r---- ld-2.31.so
00007ffff7ffd000       4       4       4 rw--- ld-2.31.so
00007ffff7ffe000       4       4       4 rw---   [ anon ]
00007ffffffde000     132      12      12 rw---   [ stack ]
ffffffffff600000       4       0       0 --x--   [ anon ]
---------------- ------- ------- ------- 
total kB            2500    1428     120



size -A ./a.out 
./a.out  :
section              size    addr
.interp                28     792
.note.gnu.property     32     824
.note.gnu.build-id     36     856
.note.ABI-tag          32     892
.gnu.hash              36     928
.dynsym               240     968
.dynstr               171    1208
.gnu.version           20    1380
.gnu.version_r         48    1400
.rela.dyn             192    1448
.rela.plt              96    1640
.init                  27    4096
.plt                   80    4128
.plt.got               16    4208
.plt.sec               64    4224
.text                 693    4288
.fini                  13    4984
.rodata                84    8192
.eh_frame_hdr          84    8276
.eh_frame             328    8360
.init_array             8   15776
.fini_array             8   15784
.dynamic              496   15792
.got                   96   16288
.data                  28   16384
.bss                   20   16412
.comment               42       0
.debug_aranges         48       0
.debug_info          1266       0
.debug_abbrev         349       0
.debug_line           346       0
.debug_str            975       0
Total                6002

*/


/*
(gdb) n
84		ExternFunc();
(gdb) 
85		StaticFunc1();
(gdb) 
86		StaticFunc2();
(gdb) 
88		malloc_ptr1 =(int *)malloc(sizeof(int) * 10);
(gdb) 
89		malloc_ptr2 =malloc(sizeof(double) * 10);
(gdb) 
90		large_block =malloc(sizeof(int) * 500 * 1000);
(gdb) 
92		malloc_ptr3 =malloc(sizeof(double) * 10);
(gdb) 
95		free(malloc_ptr1);
(gdb) p/x malloc_ptr1
$1 = 0x5555555596b0
(gdb) p/x malloc_ptr2
$2 = 0x5555555596e0
(gdb) p/x malloc_ptr3
$3 = 0x555555559740
(gdb) p/x large_block
$4 = 0x7ffff7bd8010
(gdb) p/x StaticFunc1
$5 = 0xf3
(gdb) p/x &StaticFunc1
$6 = 0x5555555552fd
*/

