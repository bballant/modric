## Modric

Nothing to "C" here. Get it?

Modric is my all-purpose c playground, experimenting with JSON, EDN, and a database.

Currently it uses (and depends on) [rocksdb](http://rocksdb.org/) and has cJSON built in.
The JSON and EDN parts aren't talking to the rocksdb parts yet. I installed librocksdb-dev
w/ apt-get.

```
# depends on gcc, make, and librocksdb-dev

# build
$ make

# show usage
$ make run
Modric Usage: ./bin/modric OPTIONS
  -alvarez       - no params, runs demo code
  -e2j file.edn  - convert edn to json and pprint it
  -ppj file.json - pprint json
  -db path-to-db - do something against rocks db at path
  -key           - key for rocks db operation (set, get, list)
  -value         - value to set at key
  -count         - num values to return, starting at key


# pprint json
$ ./bin/modric -ppj colors.json

# convert edn to json and pprint as json
$ ./bin/modric -e2j colors.edn

# Using rocks db

# make sure db is clean
$ rm -rf .data

# insert some data
$ ./bin/modric -db .data -key Valheim -value "Is the best game I've ever played!"
$ ./bin/modric -db .data -key Brian -value '{:name "Brian" :skill-level -1}'
$ ./bin/modric -db .data -key "Better than Brian" -value Everyone
$ ./bin/modric -db .data -key 1 -value one

# get a single value out
$ ./bin/modric -db .data -key Brian
{:name "Brian" :skill-level -1}

# get 2 values out, starting at Brian
$ ./bin/modric -db .data -key Brian -count 2
Brian = {:name "Brian" :skill-level -1}
Valheim = Is the best game I've ever played!

# get 5 values out, starting at 1 (only 4 total values in db)
$ ./bin/modric -db .data -key 1 -count 5
1 = one
Better than Brian = Everyone
Brian = {:name "Brian" :skill-level -1}
Valheim = Is the best game I've ever played!

```

### cJSON

This code started out as a cut-and-paste of cJSON (and is still, mostly, cJSON).
Props to the creator for all the great code.

https://github.com/DaveGamble/cJSON

### EDN

https://github.com/edn-format/edn


### RocksDB

https://rocksdb.org
