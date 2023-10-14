#include "service.h"

int valid_input(int age, char * name){
    return age <= 0 && strlen(name) == 0 ? -1 : 1;
}

Cat * create_cat(int age, char * name){
    if (age <= 0 && strlen(name) == 0) {
        return NULL;
    }
    Cat * cat = persist_cat(age,name);
    return cat;
}

Cat * get_cat(int id){
    if (id <= 0) {
        return NULL;
    }
    return find_cat(id);
}