#ifndef ALVAREZ_ROCKS_H_
#define ALVAREZ_ROCKS_H_

void a_rocks_insert(char *db_path, char *key, char *value);
char *a_rocks_select(char *db_path, char *key);
int a_rocks_iter(char *db_path, char *key, int count, char **res);
void alvarez_rocks(void);

#endif // ALVAREZ_ROCKS_H_
