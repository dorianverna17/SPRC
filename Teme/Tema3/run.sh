#!/bin/bash

docker stack rm sprc3
docker stack deploy -c stack.yml sprc3