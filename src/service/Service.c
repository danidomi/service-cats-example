#include "Service.h"
#include "../model/Repository.h"
#include <stdio.h>
#include <libc.h>

char* Create(int age, int something){
    Cat * cat = CatCreate(age, something);
    CatGetAge(cat);
    char * result = malloc( 100 * sizeof(char));
    sprintf(result,"{\"data\":{\"age\":%d}}", CatGetAge(cat));
    return result;
}
