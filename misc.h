/**
 * @file misc.h
 * @author Vladimír Hucovič (xhucov00)
 * @brief header file for miscellaneous functions
 * 
 */
#pragma once
#ifndef MISC_H_
#define MISC_H_

char* str_clone(char* src);

/**
 * @brief checks whether memory has been allocated, and exits the program if not
 *
 * @param ptr address from malloc/calloc
 */
#define CHECK_ALLOCATION(ptr) if(!ptr){ \
        fprintf(stderr, "Couldn't allocate memory, exiting.\n");\
        exit(99);\
    }

#endif // MISC_H_
