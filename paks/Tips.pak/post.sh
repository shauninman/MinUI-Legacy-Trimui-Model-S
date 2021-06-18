#!/bin/sh
# Tips.pak/post.sh

DIR=$(dirname "$0")
cd "$DIR"

# remove renamed page
if [ -f ./pages/06-updates.png ]; then
	rm -f "./pages/06-updates.png"
fi