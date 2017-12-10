#!/bin/bash
counter=0
echo "Enter the folder you want to run on"
read fold
echo "Enter your output name"
read outp
for file in $fold/*.tsp
do
    #    echo "$counter," >> hi.txt
    ./concorde -x $file | grep -oP '(?<=Optimal Solution: ).*' >> $outp.txt
    let counter+=1
done
rm *.sol
rm *.pul
rm *.sav
rm *.res
