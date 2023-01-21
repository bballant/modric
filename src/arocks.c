#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arocks.h"
#include "rocksdb/c.h"

#include <unistd.h> // sysconf() - get CPU count

#define ERR(err)                                                               \
  if (err) {                                                                   \
    fprintf(stderr, "Error: %s\n", err);                                       \
    abort();                                                                   \
  }

void a_rocks_insert_db(rocksdb_t *db, const char *key, const char *value) {
  char *err = NULL;
  // Put key-value
  rocksdb_writeoptions_t *writeoptions = rocksdb_writeoptions_create();
  rocksdb_put(db, writeoptions, key, strlen(key), value, strlen(value) + 1,
              &err);
  ERR(err);
  rocksdb_writeoptions_destroy(writeoptions);
}

char *a_rocks_select_db(rocksdb_t *db, const char *key) {
  char *err = NULL;
  rocksdb_readoptions_t *readoptions = rocksdb_readoptions_create();
  size_t len;
  char *returned_value =
      rocksdb_get(db, readoptions, key, strlen(key), &len, &err);
  ERR(err);
  rocksdb_readoptions_destroy(readoptions);
  return returned_value;
}

rocksdb_t *a_rocks_init(char *db_path, rocksdb_options_t *options) {
  // Optimize RocksDB. This is the easiest way to
  // get RocksDB to perform well.
  long cpus = sysconf(_SC_NPROCESSORS_ONLN);
  // Set # of online cores
  rocksdb_options_increase_parallelism(options, (int)(cpus));
  rocksdb_options_optimize_level_style_compaction(options, 0);
  // create the DB if it's not already present
  rocksdb_options_set_create_if_missing(options, 1);
  // open DB
  char *err = NULL;
  rocksdb_t *db = rocksdb_open(options, db_path, &err);
  ERR(err);
  return db;
}

void a_rocks_insert(char *db_path, char *key, char *value) {
  rocksdb_t *db;
  rocksdb_options_t *options = rocksdb_options_create();
  db = a_rocks_init(db_path, options);
  a_rocks_insert_db(db, key, value);
  rocksdb_close(db);
  rocksdb_options_destroy(options);
}

char *a_rocks_select(char *db_path, char *key) {
  rocksdb_t *db;
  rocksdb_options_t *options = rocksdb_options_create();
  db = a_rocks_init(db_path, options);
  char *ret = a_rocks_select_db(db, key);
  rocksdb_close(db);
  rocksdb_options_destroy(options);
  return ret;
}

int a_rocks_iter(char *db_path, char *key, int count, char *res[]) {
  return 0;
}

const char *a_rocks_iter_test(char *db_path) {
  rocksdb_t *db;
  rocksdb_options_t *options = rocksdb_options_create();
  db = a_rocks_init(db_path, options);
  //char *ret = a_rocks_select_db(db, key);
  char *err = NULL;
  rocksdb_readoptions_t *readoptions = rocksdb_readoptions_create();
  rocksdb_iterator_t *iter = rocksdb_create_iterator(db, readoptions);
  rocksdb_iter_seek_to_first(iter);
  size_t vlen;
  const char *returned_value = rocksdb_iter_value(iter, &vlen);
  ERR(err);
  rocksdb_readoptions_destroy(readoptions);
  return returned_value;
}

void alvarez_rocks(void) {
  // Put key-value
  char *db_path = ".data";
  char key[] = "Few";
  char *value = "bar";

  a_rocks_insert(db_path, key, value);
  char *ret = a_rocks_select(db_path, key);
  printf("cool: %s\n", ret);
  free(ret);

  a_rocks_insert(db_path, "wild", "stallion");
  ret = a_rocks_select(db_path, "wild");
  printf("cool: %s\n", ret);
  free(ret);

  const char *ret2 = a_rocks_iter_test(db_path);
  printf("iter: %s\n", ret2);

}
