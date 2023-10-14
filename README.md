MVC
===

# Introduction

This is an e.g. of a Microservice written in C, using the framework [c-framework-service](https://github.com/danidomi/c-framework-service) and the dependency manager [cdeps](https://github.com/danidomi/cdeps)


## Installation

If you don't have yet the tool please follow the guide from [cdeps](https://github.com/danidomi/cdeps) how to install it

```shell
cdeps install
```

## How to run

first we need to have a running database, if you don't have one the easiest way would be with docker

```shell
docker-compose up -d db
```

once the database is up, let's compile and run our `microservice`

```shell
./run.sh
```

## How to test

```shell
curl http://localhost:8080/cats\?id\=1
```


Author: Daniel Domingues
