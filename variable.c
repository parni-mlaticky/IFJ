/**
 * @file variable.c
 * @author Vladimír Hucovič (xhucov00)
 * @brief Implementace funkcí pro proměnné
 * 
 */
#include "variable.h"
#include <string.h>
#include <stdlib.h>
#include "stdio.h"
#include "misc.h"

variable* variable_clone(variable* var) {
    variable* new_var = malloc(sizeof(variable));
    CHECK_ALLOCATION(new_var)
    memcpy(new_var, var, sizeof(variable));
    new_var->name = str_clone(var->name);

    return new_var;
}
