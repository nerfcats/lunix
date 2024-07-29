#!/bin/bash

# Get the current date and time
build_date=$(date +"%Y-%m-%d %H:%M:%S")

touch ../lunix-bl/build/.builddate
# Write the build date to .builddate file
echo "$build_date" > ../lunix-bl/build/.builddate