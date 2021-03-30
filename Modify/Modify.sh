#!/usr/bin/bash

recursive=false
uppercase=false
lowercase=false
sedpattern=false
helpme=false
count=0 		#for counting number of parameters starting with "-"

#reading passed parameters and setting appropriate flags
for param in $@ ; do 
	case $param in
		"-h") helpme=true ;;
		"-r") recursive=true ; ((count++)) ;;
		"-rl") recursive=true ; lowercase=true ; ((count++)) ;;
		"-ru") recursive=true ; uppercase=true ; ((count++)) ;;
		"-l") lowercase=true ; ((count++)) ;;
		"-u") uppercase=true ; ((count++)) 
	esac
done

displayhelp() {
	cat <<-_EOF

		Usage:	modify [-r] [-l|-u] <dir/file names...>  
		   or:	modify [-r] <sed pattern> <dir/file names...>  
		   or:	modify [-h]    
		Modify names of the files

		Options:
		-r  	rename files recurscively
		-u 		change all lowercase letters into uppercase letters
		-l      change all lowercase letters into uppercase letters
		-h 		display help

	_EOF

}


#error if both -l and -u are passed
if test $uppercase = true && test $lowercase = true; then
	echo "wrong parameters"
	exit

#ifeither -u nor -l are present - raise sedpattern flag	
elif test $uppercase = false && test $lowercase = false ; then
	sedpattern=true
fi

#if helpme flag is raised or no parameters are passed - display help
if $helpme || [ "$#" = 0 ]; then
	displayhelp
	exit
fi

shift $count #moving to the first parameter without "-"

#non-recursive sedpattern
if test $sedpattern = true && test $recursive = false ; then
	sedparam="$1"	#read the first non"-" argument as the pattern for sed
	shift			#move to the next argument
	
	#loop for performing operations on each file passed in parameters
	for param in $@; do
		#if the provided file exists and is a file then perform operations		
		if test -e "$param" && test -f "$param"; then
			file="$(basename -- "$param")" 	#reading the file name
			path="$(dirname -- "$param")"	#reading the path to the file
			name="${file%.*}"		#extracting the name w/o extension
			ext="${file##*.}"		#extracting sole extension
			
			#enter if the provided file has no extension
			if test $name = $file; then
				mv "$param" "$path/$(printf -- "$name" | sed "$sedparam")" 2>/dev/null #error message send to null device so it's not displayed
				
			else	
				mv "$param" "$path/$(printf -- "$name" | sed "$sedparam").$ext" 2>/dev/null #change name without changing the extension
			fi
		else
			#display error and help if the file is incorrect
			echo "File is incorrect"
			displayhelp
		fi
	done	
	exit	

#recursive sedpattern
elif test $sedpattern = true && test $recursive = true; then
	sedparam="$1"
	shift
	
	for param in $@ ; do
		#enter if the file exists and is a directory
		if test -e "$param" && test -d "$param"; then

			#use find to parse through files in the specified folder
			for filepath in $(find "$param" 2>/dev/null ) ; do
				#perform operations only on files (not changing folders' names)
				if  test -f "$filepath"; then
					
					file="$(basename -- "$filepath")"
					path="$(dirname -- "$filepath")"
					name="${file%.*}"
					ext="${file##*.}"
					
					if test $name = $file; then
						mv "$filepath" "$path/$(printf -- "$name" | sed "$sedparam")" 2>/dev/null
					else
						mv "$filepath" "$path/$(printf -- "$name" | sed "$sedparam").$ext" 2>/dev/null
					fi
				fi
			done
		else
			echo "File is incorrect"
			displayhelp
		fi
	done
	exit

#non recursive uppercasing or lowercasing
elif test $sedpattern = false && test $recursive = false ; then
	
	for param in $@ ; do
		
		if test -e "$param" && test -f "$param"; then
			file="$(basename -- "$param")"
			path="$(dirname -- "$param")"
			name="${file%.*}"
			ext="${file##*.}"
			
			if test $name = $file; then
				if test $uppercase = true; then
					mv -- "$param" "$path/${file^^}" 2>/dev/null
					
				elif test $lowercase = true; then
					mv -- "$param" "$path/${file,,}" 2>/dev/null
				fi
				
			else	
				if test $uppercase = true; then
					mv -- "$param" "$path/${name^^}.${ext}" 2>/dev/null

				elif test $lowercase = true; then
					mv -- "$param" "$path/${name,,}.${ext}" 2>/dev/null

				fi
			fi
		else
			echo "File is incorrect"
			displayhelp
		fi	
	done
	exit

#recursive uppercasing and lowercasing
elif test $recursive = true && test $sedpattern = false ; then
	for param in $@ ; do
		
		if test -e "$param" && test -d "$param"; then

			for filepath in $(find "$param" 2>/dev/null); do
					if test -f "$filepath" ; then
					
					file="$(basename -- "$filepath")"
					path="$(dirname -- "$filepath")"
					
					name="${file%.*}"
					ext="${file##*.}"
					
					if test $name = $file; then
						if test $uppercase = true ; then
							mv -- "$filepath" "$path/${file^^}"
						
						elif test $lowercase = true; then
							mv -- "$filepath" "$path/${file,,}"
						fi
					else
						if test $uppercase = true ; then
							mv -- "$filepath" "$path/${name^^}.${ext}"
						
						elif test $lowercase = true; then
							mv -- "$filepath" "$path/${name,,}.${ext}"
						fi
					fi
				fi
			done
		else
			echo "Provided element is incorrect"
			displayhelp
		fi
	done
	exit
fi
