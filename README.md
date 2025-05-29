# Lettuce server

## Running lettuce server
`make`
- This will compile the application into the `/build/` directory from the project root and the executable in the root called `lettuce-server`
- Run the server with `./lettuce-server` from the root
- This will start the server at port 6379 (this can be overridden through the first argument) e.g `./lettuce-server 1234` will start the server on port 1234

## Running lettuce server tests
`make test`
- The catch2 headerfile can be found in the `external` directory

## Lettuce server commands
- *PING* - `ping` - Responds with `+PONG`
- *ECHO* - `echo Hello!` - Returns arguments: Responds with `+Hello!`
- *FLUSHALL* - `flushall` - Clears the db cache and returns `+OK`