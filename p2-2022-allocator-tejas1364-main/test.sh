#!/usr/bin/bash


i=2
j=3

./proj2 1 sample_input/input_${i}
./proj2 1 sample_input/input_${j}

ourOutput="./output/result-1-input_2"
sampleOutput="./sample_output/result-1-input_2"

str1=$(cat "$ourOutput") 
str2=$(cat "$sampleOutput") 

if [ "$str1" != "$str2" ]; then 
    echo "Output Not Matched" 
else
    echo "Output Matched yoo"  
fi;


ourOutput="./output/result-1-input_3"
sampleOutput="./sample_output/result-1-input_3"

str3=$(cat "$ourOutput") 
str4=$(cat "$sampleOutput") 

if [ "$str3" != "$str4" ]; then 
    echo "Output Not Matched" 
else
    echo "Output Matched yoo"  
fi;
