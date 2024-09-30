#!/bin/bash

cd lunix-bl/build

if [ -e "lunix-bl" ]; then
  # To bypass hash checks run with --ignore-hash-err
  ./lunix-bl -b # Run the bootloader
else
  echo "The bootloader doesn't exist. Have you built Lunix properly? Were there any build errors?"
fi

