#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <signal.h>

void file_operations(int operation, char *filename, char *extra);
void directory_operations(int operation, char *dirname);
void start_keylogger(char *logfile);
void stop_keylogger();

int keylogger_running = 0;
pid_t keylogger_pid;

void menu() {
    int choice;
    while (1) {
        printf("\n--- SuperCommand Menu ---\n");
        printf("1. File Operations\n");
        printf("2. Directory Operations\n");
        printf("3. Keylogger\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        char filename[256], extra[256];
        switch (choice) {
            case 1:
                printf("Enter operation (1-Create, 2-Chmod, 3-Read, 4-Write, 5-Delete): ");
                int file_op;
                scanf("%d", &file_op);
                printf("Enter filename: ");
                scanf("%s", filename);
                if (file_op == 2 || file_op == 4) {
                    printf("Enter additional input: ");
                    scanf("%s", extra);
                } else {
                    strcpy(extra, "");
                }
                file_operations(file_op, filename, extra);
                break;
            case 2:
                printf("Enter operation (1-Create, 2-Remove, 3-PWD, 4-List): ");
                int dir_op;
                scanf("%d", &dir_op);
                if (dir_op == 1 || dir_op == 2) {
                    printf("Enter directory name: ");
                    scanf("%s", filename);
                } else {
                    strcpy(filename, ".");
                }
                directory_operations(dir_op, filename);
                break;
            case 3:
                if (!keylogger_running) {
                    printf("Enter log filename: ");
                    scanf("%s", filename);
                    start_keylogger(filename);
                } else {
                    stop_keylogger();
                }
                break;
            case 4:
                if (keylogger_running) {
                    stop_keylogger();
                }
                exit(0);
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }
}

void file_operations(int operation, char *filename, char *extra) {
    int fd;
    char data[1024];

    switch (operation) {
        case 1: // Create/Open File
            fd = open(filename, O_CREAT | O_RDWR, 0644);
            if (fd == -1) {
                perror("Error creating/opening file");
            } else {
                printf("File created/opened successfully.\n");
                close(fd);
            }
            break;
        case 2: // Change File Permissions
            if (chmod(filename, strtol(extra, NULL, 8)) == -1) {
                perror("Error changing file permissions");
            } else {
                printf("Permissions changed successfully.\n");
            }
            break;
        case 3: // Read File
            fd = open(filename, O_RDONLY);
            if (fd == -1) {
                perror("Error opening file");
            } else {
                int bytes_read = read(fd, data, sizeof(data) - 1);
                if (bytes_read == -1) {
                    perror("Error reading file");
                } else {
                    data[bytes_read] = '\0';
                    printf("File contents:\n%s\n", data);
                }
                close(fd);
            }
            break;
        case 4: // Write to File
            fd = open(filename, O_WRONLY | O_APPEND);
            if (fd == -1) {
                perror("Error opening file");
            } else {
                if (write(fd, extra, strlen(extra)) == -1) {
                    perror("Error writing to file");
                } else {
                    printf("Data written successfully.\n");
                }
                close(fd);
            }
            break;
        case 5: // Delete File
            if (unlink(filename) == -1) {
                perror("Error deleting file");
            } else {
                printf("File deleted successfully.\n");
            }
            break;
        default:
            printf("Invalid file operation.\n");
    }
}

void directory_operations(int operation, char *dirname) {
    char buffer[1024];

    switch (operation) {
        case 1: // Create Directory
            if (mkdir(dirname, 0755) == -1) {
                perror("Error creating directory");
            } else {
                printf("Directory created successfully.\n");
            }
            break;
        case 2: // Remove Directory
            if (rmdir(dirname) == -1) {
                perror("Error removing directory");
            } else {
                printf("Directory removed successfully.\n");
            }
            break;
        case 3: // Print Current Working Directory
            if (getcwd(buffer, sizeof(buffer)) == NULL) {
                perror("Error getting current working directory");
            } else {
                printf("Current Working Directory: %s\n", buffer);
            }
            break;
        case 4: { // List Directory Contents
            DIR *d;
            struct dirent *dir;
            d = opendir(dirname);
            if (d) {
                printf("Directory contents:\n");
                while ((dir = readdir(d)) != NULL) {
                    printf("%s\n", dir->d_name);
                }
                closedir(d);
            } else {
                perror("Error opening directory");
            }
            break;
        }
        default:
            printf("Invalid directory operation.\n");
    }
}

void start_keylogger(char *logfile) {
    printf("Starting keylogger...\n");
    keylogger_pid = fork();

    if (keylogger_pid == 0) {
        // Keylogger child process
        FILE *logfile_ptr = fopen(logfile, "a");
        if (!logfile_ptr) {
            perror("Error opening keylog file");
            exit(1);
        }

        time_t now = time(NULL);
        fprintf(logfile_ptr, "--- Keylogger session started: %s", ctime(&now));
        fclose(logfile_ptr);

        // Simulated keylogging logic (replace with actual implementation)
        while (1) {
            sleep(5); // Simulate logging interval
        }
    } else if (keylogger_pid > 0) {
        keylogger_running = 1;
        printf("Keylogger running in the background.\n");
    } else {
        perror("Error starting keylogger");
    }
}

void stop_keylogger() {
    printf("Stopping keylogger...\n");
    if (kill(keylogger_pid, SIGTERM) == 0) {
        keylogger_running = 0;
        printf("Keylogger stopped successfully.\n");
    } else {
        perror("Error stopping keylogger");
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        int mode = atoi(argv[1]);
        int operation = atoi(argv[2]);

        switch (mode) {
            case 1:
                file_operations(operation, argv[3], argc > 4 ? argv[4] : "");
                break;
            case 2:
                directory_operations(operation, argc > 3 ? argv[3] : ".");
                break;
            case 3:
                if (operation == 1 && argc > 3) {
                    start_keylogger(argv[3]);
                } else if (operation == 0) {
                    stop_keylogger();
                } else {
                    printf("Invalid keylogger command.\n");
                }
                break;
            default:
                printf("Invalid mode.\n");
        }
    } else {
        menu();
    }
    return 0;
}
