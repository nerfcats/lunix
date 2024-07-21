#!/bin/bash

cd lunix-bl/build

if [ -e "lunix-bl" ]; then
  ./lunix-bl -b # Run the bootloader with the -b arg to run the post-build script
else
  echo "The bootloader doesn't exist. Have you built Lunix properly? Were there any build errors?"
fi

