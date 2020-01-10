#!/bin/bash

git fetch --all
if [ $# -eq 0 ]
then
	HASH=$(git ls-remote hydrogenvr -h refs/heads/master | cut -f1)
	git pull --squash -X theirs hydrogenvr master --allow-unrelated-histories
else
	HASH=$1
	CURRENT_BRANCH=$(git branch | grep \* | cut -d ' ' -f2)
	git checkout --orphan update_hydrogenvr
	git reset --hard
	git pull hydrogenvr master
	git reset --hard $HASH
	git checkout $CURRENT_BRANCH
	git merge --squash -X theirs update_hydrogenvr --allow-unrelated-histories
	git branch -D update_hydrogenvr
fi
git commit -m "Update HydrogenVR to $HASH" -e

if [[ $(grep PROJECT build.conf.example | wc -l) -ne $(grep PROJECT build.conf | wc -l) ]]
then
	echo "WARNING:"
	echo "Your build.conf seems outdated, please compare it with build.conf.example."
fi
