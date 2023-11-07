#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>



#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <openssl/md5.h>
#pragma GCC diagnostic pop
#include <stdlib.h>



char * calculate_file_md5(FILE *original_fopen_ret) {
	if (!original_fopen_ret) {
		return NULL;
	}
	char *filemd5 = (char*) malloc(33 *sizeof(char));

	// create file fingerprint
	unsigned char fingerprint[MD5_DIGEST_LENGTH];
	int length = 0;

	int current_pos = ftell( original_fopen_ret );
	fseek( original_fopen_ret , 0 , SEEK_END );
	length = ftell( original_fopen_ret );

	// read file
	fseek( original_fopen_ret , 0 , SEEK_SET );
	char buf[length];
	fread( buf , 1 , length , original_fopen_ret );

	// reset seek
	fseek( original_fopen_ret , 0 , current_pos );
	
	// create fingerprint
	MD5_CTX mdContext;
	MD5_Init( &mdContext );
	MD5_Update( &mdContext , buf , length );
	MD5_Final( fingerprint , &mdContext );
	for(int i = 0; i < MD5_DIGEST_LENGTH; i++) {
		sprintf(&filemd5[i*2], "%02x", (unsigned int)fingerprint[i]);
	}
	return filemd5;
}


char * recover_filename(FILE * stream) {
  	int fd = fileno(stream);
    char fd_path[256];
    char *filename = NULL;
    ssize_t n;

    // Construct the symbolic link path
    snprintf(fd_path, sizeof(fd_path), "/proc/self/fd/%d", fd);

    // Allocate memory for the filename
    filename = (char *)malloc(256); // Adjust the size as needed

    if (filename == NULL) {
        perror("Memory allocation failed");
        return NULL;
    }

    // Read the symbolic link to get the full path
    n = readlink(fd_path, filename, 256); // Adjust the size as needed

    if (n < 0) {
        perror("Failed to read the symbolic link");
        free(filename);
        return NULL;
    }

    // Null-terminate the full path string
    filename[n] = '\0';

    // Use the basename function to extract the filename from the full path
    char *base_name = basename(filename);

    // Create a new string to store just the filename
    char *result = strdup(base_name);

    free(filename); // Free the dynamically allocated memory for the full path

    return result;
}


FILE *fopen(const char *path, const char *mode) {
	// Log information 
	// User id
    uid_t user_id = getuid();
	
	// Time and date
    time_t current_time;
	struct tm *timeinfo;
	time(&current_time);
	timeinfo = localtime(&current_time);
	char date_str[50];
	char time_str[50];
	strftime(date_str, sizeof(date_str), "%Y-%m-%d", timeinfo);
	strftime(time_str, sizeof(time_str), "%H:%M:%S", timeinfo);

	// Access type
	int access_type;
	if (strchr(mode, 'w') || strchr(mode, 'a'))
		access_type = 0; // file creation
	else
		access_type = 1; // open file


	// standard I/O operation
	FILE* original_fopen_ret;
	FILE* (*original_fopen)(const char *, const char *);
	original_fopen = dlsym( RTLD_NEXT, "fopen");
	original_fopen_ret = (*original_fopen)(path, mode);


	// Access to file
	int s_action_denied = 0;
	//Check if action denied
	if(!original_fopen_ret) {
		s_action_denied = 1;
	}
	FILE* log = (*original_fopen)("file_logging.log", "a");

	
	chmod("file_logging", 0777);


	fprintf(log, "UID: %d\nFile Name: %s\nDate: %s\nTime: %s\nAccess Type: %d\nIs-action-denied flag: %d\nFile fingerprint: %s\n\n", user_id, path, date_str, time_str, access_type, s_action_denied, calculate_file_md5(original_fopen_ret));
	fclose(log);
	
	
    // Call the original fopen function
    return original_fopen_ret;
}



size_t fwrite(const void *ptr, size_t size, size_t count, FILE *stream) {
	// Log information 
	// User id
    uid_t user_id = getuid();
	
	// Time and date
    time_t current_time;
	struct tm *timeinfo;
	time(&current_time);
	timeinfo = localtime(&current_time);
	char date_str[50];
	char time_str[50];
	strftime(date_str, sizeof(date_str), "%Y-%m-%d", timeinfo);
	strftime(time_str, sizeof(time_str), "%H:%M:%S", timeinfo);

	// Access type
	int access_type=2;


	// standard I/O operation
	size_t original_fwrite_ret;
	size_t (*original_fwrite)(const void *, size_t, size_t, FILE*);
	original_fwrite = dlsym( RTLD_NEXT, "fwrite");
	original_fwrite_ret = (*original_fwrite)(ptr, size, count, stream);


	// Access to file
	int s_action_denied = 0;
	//Check if action denied
	if(!original_fwrite_ret) {
		s_action_denied = 1;
	}

	FILE* (*original_fopen)(const char*, const char*);
	original_fopen = dlsym( RTLD_NEXT, "fopen");
	FILE* log = (*original_fopen)("file_logging.log", "a");

	chmod("file_logging", 0777);
                                                                                                                                                                                                                                           

	fprintf(log, "UID: %d\nFile Name: %s\nDate: %s\nTime: %s\nAccess Type: %d\nIs-action-denied flag: %d\nFile fingerprint: %s\n\n", user_id, recover_filename(stream), date_str, time_str, access_type, s_action_denied, calculate_file_md5(stream));
	fclose(log);
        

    // Call the original fopen function
    return original_fwrite_ret;
}