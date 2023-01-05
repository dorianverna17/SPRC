#!/bin/bash

docker build -t adapter .
docker stack deploy -c stack.yml sprc3
