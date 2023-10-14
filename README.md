# service-cats-example

## Table of Contents

- [Introduction](#introduction)
- [Installation](#installation)
- [How to Run](#how-to-run)
- [How to Test](#how-to-test)

# Introduction

This is an example of a microservice written in C, using the [c-framework-service](https://github.com/danidomi/c-framework-service) framework 
and the dependency manager [cdeps](https://github.com/danidomi/cdeps).

## Installation

Before getting started, make sure you have [cdeps](https://github.com/danidomi/cdeps) installed. 
You can follow the installation guide on their repository:

```shell
cdeps install
```

## How to run

To run the microservice, you need a running database. If you don't have one, the easiest way to set it up is using Docker. You can bring up the database with the following command:

```shell
docker-compose up -d db
```

Once the database is up and running, you can compile and run the microservice:

```shell
./run.sh
```

## How to test

To test the microservice, you can use curl or any other HTTP client.
For example, you can make a request to the microservice using the following curl command:

```shell
curl http://localhost:8080/cats\?id\=1
```
