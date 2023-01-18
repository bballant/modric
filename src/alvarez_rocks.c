

//  Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "alvarez_rocks.h"
#include "rocksdb/c.h"

#include <unistd.h> // sysconf() - get CPU count

const char DBPath[] = "/tmp/rocksdb_c_simple_example";
const char DBBackupPath[] = "/tmp/rocksdb_c_simple_example_backup";

#define ERR(err) \
  if (err) { \
    fprintf(stderr, "Error: %s\n", err); \
    abort(); \
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



int a_main(void) {
  rocksdb_t *db;
  rocksdb_backup_engine_t *be;
  rocksdb_options_t *options = rocksdb_options_create();
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
  db = rocksdb_open(options, DBPath, &err);

  // open Backup Engine that we will use for backing up our database
  be = rocksdb_backup_engine_open(options, DBBackupPath, &err);
  assert(!err);

  // Put key-value
  char key[] = "new";
  char *value = "zippio";

  a_rocks_insert_db(db, key, value);

//  rocksdb_readoptions_t *readoptions = rocksdb_readoptions_create();
//  size_t len;
//  char *returned_value =
//      rocksdb_get(db, readoptions, key, strlen(key), &len, &err);
//  assert(!err);
  char *returned_value = a_rocks_select_db(db, key);
  assert(strcmp(returned_value, "zippio") == 0);
  printf("This works: %s\n", returned_value);
  free(returned_value);

  // create new backup in a directory specified by DBBackupPath
  rocksdb_backup_engine_create_new_backup(be, db, &err);
  assert(!err);

  rocksdb_close(db);

  // If something is wrong, you might want to restore data from last backup
  rocksdb_restore_options_t *restore_options = rocksdb_restore_options_create();
  rocksdb_backup_engine_restore_db_from_latest_backup(be, DBPath, DBPath,
                                                      restore_options, &err);
  assert(!err);
  rocksdb_restore_options_destroy(restore_options);

  db = rocksdb_open(options, DBPath, &err);
  assert(!err);

  // cleanup
  rocksdb_options_destroy(options);
  rocksdb_backup_engine_close(be);
  rocksdb_close(db);

  return 0;
}

void a_rocks_insert(char *key, char *value) {

}

void a_rocks_select(char *key) {

}

void alvarez_rocks(void) {
  printf("Hello Alvarez!\n");
  a_main();

  printf("\n");
  const char foo[] = "bar";
  const char *bar = "bar";

  printf("foo is %lu\n", strlen(foo));
  fprintf(stderr, "bar is %lu\n", strlen(bar));
}
