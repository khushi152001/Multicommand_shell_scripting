#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <signal.h>

#define MAX_P_LENGTH 256
// i have created a function that will check the process is in given tree or not 
int Check_process(pid_t processId, pid_t rootProcess) {
    char filename[1024];  // Buffer for file path
    char line[1024];    // Buffer for reading lines from files
    FILE *file;
    snprintf(filename, sizeof(filename), "/proc/%d/status", processId);
    file = fopen(filename, "r");
    //this condition will check if the process in not found in that tree
    if (file == NULL) {
        return 0; // Process does not exist
    }

    // this condition will read process id of parent 
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "PPid:", 5) == 0) {
            pid_t ppid;
            sscanf(line + 5, "%d", &ppid);
            fclose(file);
            return (ppid == rootProcess || ppid == processId || ppid == 1);
        }
    }

    fclose(file);
    //if ppid is not found 
    return 0;
}

// this Function  will desiplay the list of id ofimme desc of a process
void Display_imme_desc(pid_t processId) {
    DIR *dir;
    struct dirent *a;
    char path[1024];

    snprintf(path, sizeof(path), "/proc");

    if ((dir = opendir(path)) != NULL) {
        while ((a = readdir(dir)) != NULL) {
            if (a->d_type == DT_DIR) {
                // thiscondition  will be Checking  if  directory name 
                char *endptr;
                long pid = strtol(a->d_name, &endptr, 10);
                if (*endptr == '\0') {
                    // it Reads the status file to get the PPid (parent process ID)
                    char status_path[1024];
                    snprintf(status_path, sizeof(status_path), "/proc/%ld/status", pid);
                    FILE *status = fopen(status_path, "r");
                    if (status != NULL) {
                        char line[1024];
                        while (fgets(line, sizeof(line), status)) {
                            if (strncmp(line, "PPid:", 5) == 0) {
                                long ppid;
                                sscanf(line + 5, "%ld", &ppid);
                                if (ppid == processId) {
                                    printf("%ld\n", pid);
                                }
                                break;
                            }
                        }
                        fclose(status);
                    }
                }
            }
        }
        closedir(dir);
    }
}

// i have created logic in which it will print non direct desc of process id 
void listNonDirectDescendants(pid_t processId) {
    DIR *dir;
    struct dirent *a;
    char path[1024];

    snprintf(path, sizeof(path), "/proc");

    if ((dir = opendir(path)) != NULL) {
        while ((a = readdir(dir)) != NULL) {
            if (a->d_type == DT_DIR) {
                char *endptr;
                long pid = strtol(a->d_name, &endptr, 10);
                if (*endptr == '\0') {
                    // Read the status file to get the PPid (parent process ID)
                    char status_path[1024];
                    snprintf(status_path, sizeof(status_path), "/proc/%ld/status", pid);
                    FILE *status = fopen(status_path, "r");
                    if (status != NULL) {
                        char line[1024];
                        while (fgets(line, sizeof(line), status)) {
                            if (strncmp(line, "PPid:", 5) == 0) {
                                long ppid;
                                sscanf(line + 5, "%ld", &ppid);
                                if (ppid != processId && Check_process(ppid, processId)) {
                                    printf("%ld\n", pid);
                                }
                                break;
                            }
                        }
                        fclose(status);
                    }
                }
            }
        }
        closedir(dir);
    }
}

// this is the logic that will display the siblings of the of the process id


void listSiblingProcesses(pid_t process_id) {
    DIR *kp_procdir = opendir("/proc");
    if (kp_procdir == NULL) {
        perror("Error  /proc directory");
        return;
    }

    pid_t parent_pid = -1;
    char status_path[MAX_P_LENGTH];
    snprintf(status_path, sizeof(status_path), "/proc/%d/status", process_id);
    FILE *status = fopen(status_path, "r");
    if (status == NULL) {
        perror("ther is Error in opening the file");
        closedir(kp_procdir);
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), status)) {
        if (strncmp(line, "PPid:", 5) == 0) {
            sscanf(line + 6, "%d", &parent_pid);
            break;
        }
    }
    fclose(status);

    struct dirent *a;
    while ((a = readdir(kp_procdir)) != NULL) {
        if (a->d_type == DT_DIR) {
            pid_t pid = atoi(a->d_name);
            if (pid > 0 && pid != process_id) {
                char process_status_path[MAX_P_LENGTH];
                snprintf(process_status_path, sizeof(process_status_path), "/proc/%d/status", pid);
                FILE *process_status = fopen(process_status_path, "r");
                if (process_status != NULL) {
                    pid_t process_parent_pid = -1;
                    while (fgets(line, sizeof(line), process_status)) {
                        if (strncmp(line, "PPid:", 5) == 0) {
                            sscanf(line + 6, "%d", &process_parent_pid);
                            break;
                        }
                    }
                    fclose(process_status);
                    if (process_parent_pid == parent_pid) {
                        printf("%d\n", pid);
                    }
                }
            }
        }
    }

    closedir(kp_procdir);
}
// logic created to pause the  process id 
void pauseProcess(pid_t processId) {
    if (kill(processId, SIGSTOP) == 0) {
        printf("Process %d paused\n", processId);
    } else {
        perror("Error in pausing the process");
    }
}

// Function to resume (send SIGCONT) all previously paused processes
void resumePausedProcesses() {
    // i need to implement this function
    printf("Resuming paused processes\n");
}

// i have implemented the logic which will display information about zombie process 
void listDefunctProcesses(pid_t processId) {
    DIR *dir;
    struct dirent *a;
    char path[1024];

    snprintf(path, sizeof(path), "/proc");

    if ((dir = opendir(path)) != NULL) {
        while ((a = readdir(dir)) != NULL) {
            if (a->d_type == DT_DIR) {
                char *endptr;
                long pid = strtol(a->d_name, &endptr, 10);
                if (*endptr == '\0') {
                    // it reads ths status
                    char status_path[1024];
                    snprintf(status_path, sizeof(status_path), "/proc/%ld/status", pid);
                    FILE *status = fopen(status_path, "r");
                    if (status != NULL) {
                        char line[1024];
                        while (fgets(line, sizeof(line), status)) {
                            if (strncmp(line, "State:", 6) == 0) {
                                if (strstr(line, "defunct") != NULL && Check_process(pid, processId)) {
                                    printf("%ld\n", pid);
                                }
                                break;
                            }
                        }
                        fclose(status);
                    }
                }
            }
        }
        closedir(dir);
    }
}

// this will show the details og grandchilfren
void listGrandchildren(pid_t processId) {
     DIR *dir;
    struct dirent *a;

    dir = opendir("/proc");
    if (dir == NULL) {
        perror("Error  /proc directory");
        return;
    }

    while ((a = readdir(dir)) != NULL) {
        if (a->d_type != DT_DIR || !isdigit(*a->d_name))
            continue;

        pid_t pid = atoi(a->d_name);

        if (pid == getpid() || getppid() == pid)
            continue;

        char status_path[MAX_P_LENGTH];
        snprintf(status_path, sizeof(status_path), "/proc/%s/status", a->d_name);
        FILE *status = fopen(status_path, "r");
        if (status == NULL)
            continue;

        char line[256];
        pid_t ppid = -1;
        while (fgets(line, sizeof(line), status)) {
            if (strncmp(line, "PPid:", 5) == 0) {
                sscanf(line + 6, "%d", &ppid);
                break;
            }
        }
        fclose(status);

        if (ppid == processId) {
            printf("%s\n", a->d_name);
        }
    }

    closedir(dir);
}

// this function will show the status that if the process has became zombie or not 
void printProcessStatus(pid_t processId) {
    DIR *dir;
    struct dirent *a;
    char path[1024];

    snprintf(path, sizeof(path), "/proc");

    if ((dir = opendir(path)) != NULL) {
        while ((a = readdir(dir)) != NULL) {
            if (a->d_type == DT_DIR) {
                // this will check the directory name 
                char *endptr;
                long pid = strtol(a->d_name, &endptr, 10);
                if (*endptr == '\0' && pid == processId) {
                    char status_path[1024];
                    snprintf(status_path, sizeof(status_path), "/proc/%ld/status", pid);
                    FILE *status = fopen(status_path, "r");
                    if (status != NULL) {
                        char line[1024];
                        while (fgets(line, sizeof(line), status)) {
                            if (strncmp(line, "State:", 6) == 0) {
                                if (strstr(line, "defunct") != NULL) {
                                    printf("Defunct\n");
                                } else {
                                    printf("Not Defunct\n");
                                }
                                fclose(status);
                                return;
                            }
                        }
                        fclose(status);
                    }
                }
            }
        }
        closedir(dir);
    }
    printf("Process not found\n");
}

// Function to handle the various options
void handleOption(pid_t processId, pid_t rootProcess, const char *option) {
    if (strcmp(option, "-xd") == 0) {
        // List immediate descendants of processId
        Display_imme_desc(processId);
    } else if (strcmp(option, "-xn") == 0) {
        // List all non-direct descendants of processId
        listNonDirectDescendants(processId);
    } else if (strcmp(option, "-xs") == 0) {
        // List all sibling processes of processId
        listSiblingProcesses(processId);
    } else if (strcmp(option, "-xt") == 0) {
        // Pause the process
        pauseProcess(processId);
    } else if (strcmp(option, "-xc") == 0) {
        // Resume all previously paused processes
        resumePausedProcesses();
    } else if (strcmp(option, "-xz") == 0) {
        // List all defunct (zombie) processes
        listDefunctProcesses(processId);
    } else if (strcmp(option, "-xg") == 0) {
        // List all grandchildren of processId
        listGrandchildren(processId);
    } else if (strcmp(option, "-zs") == 0) {
        // Print process status (Defunct/Not Defunct)
        printProcessStatus(processId);
    } else if (strcmp(option, "-rp") == 0) {
        // Kill processId if it belongs to the process tree rooted at rootProcess
        if (Check_process(processId, rootProcess)) {
            if (kill(processId, SIGKILL) == 0) {
                printf("Process %d killed\n", processId);
            } else {
                perror("Error in killing the process");
            }
        } else {
            printf("this Process does not belongs to  process tree\n");
        }
    } else if (strcmp(option, "-pr") == 0) {
        // Kill the root process
        if (kill(rootProcess, SIGKILL) == 0) {
            printf("Root process %d killed\n", rootProcess);
        } else {
            perror("Error killing root process");
        }
    } else {
        printf("Invalid option\n");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s [process_id] [root_process] [OPTION]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Extract process_id, root_process, and option from command-line arguments
    pid_t processId = atoi(argv[1]);
    pid_t rootProcess = atoi(argv[2]);
    const char *option = argv[3];

    // Check if processId belongs to the process tree rooted at rootProcess
    if (!Check_process(processId, rootProcess)) {
        printf("Does not belong to the process tree\n");
        exit(EXIT_SUCCESS);
    }

    // Handle the specified option
    handleOption(processId, rootProcess, option);

    return 0;
}
