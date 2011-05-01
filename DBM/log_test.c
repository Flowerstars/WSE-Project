#include "log_manager.h"

#define DEFAULT_LEVEL 	"err"
#define DEFAULT_PATH	"log_test.txt"


void usage(const char* program)
{
    printf("\nUSAGE: %s [options] <output-log-filename>\n", program);
    printf("Opion list:\n");
    printf("info	show logs with level INFO.\n");
    printf("warn	show logs with level INFO and WARNING.\n");
	printf("err	show logs with level INFO, WARNING and ERROR.\n");
	printf("-h	print this help.\n");
}

void main(int argc, char *argv[])
{
	char level[8];
	char file_path[64];

	memset(level, 0, sizeof(level));
	memset(file_path, 0, sizeof(file_path));

	switch (argc)
	{
		case 1:
			strcpy(level, DEFAULT_LEVEL);
			strcpy(file_path, DEFAULT_PATH);
			break;
		case 2:
			if (strncmp(argv[1], "-h", 2) == 0)
			{
				usage(argv[0]);
				return;
			}
			else
			{
				strcpy(level, argv[1]);
				strcpy(file_path, DEFAULT_PATH);
			}
			break;
		case 3:
			strcpy(level, argv[1]);
			strcpy(file_path, argv[2]);
			break;
		default:
			usage(argv[0]);
			break;
	}

	set_log_level(level);
	set_log_file_info(file_path);

	LOG_INFO(LOG_TYPE_THREAD, "thread_id = %d , thread_dscrp = %s\n", 1, "Thread 1.");
	LOG_WARN(LOG_TYPE_SOCKET, "socket_id = %d , socket_dscrp = %s\n", 2, "Socket 1.");
	LOG_ERR(LOG_TYPE_BUFFER, "buffer_id = %d , buffer_dscrp = %s\n", 3, "Buffer 1.");
}
