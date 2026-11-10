/*
1. -l flag for long format
2. -h flag for human format
3. -a flag for all files (hidden too)
4. Ability to combine them like -alh
5. usage: ls [FLAGS] [PATH]
6. -h for help
*/

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>

#define PATH_SIZE 256

int main(int argc, char *argv[]) {
	char path[PATH_SIZE] = "."; // default is current directory 
	
	// support for user given path
	if (argc == 2) {
		strcpy(path, argv[1]);
	}

	// open the current directory
	DIR* dir = opendir(path);	
	if (!dir) {
		perror("opendir");
		return EXIT_FAILURE;
	}

	// read the opened directory
	struct dirent *dir_content;
	errno = 0;
	while (dir_content = readdir(dir)) {
		printf("%s\n", dir_content->d_name);	
	}

	switch (errno) {
		case EOVERFLOW:
			perror("printf");
			break;
		case EBADF:
			perror("readdir");
	}

	free(d);
	if (closedir(dir) < 0) {
		perror("closedir");
	}
}
