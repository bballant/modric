#ifndef ALVAREZ_ROCKS_H_
#define ALVAREZ_ROCKS_H_

void arocks_insert(char *db_path, char *key, char *value);
char *arocks_select(char *db_path, char *key);
int arocks_iter(char *db_path, char *key, int count, char *keys[], char *vals[]);
void alvarez_rocks(void);

#endif // ALVAREZ_ROCKS_H_
