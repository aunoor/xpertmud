#!/bin/bash

echo "Generating build dependencies"
if [ ! -f /tmp/log-conf ]; then
    echo -n "Building strace config log... "
    strace -f -o /tmp/log-conf ./configure --prefix=/tmp/build-tmp
    echo "done."
fi

if [ ! -f /tmp/log-make ]; then
    echo -n "Building strace make log... "
    strace -f -o /tmp/log-make make
    echo "done."
fi

if [ ! -f /tmp/log-inst ]; then
    echo -n "Building strace inst log... "
    strace -f -o /tmp/log-inst make install
    echo "done."
fi

echo -n "Removing directories from list... "
DFILES=`grep open /tmp/log-*|perl -pe 's!.* open\(\"([^\"]*).*!$1!' |grep "^/"| sort | uniq| grep -v "^\(/tmp\|/dev\|/proc\)"`
FILES=""
echo $DFILES
for f in $DFILES; do
    if [ ! -d $f ]; then
	FILES="$FILES $f"
    fi
done
echo "done."

echo -n "Creating essential package list... "
# these are already done (e.g. included automatically)
DONEPACKAGES=""
for p in `grep-status -FEssential -sPackage -ni yes`; do
    DONEPACKAGES="$DONEPACKAGES $p"
done
DONEPACKAGES="$DONEPACKAGES libc6-dev libc-dev libc0.3-dev libc6.1-dev gcc g++ make dpkg-dev"
echo "done."

echo -n "Creating list of used packages... "
# these are candidates to be evaluated
TODOPACKAGES="`dpkg -S $FILES 2>/dev/null |cut -f1 -d\":\" | perl -pe 's/,//g; s/ /\n/g' |sort |uniq`"
echo "done."

echo -n "Adding dependencies to nodeps list..."
# all dependencies of (build-)essential and
# dependencies are also done (at least afterwards...)
TXX=""
ALLP="$DONEPACKAGES $TODOPACKAGES"
for p in $ALLP; do
    TXX="$TXX `grep-status -FPackage -sDepends $p |perl -pe 's/\((>|<|=)(>|<)?[^\)]*\)//g; s/,//g; s/Depends: //' | \
	while read l; do \
	    for p2 in $l; do \
                if [ \"$p2\" != \"$p\" ]; then \
		    echo \"$p2\"; \
                fi \
	    done; \
        done`"
done
echo $TXX
DONEPACKAGES="$DONEPACKAGES $TXX"
DONEPACKAGES="`echo -e \"$DONEPACKAGES\" |perl -pe's/ /\\n/g;' |sort |uniq`"
echo "done."

#echo -e "$TODOPACKAGES"
echo "Writing dependencies to ./build-deps... "
for x in $TODOPACKAGES; do
    if [ "`echo \"$DONEPACKAGES\" |grep $x`" = "" ]; then
	echo -n "$x (>=" `dpkg -s $x|grep ^Version|cut -f2 -d":"` "), "
    fi
done |tee ./build-deps
echo "Finished."
    
