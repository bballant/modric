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

void arocks_insert_db(rocksdb_t *db, const char *key, const char *value) {
  char *err = NULL;
  // Put key-value
  rocksdb_writeoptions_t *writeoptions = rocksdb_writeoptions_create();
  // add 1 to len to account for null character in string key and value
  rocksdb_put(db, writeoptions, key, strlen(key) + 1, value, strlen(value) + 1,
              &err);
  ERR(err);
  rocksdb_writeoptions_destroy(writeoptions);
}

char *arocks_select_db(rocksdb_t *db, const char *key) {
  char *err = NULL;
  rocksdb_readoptions_t *readoptions = rocksdb_readoptions_create();
  size_t len;
  char *returned_value =
      rocksdb_get(db, readoptions, key, strlen(key) + 1, &len, &err);
  ERR(err);
  rocksdb_readoptions_destroy(readoptions);
  return returned_value;
}

rocksdb_t *arocks_init(char *db_path, rocksdb_options_t *options) {
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

void arocks_insert(char *db_path, char *key, char *value) {
  rocksdb_t *db;
  rocksdb_options_t *options = rocksdb_options_create();
  db = arocks_init(db_path, options);
  arocks_insert_db(db, key, value);
  rocksdb_close(db);
  rocksdb_options_destroy(options);
}

char *arocks_select(char *db_path, char *key) {
  rocksdb_t *db;
  rocksdb_options_t *options = rocksdb_options_create();
  db = arocks_init(db_path, options);
  char *ret = arocks_select_db(db, key);
  rocksdb_close(db);
  rocksdb_options_destroy(options);
  return ret;
}

int arocks_iter(char *db_path, char *key, int count, char *keys[], char *vals[]) {
  rocksdb_t *db;
  rocksdb_options_t *options = rocksdb_options_create();
  db = arocks_init(db_path, options);
  rocksdb_readoptions_t *readoptions = rocksdb_readoptions_create();
  rocksdb_iterator_t *iter = rocksdb_create_iterator(db, readoptions);
  rocksdb_iter_seek(iter, key, strlen(key));
  int i = 0;
  size_t klen;
  size_t vlen;
  while(i < count && rocksdb_iter_valid(iter)) {
    const char *ret_key = rocksdb_iter_key(iter, &klen);
    const char *ret_val = rocksdb_iter_value(iter, &vlen);
    // copy into result pointers
    keys[i] = malloc(klen * sizeof(char));
    strncpy(keys[i], ret_key, klen);
    // copy into result pointers
    vals[i] = malloc(vlen * sizeof(char));
    strncpy(vals[i], ret_val, vlen);
    // move on
    rocksdb_iter_next(iter);
    i++;
  }
  return i;
}

void alvarez_rocks(void) {
  // Put key-value
  char *db_path = ".data";
  char *key = "few";
  char *value = "bar";

  arocks_insert(db_path, key, value);
  char *ret = arocks_select(db_path, key);
  printf("cool: %s\n", ret);
  free(ret);

  arocks_insert(db_path, "wild", "stallion");
  ret = arocks_select(db_path, "wild");
  printf("cool: %s\n", ret);
  free(ret);

  arocks_insert(db_path, "cool", "dude");
  arocks_insert(db_path, "rad", "hombre");
  arocks_insert(db_path, "silly", "man");

  int db_count = 5;
  char *keys[db_count];
  char *vals[db_count];
  int n = arocks_iter(db_path, "cool", db_count, keys, vals);
  if (n == 0) {
    printf("key not found\n");
  } else {
    for (int i = 0; i < n; i++) {
      printf("%s = %s\n", keys[i], vals[i]);
      free(keys[i]);
      free(vals[i]);
    }
  }
}
