## Modric

All this microcontroller business made me want to sharpen my C skills, and I was watching the World Cup, inspired by Luka Modric (among many others!).

```
# depends on gcc and make

# build
make

# run the demo
make run

# pprint json
./bin/modric -ppj colors.json

# convert edn to json and pprint as json
./bin/modric -e2j colors.json

```

### EDN

EDN is the native map serialization format for data structures in Clojure, similar to JSON,
but with more features.

https://github.com/edn-format/edn

### cJSON

cJSON is a relatively simple JSON parser written in C.

Almost all of this code was taken from cJSON and cJSON.h and cJSON.c are in this repo.

https://github.com/DaveGamble/cJSON

### Modric

So far Modric is mostly a cut-and-paste of cJSON, with tweaks to parse edn files
and pretty print json in a way more to my liking. The main goal is to maybe brush
up on C while doing something marginally useful.


### Ideas and TODOs

Immediate stuff

* Parse EDN keyword values
* Support of EDN #inst and #uuid
* pprint EDN files
* Flat printing of both formats
* updates to CLI to support combinations of above

Maybe in the year 2000 

* Querying EDN files w/ Datalog - Basically I think it would be cool to have
something that is like jq for EDN files and the query syntax is Datalog. This is
where I'd like to end up with all this.







