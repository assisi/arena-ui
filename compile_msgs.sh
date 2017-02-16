#!/bin/bash
# Build python classes for parsing protobuf messages

echo "Starting the protoc compiler!"
if [ ! -d "src/msg" ]; then
  mkdir src/msg
fi
protoc -I=msg --cpp_out=src/msg msg/*.proto
