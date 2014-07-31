
#!/bin/bash          
echo "Deleting old output files"
find . -name "*_*_*" -type d -print0|xargs -0 rm -r --
echo "This script is automating the running process"
# for c files
gcc  -c -o test.c.o test.c 
 # for cpp files        
g++ -c -o tables.cpp.o tables.cpp 
# creating executable
g++ -o exe tables.cpp.o test.c.o
 #  showing the op   
./exe   

# Comand to create output folder and to move all the intermediate files into that folder
now=$(date +"%m_%d_%Y")
mkdir $now &&
mv "$(pwd)"/*.txt *.o exe *.out $now                              
 
