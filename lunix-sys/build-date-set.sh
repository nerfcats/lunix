#!/bin/bash

# Get the current date and time
build_date=$(date +"%Y-%m-%d %H:%M:%S")

# Write the build date to .builddate file
echo "$build_date" > ../lunix-bl/build/.builddate