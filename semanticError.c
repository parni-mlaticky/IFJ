#include "semanticError.h"

void semanticError(int code){
    fprintf(stderr, "Semantic error code %d, exiting...\n", code);
    exit(code);
}