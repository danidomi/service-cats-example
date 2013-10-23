#include "Cat.h"

// Constructor:
Cat * CatCreate(int age, int something) { 
    Cat * newCat = malloc(sizeof(Cat));

    CatInitialize(newCat, age, something);

    return newCat;
}

void CatInitialize(Cat * this, int age, int something)
{
    this->age = age;
    this->something = something;
}

// "Property" setter:
void CatSetAge(Cat * this, int age) {
    this->age = age;
}

void CatFree(Cat * this) { 
    // Do any other freeing required here.
    free(this);
}
