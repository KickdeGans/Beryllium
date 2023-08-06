#ifndef MAIN_H
#define MAIN_H

#define FCARG(sw,flag) \
	if(fast_compare(argv[i],sw)==0)

#define FCARG_ACTION(flag) \
	{ \
            flag = 1; \
            i++; \
            compile_file = argv[i]; \
            break; \
        }

#endif
