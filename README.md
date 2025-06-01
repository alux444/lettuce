# Lettuce server

A lightweight, Redis-like in-memory key-value store with support for strings, lists, and hashes.  
Implements a subset of the Redis protocol (RESP) and supports basic persistence.

---c

## Building

`make`

- This will compile the application into the `/build/` directory from the project root and the executable in the root called `lettuce-server`.

---

## Running the server

- Run the server with `./lettuce-server` from the root.
- This will start the server at port 6379 (this can be overridden through the first argument) e.g. `./lettuce-server 1234` will start the server on port 1234.

---

## Running lettuce server tests

`make test`

- The Catch2 header file can be found in the `external` directory.

---

## Lettuce server commands

| Command  | Example (RESP)                                     | Description                                 |
| -------- | -------------------------------------------------- | ------------------------------------------- |
| PING     | `*1\r\n$4\r\nPING\r\n`                             | Responds with `+PONG`                       |
| ECHO     | `*2\r\n$4\r\nECHO\r\n$5\r\nHello\r\n`              | Responds with `+Hello!`                     |
| FLUSHALL | `*1\r\n$8\r\nFLUSHALL\r\n`                         | Clears the db cache and returns `+OK`       |
| SET      | `*3\r\n$3\r\nSET\r\n$3\r\nfoo\r\n$3\r\nbar\r\n`    | Sets key to value, returns `+OK`            |
| GET      | `*2\r\n$3\r\nGET\r\n$3\r\nfoo\r\n`                 | Gets value for key, returns bulk string     |
| DEL      | `*2\r\n$3\r\nDEL\r\n$3\r\nfoo\r\n`                 | Deletes key, returns `:1` if deleted        |
| EXPIRE   | `*3\r\n$6\r\nEXPIRE\r\n$3\r\nfoo\r\n$2\r\n10\r\n`  | Sets key to expire in N seconds             |
| RENAME   | `*3\r\n$6\r\nRENAME\r\n$3\r\nfoo\r\n$3\r\nbar\r\n` | Renames key                                 |
| KEYS     | `*1\r\n$4\r\nKEYS\r\n`                             | Lists all keys                              |
| TYPE     | `*2\r\n$4\r\nTYPE\r\n$3\r\nfoo\r\n`                | Returns type of key (`string`, `list`, etc) |

### List Commands

| Command | Example (RESP)                                               | Description                  |
| ------- | ------------------------------------------------------------ | ---------------------------- |
| LLEN    | `*2\r\n$4\r\nLLEN\r\n$5\r\nmylist\r\n`                       | Returns length of list       |
| LPUSH   | `*3\r\n$5\r\nLPUSH\r\n$5\r\nmylist\r\n$1\r\na\r\n`           | Pushes value to head of list |
| RPUSH   | `*3\r\n$5\r\nRPUSH\r\n$5\r\nmylist\r\n$1\r\nb\r\n`           | Pushes value to tail of list |
| LPOP    | `*2\r\n$4\r\nLPOP\r\n$5\r\nmylist\r\n`                       | Pops value from head         |
| RPOP    | `*2\r\n$4\r\nRPOP\r\n$5\r\nmylist\r\n`                       | Pops value from tail         |
| LREM    | `*4\r\n$4\r\nLREM\r\n$5\r\nmylist\r\n$1\r\n0\r\n$1\r\na\r\n` | Removes occurrences of value |
| LINDEX  | `*3\r\n$6\r\nLINDEX\r\n$5\r\nmylist\r\n$1\r\n0\r\n`          | Gets element at index        |
| LSET    | `*4\r\n$4\r\nLSET\r\n$5\r\nmylist\r\n$1\r\n0\r\n$1\r\nz\r\n` | Sets element at index        |

### Hash Commands

| Command | Example (RESP)                                                                          | Description                |
| ------- | --------------------------------------------------------------------------------------- | -------------------------- |
| HSET    | `*4\r\n$4\r\nHSET\r\n$6\r\nmyhash\r\n$5\r\nfield\r\n$5\r\nvalue\r\n`                    | Sets field in hash         |
| HGET    | `*3\r\n$4\r\nHGET\r\n$6\r\nmyhash\r\n$5\r\nfield\r\n`                                   | Gets field value           |
| HEXISTS | `*3\r\n$7\r\nHEXISTS\r\n$6\r\nmyhash\r\n$5\r\nfield\r\n`                                | Checks if field exists     |
| HDEL    | `*3\r\n$4\r\nHDEL\r\n$6\r\nmyhash\r\n$5\r\nfield\r\n`                                   | Deletes field              |
| HGETALL | `*2\r\n$7\r\nHGETALL\r\n$6\r\nmyhash\r\n`                                               | Gets all fields and values |
| HKEYS   | `*2\r\n$5\r\nHKEYS\r\n$6\r\nmyhash\r\n`                                                 | Gets all field names       |
| HVALS   | `*2\r\n$5\r\nHVALS\r\n$6\r\nmyhash\r\n`                                                 | Gets all field values      |
| HLEN    | `*2\r\n$4\r\nHLEN\r\n$6\r\nmyhash\r\n`                                                  | Gets number of fields      |
| HMSET   | `*6\r\n$5\r\nHMSET\r\n$6\r\nmyhash\r\n$2\r\nf1\r\n$2\r\nv1\r\n$2\r\nf2\r\n$2\r\nv2\r\n` | Sets multiple fields       |

---

## Example Usage

You can use `redis-cli` to interact with the server:

```sh
redis-cli -p 6379 ping
redis-cli -p 6379 set foo bar
redis-cli -p 6379 get foo
```
