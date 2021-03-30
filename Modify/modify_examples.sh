#!/usr/bin/bash

modidir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
modify=$modidir/modify.sh

rm -r test_directory

mkdir test_directory
mkdir -p test_directory/folder test_directory/folder2
cd test_directory
touch file text.txt 
touch -- -file -n -n.txt -r ",.=+*!?special.txt"
touch folder/text.txt folder/file folder2/file2
cd ../
clear

echo -e "================test 1: Testing help=============================="
echo "Command: modify"
$modify
read -p "Press enter to continue"
clear


echo -e "================test 2: Testing help ============================="
echo "Command: modify -r -h text.txt"
$modify -r -h text.txt
read -p "Press enter to continue"
clear


echo -e "===========test 3: Uppercasing files (inside test_directory)  ========"
cd test_directory
ls -R 
echo -e "\nCommand: modify -u file ./folder/file ./folder/text.txt\n"
$modify -u file ./folder/file ./folder/text.txt
ls -R 
read -p "Press enter to continue"
clear


echo -e "===========test 4: Lowercasing files (inside test_directory) ========"
ls -R 
echo -e "\nCommand: modify -l nonexistent ./folder/TEXT.txt FILE ./folder/FILE\n"
$modify -l nonexistent ./folder/TEXT.txt FILE ./folder/FILE
ls -R 
read -p "Press enter to continue"
clear


echo -e "===========test 5: Testing sed pattern==========="
ls -R
echo -e "\nCommand: modify s/x/XXX/ ./folder/file text.txt\n"
$modify s/x/XXX/ ./folder/text.txt text.txt
ls -R
read -p "Press enter to continue"
clear


echo -e "===========test 6: Recursive uppercasing==========="
cd ../
ls -R
echo -e "\nCommand: modify -r -u test_directory"
$modify -r -u test_directory
ls -R
read -p "Press enter to continue"
clear


echo -e "===========test 7: Recursive sed pattern==========="
ls -R
echo -e "\nCommand: modify s/E/ee\-e/ test_directory"
$modify -r s/E/ee\-e/ test_directory
ls -R
read -p "Press enter to continue"
clear
