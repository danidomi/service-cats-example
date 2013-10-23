typedef struct Cat Cat;
Cat * CatCreate(int age, int something);
void CatSetAge(Cat * this, int age);
void CatFree(Cat * this);
