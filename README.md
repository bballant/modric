## Modric

Nothing to "C" here. Get it?

All this microcontroller business made me want to sharpen my C skills and I was watching the World Cup.

This repo is for noodling in C for practice and fun. I have a loose idea to build
a little edn/datalog/triple-store query engine, like a cross between xtdb and jq.
But for now it is mostly a cut-and-paste of cJSON, with tweaks to parse edn files
and pretty print json in a way more to my liking.

```
# depends on gcc and make

# build
make

# run the "demo"
make run

# pprint json
./bin/modric -ppj colors.json

# convert edn to json and pprint as json
./bin/modric -e2j colors.edn

```

### cJSON

This code started out as a cut-and-paste of cJSON (and is still, mostly, cJSON).
Props to the creator for all the great code.

https://github.com/DaveGamble/cJSON

### EDN

https://github.com/edn-format/edn


### Ideas and TODOs

Immediate

* Parse EDN keyword values
* Support of EDN #inst and #uuid
* pprint EDN files
* Flat printing of both formats
* updates to CLI to support combinations of above

Long Term

* Persisting EDN files in a triple-store
  * Look into RocksDB which backs XTDB
* Querying triple store with datalog
* An API like this

```
# if db dir doesn't exist, create db from file
modric insert -db ./data/ -file colors.edn
modric insert -db ./data/ -edn "{:color "orange" ...}"

# query w datalog (not sure of datalog syntax)
modric query -db ./data/ -file my-datalog-query.edn 
modric query -db ./data/ -edn "{:find [?typ] :in [?c :color "red"][?c :type ?typ]}" 
 
# all commands could use an environment variable for db
export M_DB=./data/
modric delete -edn "{:find  ...}"
```
