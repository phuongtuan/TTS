#!/bin/sh
 
# check for internet connection
wget -q --tries=10 --timeout=20 --spider http://google.com
while [ $? -ne 0 ]
do
wget -q --tries=10 --timeout=20 --spider http://google.com
echo "No internet connection"
done

# fetch changes, git stores them in FETCH_HEAD
echo "Fetch data from remote repo"
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
