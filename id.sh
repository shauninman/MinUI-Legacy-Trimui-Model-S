#!/bin/sh

REPO=$1
TARGET=$2

cd "$REPO"
REPO_NAME=$(basename $(git rev-parse --show-toplevel))
REPO_HASH=$(git show -s --format=%H | head -c 16)

cd -
echo "$REPO_NAME#$REPO_HASH" > "$TARGET/id.txt"