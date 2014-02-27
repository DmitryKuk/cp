// copy.c
// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include "copy.h"

// Copy from file descriptor into another one.
// Returns: OK, ERROR_READ, ERROR_WRITE
static int copy_fd_fd(int src_fd, int dest_fd, void *buf, size_t buf_size)
{
	int n;
	while ((n = read(src_fd, buf, BUFFER_SIZE)) > 0)	// Chech read status
		if (write(dest_fd, buf, n) <= 0)	// Check write status
			return ERROR_WRITE;
	if (n < 0)	// Check read status
		return ERROR_READ;
	return OK;
}


// Copy from file into another one.
// Returns: OK, ERROR_OPEN_READ, ERROR_OPEN_WRITE, ERROR_READ, ERROR_WRITE
int copy_file_file(const char *src_filename, const char *dest_filename, mode_t dest_file_mode)
{
	int src_fd = open(src_filename, O_RDONLY);
	if (src_fd < 0) {
		print_error(2, CANT_OPEN_FILE_, src_filename);
		return ERROR_OPEN_READ;
	}
	
	int dest_fd = open(dest_filename, O_WRONLY | O_TRUNC | O_CREAT, dest_file_mode);
	if (dest_fd < 0) {
		print_error(2, CANT_OPEN_FILE_, dest_filename);
		return ERROR_OPEN_WRITE;
	}
	
	void *buf = malloc(BUFFER_SIZE);
	if (buf == NULL) {
		print_error(1, ALLOCATION_ERROR);
		close(src_fd);
		close(dest_fd);
		return ERROR_ALLOCATION;
	}
	
	int status = copy_fd_fd(src_fd, dest_fd, buf, BUFFER_SIZE);
	switch (status) {
	case ERROR_READ:
		print_error(2, CANT_READ_FROM_FILE_, src_filename);
		break;
	case ERROR_WRITE:
		print_error(2, CANT_WRITE_TO_FILE_, dest_filename);
		break;
	default:
		break;
	}
	
	free(buf);
	close(src_fd);
	close(dest_fd);
	return status;
}


// Copy file into dir.
// Returns: OK, ERROR_OPEN_READ, ERROR_OPEN_WRITE, ERROR_READ, ERROR_WRITE
static int copy_file_dir(const char *src_filename, const char *dest_path, mode_t dest_file_mode)
{
	size_t dest_path_len = strlen(dest_path);
	const char *src_filename_only = src_filename + filename(src_filename);
	
	// Creating dest_filename
	char *dest_filename = malloc(dest_path_len + strlen(src_filename_only) + ((dest_path[dest_path_len] == '/')? 1: 2));
	if (dest_filename == NULL) {
		print_error(1, ALLOCATION_ERROR);
		return ERROR_ALLOCATION;
	}
	
	strcpy(dest_filename, dest_path);
	if (dest_path[dest_path_len - 1] == '/')
		strcpy(dest_filename + dest_path_len, src_filename_only);
	else {
		dest_filename[dest_path_len] = '/';
		strcpy(dest_filename + dest_path_len + 1, src_filename_only);
	}
	
	int status = copy_file_file(src_filename, dest_filename, dest_file_mode);
	free(dest_filename);
	return status;
}


// Copy dir to another dir, if that another does not exist, or copy dir into another one, otherwise.
// Called only with '-R' option, uses recursive calls.
// Returns: OK, ERROR_ALLOCATION, ERROR_OPEN_READ, ERROR_OPEN_WRITE, ERROR_READ, ERROR_WRITE
static int copy_dir_dir(const char *src_dirname, const char *dest_dirname, mode_t dest_dir_mode)
{
	// Opening source directory
	DIR *src_dir = opendir(src_dirname);
	if (src_dir == NULL) {
		print_error(2, CANT_OPEN_DIR_, src_dirname);
		return ERROR_OPEN_READ;
	}
	
	
	// Making pathes
	size_t src_dirname_only_pos = dirname(src_dirname);
	const char *src_dirname_only = src_dirname + src_dirname_only_pos;
	size_t src_dirname_only_len = strlen(src_dirname_only);
	
	// ... src path
	size_t src_path_pos = src_dirname_only_pos + src_dirname_only_len;
	size_t src_path_cap = src_path_pos + ((src_dirname[src_path_pos - 1] == '/')? 0: 1) + 2;
	
	char *src_path = malloc(src_path_cap + 1);
	if (src_path == NULL) {
		print_error(1, ALLOCATION_ERROR);
		closedir(src_dir);
		return ERROR_ALLOCATION;
	}
	strcpy(src_path, src_dirname);
	if (src_path[src_path_pos - 1] != '/') {
		src_path[src_path_pos] = '/';
		src_path[++src_path_pos] = '\0';
	}
	
	// ... dest path
	size_t dest_path_pos = strlen(dest_dirname);
	size_t dest_path_cap = dest_path_pos + src_dirname_only_len + ((dest_dirname[dest_path_pos - 1] == '/')? 0: 1);
	
	char *dest_path = malloc(dest_path_cap + 1);
	if (dest_path == NULL) {
		print_error(1, ALLOCATION_ERROR);
		free(src_path);
		closedir(src_dir);
		return ERROR_ALLOCATION;
	}
	strcpy(dest_path, dest_dirname);
	if (dest_path[dest_path_pos - 1] != '/') {
		dest_path[dest_path_pos] = '/';
		dest_path[++dest_path_pos] = '\0';
	}
	
	// Trying to make the directory named dest_dirname.
	// If error, trying to make the directory like dest_dirname/src_dirname_only.
	// Creating dest directory, if not exists
	if (mkdir(dest_path, dest_dir_mode)) {
		strcpy(dest_path + dest_path_pos, src_dirname_only);
		dest_path_pos = dest_path_cap;
		if (dest_path[dest_path_pos - 1] != '/') {
			dest_path[dest_path_pos] = '/';
			dest_path[++dest_path_pos] = '\0';
		}
		
		// Creating dest directory, if not exists
		if (mkdir(dest_path, dest_dir_mode) && errno != EEXIST) {
			print_error(2, CANT_OPEN_DIR_, dest_path);
			free(src_path);
			free(dest_path);
			closedir(src_dir);
			return ERROR_OPEN_WRITE;
		}
	}
	
	// ... and correcting permissions for it, because MAN says:
	// > Note: the behavior of mkdir() is undefined when mode bits other than the
    // > low 9 bits are used. Use chmod(2) after mkdir() to explicitly set the
    // > other bits (See example below).
	if (chmod(dest_path, dest_dir_mode))
		print_error(2, CANT_SET_PERMISSIONS_, dest_path);
	
	
	// Getting links to parent and current dir
	ino_t parent_ino, self_ino;
	{
		struct stat st1, st2;
		strcpy(src_path + src_path_pos, ".");
		stat(src_path, &st1);
		strcpy(src_path + src_path_pos + 1, ".");
		stat(src_path, &st2);
		self_ino = st1.st_ino;
		parent_ino = st2.st_ino;
	}
	
	struct dirent *dp;
	while ((dp = readdir(src_dir)) != NULL) {
		// Skipping links to parent or current dir
		if (dp->d_ino == parent_ino || dp->d_ino == self_ino)
			continue;
		
		// Correcting pathes
		size_t name_len = strlen(dp->d_name);	// Mac OS: d_namlen
		
		// ... src path
		if (src_path_pos + name_len > src_path_cap) {
			src_path_cap = src_path_pos + name_len;
			char *tmp_str = realloc(src_path, src_path_cap + 1);
			if (tmp_str == NULL) {
				print_error(1, ALLOCATION_ERROR);
				free(src_path);
				free(dest_path);
				closedir(src_dir);
				return ERROR_ALLOCATION;
			}
			src_path = tmp_str;
		}
		strcpy(src_path + src_path_pos, dp->d_name);
		
		// ... dest path
		if (dest_path_pos + name_len > dest_path_cap) {
			dest_path_cap = dest_path_pos + name_len;
			char *tmp_str = realloc(dest_path, dest_path_cap + 1);
			if (tmp_str == NULL) {
				print_error(1, ALLOCATION_ERROR);
				free(src_path);
				free(dest_path);
				closedir(src_dir);
				return ERROR_ALLOCATION;
			}
			dest_path = tmp_str;
		}
		strcpy(dest_path + dest_path_pos, dp->d_name);
		
		
		// Recursively call the copy function, which is dispather
		copy(src_path, dest_path, 1);
	}
	
	
	free(src_path);
	free(dest_path);
	closedir(src_dir);
	return OK;
}


// The main copy function. Calls other copy_* functions.
// Returns: OK, ERROR_OPEN_READ, ERROR_OPEN_WRITE, ERROR_READ, ERROR_WRITE
int copy(const char *src_path, const char *dest_path, char recursive)
{
	// Getting info about pathes
	struct stat src_stat, dest_stat;
	
	// ... src path
	if (stat(src_path, &src_stat)) {	// Not lstat, because will fail with links copying
		print_error(2, CANT_GET_STATUS_, src_path);
		return ERROR_GET_STATUS;
	}
	
	// If src_path is directory, trying to copy it
	if (src_stat.st_mode & S_IFDIR) {
		if (recursive)
			return copy_dir_dir(src_path, dest_path, src_stat.st_mode);
		else {
			print_strings(2, 2, CANT_COPY_DIR_, src_path);
			return ERROR_NEED_RECURS;
		}
	}
	
	// ... dest path
	if (stat(dest_path, &dest_stat))	// Not lstat (see above)
		return copy_file_file(src_path, dest_path, src_stat.st_mode);
	else {
		if (src_stat.st_ino == dest_stat.st_ino)	// Src and dest are the same
			return OK;
		
		if (dest_stat.st_mode & S_IFDIR)
			return copy_file_dir(src_path, dest_path, src_stat.st_mode);
		else
			return copy_file_file(src_path, dest_path, src_stat.st_mode);
	}
}