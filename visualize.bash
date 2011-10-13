#!/bin/bash

set -e


if [ $# -ne 1 ]; then
    echo "Usage: $0 dat_dir"
    echo "where dat_dir is a dir with generated dat files"
    exit 1
fi

readonly dat_directory="$1"
readonly script="/tmp/$(basename $0).$$.tmp"
readonly joined_script=$script.joined

touch $script
echo "plot " >> $script
for path in $dat_directory/*.dat; do
    echo "\"$path\" using 1:2 smooth csplines with lines|" >> $script
done

sed -e :a -e '$!N; s/\n//; ta' -i "*.backup" $script
sed -e 's/|/,/g' -i "*.backup" $script
sed -e 's/,$//g' -i "*.backup" $script

echo "set xlabel \"input size\"" >> $joined_script
echo "set ylabel \"time (sec)\"" >> $joined_script
cat $script >> $joined_script

gnuplot -p $joined_script
