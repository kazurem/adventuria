/*
 * TODO
 * 1. Add support for flags (-h, -l, -alhR etc)
 * 2. Support human mode (-h)
 * 3. Support recursive listing (-R)
 * 4. Support help (-h)
 * 5. Support hidden files (-a)
 * 6. Sort alphabetically by default
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h> 

// for getpwuid() (convert uid to user name)
#include <pwd.h>
// for getgrgid() (convert gid to group name)
#include <grp.h>


#define PATH_SIZE 4096
#define TIME_STR_SIZE 256
#define TIME_FORMAT "%b %d %H:%M"

int append_fslash(char str[], size_t size) {
	size_t len = strlen(str);
	
	// if string buffer is full
	if (len == (size - 1)) {
		return -1;
	}

	// check if / already exists at the end. If not, add it
	if (str[len - 1] != '/') {
		str[len] = '/';
		str[len + 1] = '\0';
	}
	return 0;
}

// @brief get linux file type from stat structure
// @note see stat(3type) and inode(7)
char get_filetype(struct stat *sb) {
	switch (sb->st_mode & S_IFMT) {
		case S_IFBLK: return 'b';
		case S_IFIFO: return 'p';
		case S_IFREG: return '-';
		case S_IFDIR: return 'd';
		case S_IFCHR: return 'c';
		case S_IFLNK: return 'l';
		case S_IFSOCK: return 's';
		default: return 'u';
	}
}

// @brief convert file permission part of `st_mode` from `struct stat` to a string representation 
// @note see stat(3type) and inode(7) 
void convert_num_perm_to_str(uintmax_t st_mode, char buf[10]) {
	/*
	 * st_mode & 00777 represents the file permission bits for user, group and others
	 *
	 * S_IRWXU - 00700 - rwx for user
	 * S_IRUSR - 00400 - r   for user
	 * S_IWUSR - 00200 - w   for user
	 * S_IXUSR - 00100 - x   for user
	 *
	 * S_IRWXG - 00070 - rwx for group
	 * S_IRGRP - 00040 - r   for group
	 * S_IWGRP - 00020 - w   for group
	 * S_IXGRP - 00010 - x   for group
	 *
	 * S_IRWXO - 00007 - rwx for others
	 * S_IROTH - 00004 - r   for others
	 * S_IWOTH - 00002 - w   for others
	 * S_IXOTH - 00001 - x   for others
	*/

	// user
	buf[0] = (st_mode & S_IRUSR) ? 'r' : '-';
	buf[1] = (st_mode & S_IWUSR) ? 'w' : '-';
	buf[2] = (st_mode & S_IXUSR) ? 'x' : '-';

	// group
	buf[3] = (st_mode & S_IRGRP) ? 'r' : '-';
	buf[4] = (st_mode & S_IWGRP) ? 'w' : '-';
	buf[5] = (st_mode & S_IXGRP) ? 'x' : '-';
	
	// others
	buf[6] = (st_mode & S_IROTH) ? 'r' : '-';
	buf[7] = (st_mode & S_IWOTH) ? 'w' : '-';
	buf[8] = (st_mode & S_IXOTH) ? 'x' : '-';
	
	buf[9] = '\0';
}

int main(int argc, char *argv[]) {
	char path[PATH_SIZE] = "."; // default is current directory 
	
	// support for user given path
	if (argc == 2) {
		// check if user given path's length' is greater than supported length 
		if ((strlen(argv[1]) + 1) > PATH_SIZE) {
			fprintf(stderr, "path too big: %s\n", argv[1]);
			exit(EXIT_FAILURE);
		}
		strcpy(path, argv[1]);
	}

	struct stat sb;
	if (stat(path, &sb) < 0) {
		perror("stat");
		exit(EXIT_FAILURE);
	}

	// if given path is not a directory, simply print the file and exit
	if ((sb.st_mode & S_IFMT) != S_IFDIR) {
		printf("%s\n", path);
		exit(EXIT_SUCCESS);
	}

	// open the current directory
	DIR* dir = opendir(path);	
	if (!dir) {
		perror("opendir");
		exit(EXIT_FAILURE);
	}

	if (append_fslash(path, PATH_SIZE) < 0) {
		fprintf(stderr, "path too big: %s\n", path);
		exit(EXIT_FAILURE);
	}

	struct tm *tms;
	char time_str[TIME_STR_SIZE];
	struct dirent *dir_content;
	char perm[10];
	char tmp_path[PATH_SIZE];
	
	// read the opened directory
	while ((dir_content = readdir(dir))) {
		strcpy(tmp_path, path);
		
		// check if strcat will be successful 
		if ((strlen(tmp_path) + strlen(dir_content->d_name + 1)) > PATH_SIZE) {
			fprintf(stderr, "path too big\n");
			exit(EXIT_FAILURE);
		}
		strcat(tmp_path, dir_content->d_name);
		
		if (stat(tmp_path, &sb) < 0) {
			perror("stat");
			exit(EXIT_FAILURE);
		}

		// turn numerical file permissions to string
		convert_num_perm_to_str((uintmax_t) sb.st_mode, perm);

		// last modification time
		tms = localtime(&sb.st_mtime);

		// note that strftime returning 0 does not necessarily mean an error
		if (strftime(time_str, sizeof(time_str), TIME_FORMAT, tms) == 0) {
			fprintf(stderr, "warn: strftime returned 0\n");
			exit(EXIT_FAILURE);
		}

		printf("%c%s %3ju %5s %5s %8jd %s %s\n", 
			get_filetype(&sb), 	      // file type 
			perm,       		      // file permissions
			(uintmax_t) sb.st_nlink,      // number of links
			getpwuid(sb.st_uid)->pw_name, // user name
			getgrgid(sb.st_gid)->gr_name, // group name
			(intmax_t) sb.st_size,	      // file size
			time_str,		      // last modification time
			dir_content->d_name 	      // file name
		);
	}

	if (closedir(dir) < 0) {
		perror("closedir");
	}
}
