#!/bin/sh
 
# fetch changes, git stores them in FETCH_HEAD
git fetch
 
# check for remote changes in origin repository
newUpdatesAvailable=`git diff HEAD FETCH_HEAD`
if [ "$newUpdatesAvailable" != "" ]
then
	# reset to the most recent commit
        git reset --hard
	git clean -f
        git merge FETCH_HEAD
        echo "merged updates"
	# build the new source code
	./install.sh
else
        echo "no updates available"
fi
