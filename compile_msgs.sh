#!/bin/bash
# Build python classes for parsing protobuf messages

echo "Starting the protoc compiler!"
protoc -I=msg --cpp_out=msg/cpp msg/*.proto
