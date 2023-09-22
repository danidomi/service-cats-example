typedef struct Cat Cat;
Cat * CatCreate(int age, int something);
void CatSetAge(Cat * this, int age);
int CatGetAge(Cat * this);
void CatFree(Cat * this);
