#include "Service.h"

Cat * CreateCat(int age, char * name){
    if (age <= 0 && strlen(name) == 0) {
        return NULL;
    }
    Cat * cat = PersistCat(age,name);
    return cat;
}


Cat * GetCat(int id){
    if (id <= 0) {
        return NULL;
    }
    return FindCat(id);
}