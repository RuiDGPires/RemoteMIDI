#!/bin/bash

script_dir=$(dirname "$0")

SERVER=$script_dir/../build/server
BRIDGE=a2jmidid

if [[ ! -f "$SERVER" ]]; then
    echo "File $filename missing"
    echo "Server should be compiled before running this script"
    exit
fi

echo Opening $BRIDGE
# Open P1 in the background
$BRIDGE &
P1_PID=$!

# Open P2 directly
echo Opening server...
$SERVER

# Close P1
kill $P1_PID
