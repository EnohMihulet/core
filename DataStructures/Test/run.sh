#!/usr/bin/env bash
set -e

# compile
file="$1"
output="${file%.*}"

clang "$file" -o "$output" -I test.h -I ../dynarray.h -I ../queue.h -I ../ringbuffer.h -I ../stack.h -I ../priorityqueue.h

if  [ $? -ne 0 ]; then
	echo "Compilation failed"
	exit 1
fi
	
# run
./"$output" && rm "$output"
