//EOPSY - task 6
//Laura Ploch, 300176
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>     //getopt(), lseek()
#include <stdlib.h>     //exit()
#include <stdbool.h>
#include <string.h>     //strcmp

//program to copy files
//syntax:
//copy [-m] <file_name> <new_file_name>
//copy [-h]
//
//options:
//-m        use mmap() and memcpy() to copy the file
//otherwise use read() and write() to copy file contents
//-h        help information

void help();
void copy_using_readwrite(int fd_source, int fd_dest);
void copy_using_mmap(int fd_source, int fd_dest);

int main(int argc, char *argv[])
{
    if(argc == 1)
    {
        help();
        exit(0);
    }

    int argument;
    bool m_flag = false;    //set to true if -m parameter detected

    //loop for reading the parameters
    while((argument = getopt(argc, argv, "hm:")) != -1)
    {
        if(argument == 'm')
        {
            m_flag = true;
            break;
        }
        else if(argument == 'h')
        {
            help();
            exit(0);
        }
        else
        {
            perror("Wrong parameter\n");
            help();
            exit(1);
        }
    }

    if((m_flag == false && argc != 3) || (m_flag == true && argc != 4))
    {
        perror("Wrong number of parameters\n");
        help();
        exit(1);
    }

    static const char* source_file = NULL;
    static const char* destination_file = NULL;
    int fd_source, fd_dest;                     //file descriptors

    if(!m_flag)
    {
        //assign files
        source_file = argv[1];
        destination_file = argv[2];

        //open files
        fd_source = open(source_file, O_RDONLY);
        if(fd_source == -1)
        {
            perror("Error during opening of the source file\n");
            exit(1);
        }

        //open destination file in read/write mode,
        //if doesn't exist - create file with r/w permissions for each class
        fd_dest = open(destination_file, O_RDWR | O_CREAT, 0666);
        if(fd_dest == -1)
        {
            perror("Error during opening of the destination file \n");
            exit(1);
        }

        copy_using_readwrite(fd_source, fd_dest);
    }

    else
    {
        //assign files
        for(int i = 1; i < argc; i++)
        {

            //if the argument is not -m
            if(strcmp(argv[i], "-m") != 0)
            {
                if(source_file == NULL)
                    source_file = argv[i];
                else
                    destination_file = argv[i];
            }
        }

        //open files
        fd_source = open(source_file, O_RDONLY);
        if(fd_source == -1)
        {
            perror("Error during opening of the source file\n");
            exit(1);
        }

        //open destination file in read/write mode,
        //if doesn't exist - create file with r/w permissions for each class
        fd_dest = open(destination_file, O_RDWR | O_CREAT, 0666);
        if(fd_dest == -1)
        {
            perror("Error during opening of the destination file \n");
            exit(1);
        }

        copy_using_mmap(fd_source, fd_dest);
    }


    if(close(fd_source) == -1)
    {
        perror("Error during closing of the source file\n");
        exit(1);
    }

    if(close(fd_dest) == -1)
    {
        perror("Error during closing of the destination file\n");
        exit(1);
    }

    exit(0);
}

void help()
{
    printf("Usage:  copy [-m] <source file> <destination file>\n"
            "or:     copy <source file> <destination file>\n"
            "or:     copy [-h]\n"
            "Copy content of one file to another\n"
            "\n"
            "Options:\n"
            "-m      copy files with the use of mmap() and memcpy() functions\n"
            "        otherwise copy with the use of read() and write() functions\n"
            "-h      display help\n");
}

void copy_using_readwrite(int fd_source, int fd_dest)
{
    struct stat stat_source;

    //load information about the source file
    if(fstat(fd_source, &stat_source) == -1)
    {
        perror("Error during loading of the source file\n");
        exit(1);
    }

    int file_size = stat_source.st_size;
    char buf[file_size];
    int temp_read;              

    while((temp_read = read(fd_source, buf, file_size)) > 0)
    {
        if(write(fd_dest, buf, file_size) < 0)
        {
            perror("Error during writing to the file\n");
            exit(1);
        }
    }
}

void copy_using_mmap(int fd_source, int fd_dest)
{
    struct stat stat_source;

    //load information about the source file
    if(fstat(fd_source, &stat_source) == -1)
    {
        perror("Error during loading of the source file\n");
        exit(1);
    }

    int file_size = stat_source.st_size;
    char *buf_source;   //pointer to the source
    char *buf_dest;     //pointer to the destination

    //create a new mapping for the source file
    buf_source = mmap(NULL, file_size, PROT_READ, MAP_SHARED, fd_source, 0);

    //check if the area was mapped succesfully (ret value not MAP_FAILED)
    if(buf_source == (void*) -1)
    {
        perror("Error during mapping of the source file\n");
        exit(1);
    }

    //create a new mapping for the destination file
    buf_dest = mmap(NULL, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_dest, 0);

    //check if the area was mapped succesfully (ret value not MAP_FAILED)
    if(buf_source == (void*) -1)
    {
        perror("Error during mapping of the destination file\n");
        exit(1);
    }

    //resize the destination file
    if(ftruncate(fd_dest, file_size) == -1)
    {
        perror("Error during resizing of the destination file\n");
        exit(1);
    }

    //printf("%p\n", (void *) buf_dest);
    //copy block of memory from source memory area to destination memory area4
    memcpy(buf_dest, buf_source, file_size); //returns a pointer to the destination
    //printf("%p\n", (void *) buf_dest);

    //check for errors during copying of the memory block
    if(buf_dest == (void *) -1)
    {
        perror("Error during copying of the source memory  block to the destination\n");
        exit(1);
    }

    //remove mappings for pages containing any part of the address space of source and destination
    if(munmap(buf_source, file_size) == -1 || munmap(buf_dest, file_size) == -1)
    {
        perror("Error during removal of the mapping\n");
        exit(1);
    }
}