/*
 * TODO
 * 1. Support human mode (-h)
 * 2. Support recursive listing (-R)
 * 3. Support help (-h)
 * 4. Sort alphabetically by default
 * 5. Deal with padding
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
#define NUM_FLAG_SUPPORTED 4
#define FLAGS_DTYPE uint16_t

#define SET(x, n)  ((x) | (1 << (n)))

// Macros to see if various flags are provided
#define IS_ALL(flags)  ((flags) & 1)
#define IS_LONG(flags) ((flags) & 2)
#define IS_HUM(flags)  ((flags) & 4)
#define IS_REC(flags)  ((flags) & 8)

#define RAISE_UNSUPPORTED_FLAG(x) { fprintf(stderr, "ls: unknown flag %c\n", x); exit(EXIT_FAILURE); }

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


// @brief parses commands line arguments given by argv
// @returns argv index of path argument if one is given else 0
// What wach bit of flag represents:
// 0 -> -a
// 1 -> -l
// 2 -> -h
// 3 -> -R
// NOTE: start of counting is from LSB
// After this functions returns, each bit of flags will
// be set according to the command line flags given
int parse_args(int argc, char *argv[], FLAGS_DTYPE *flags) {
	int j;
	int ret = 0;
	for (int i = 1; i < argc; i++) {
		// if not a flag (all flags start with -)
		if (argv[i][0] != '-') {
			ret = i; // index of last non flag argument will be returned
			continue;
		}
		
		j = 1;
		while (argv[i][j] != '\0') {
			switch (argv[i][j]) {
				case 'a':
					*flags = SET(*flags, 0);
					break;
				case 'l':
					*flags = SET(*flags, 1);
					break;
				default:
					RAISE_UNSUPPORTED_FLAG(argv[i][j]);
					exit(EXIT_FAILURE);
			}
			j++;
		}
	}

	return ret;
}

int main(int argc, char *argv[]) {
	char path[PATH_SIZE] = "."; // default is current directory 
	
	// support for user given path
	int path_idx;
	FLAGS_DTYPE flags = 0;
	if ((path_idx = parse_args(argc, argv, &flags)) > 0) {
		// check if user given path's length' is greater than supported length 
		if ((strlen(argv[path_idx]) + 1) > PATH_SIZE) {
			fprintf(stderr, "path too big: %s\n", argv[path_idx]);
			exit(EXIT_FAILURE);
		}
		strcpy(path, argv[path_idx]);
	}

	struct stat sb;
	if (stat(path, &sb) < 0) {
		perror("stat");
		exit(EXIT_FAILURE);
	}

	// if given path is not a directory, simply print the file name  and exit
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
		// skip if file is hidden and -a flag is not supplied
		if (!IS_ALL(flags) && (dir_content->d_name[0] == '.')) continue;
		
		// only print file name if -l is not supplied
		if (!IS_LONG(flags)) {
			printf("%s\n", dir_content->d_name);
			continue;
		}

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
