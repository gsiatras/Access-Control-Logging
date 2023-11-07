#include <stdio.h>
#include <string.h>
#define MAX_USERS 5

typedef struct {
    int uid;
    int denied_count;
    int times_modified;
    char fingerprint[64];
} User;


int isUIDInArray(int uid, User uids[], int uidCount) {
    for (int i = 0; i < uidCount; i++) {
        if (uids[i].uid == uid) {
            return i; // UID found in the array
        }
    }
    return -1; // UID not found in the array
}



int printUsers(const char *filename_given) {
    User uids[MAX_USERS];
    int uidCount = 0;

    FILE *logFile = fopen("file_logging.log", "r");

    if (logFile == NULL) {
        perror("Failed to open log file");
        return 0;
    }

    char buffer[256];
    int foundFile = 0;
    int currentUID = -1; // Initialize to an invalid UID
    char prevFingerprint[64] = ""; // Initialize to an empty string
    char currFingerprint[64] = ""; // Initialize to an empty string

    // Read the log file from end to start
    fseek(logFile, 0, SEEK_END);
    long filePos = ftell(logFile);

    while (filePos >= 0) {
        fseek(logFile, filePos, SEEK_SET);
        fgets(buffer, sizeof(buffer), logFile);
        filePos--;

        if (strstr(buffer, "File fingerprint:") != NULL) {
            // Read and store the file fingerprint
            char *fingerprint = strchr(buffer, ':');
            if (fingerprint != NULL) {
                fingerprint++;
                while (*fingerprint == ' ' || *fingerprint == '\t') {
                    fingerprint++; // Skip leading spaces or tabs
                }
                // Remove newline character (if present)
                char *newline = strchr(fingerprint, '\n');
                if (newline != NULL) {
                    *newline = '\0'; // Replace newline with null terminator
                }
                strncpy(prevFingerprint, fingerprint, sizeof(prevFingerprint) - 1);
            }
        }
        
        if (strstr(buffer, "File Name:") != NULL) {
            // Check if the file name matches the given filename
            char *filename = strchr(buffer, ':');

            if (filename != NULL) {
                filename++;
                while (*filename == ' ' || *filename == '\t') {
                    filename++; // Skip leading spaces or tabs
                }
                // Remove newline character (if present)
                char *newline = strchr(filename, '\n');
                if (newline != NULL) {
                    *newline = '\0'; // Replace newline with null terminator
                }

                //printf("%s,%s\n", filename_given, filename);
                if (strcmp(filename, filename_given) == 0) {
                    foundFile = 1;
                    strncpy(currFingerprint, prevFingerprint, sizeof(currFingerprint) - 1); // Copy fingerprint
                    //printf("1\n");
                } else {
                    //printf("2\n");
                    foundFile = 0; // Reset foundFile if file name doesn't match
                }
            }
        } else if (foundFile) {
            if (sscanf(buffer, "UID: %d", &currentUID) == 1) {
                int userIndex = isUIDInArray(currentUID, uids, uidCount);
                //printf("%d", userIndex);
                if (userIndex != -1) {
                    if (strcmp(currFingerprint, uids[userIndex].fingerprint) != 0) {
                        // Fingerprint has changed, increment times_modified
                        uids[userIndex].times_modified++;
                        strncpy(uids[userIndex].fingerprint, currFingerprint, sizeof(currFingerprint) - 1);
                        foundFile = 0;
                    }
                } else if (uidCount < MAX_USERS) {
                    // Check if the UID is not in the array before adding
                    uids[uidCount].uid = currentUID;
                    uids[uidCount].times_modified = 0;
                    strncpy(uids[uidCount].fingerprint, currFingerprint, sizeof(uids[userIndex].fingerprint) - 1);
                    uidCount++;
                    foundFile = 0;
                }
            }
        }
    }

    fclose(logFile);

    // Print users
    if (uidCount > 0) {
        printf("Users who tried to access %s and their times_modified:\n", filename_given);
        for (int i = 0; i < uidCount; i++) {
            printf("UID: %d, times_modified: %d\n", uids[i].uid, uids[i].times_modified);
        }
    } else {
        printf("No users found for %s\n", filename_given);
    }
    return 1;
}

    




int printMalUsers() {
    FILE *logFile = fopen("file_logging.log", "r");

    if (logFile == NULL) {
        perror("Failed to open log file");
        return 1;
    }
    User users[MAX_USERS];
    int userCount = 0;

    char buffer[256];
    int isActionDenied = 0;
    int currentUID = -1; // Initialize to an invalid UID

    // Fill the array with users and number of denied accesses
    while (fgets(buffer, sizeof(buffer), logFile) != NULL) {
        if (sscanf(buffer, "UID: %d", &currentUID) == 1) {
            // Check if the current UID is not in the users array
            int userIndex = -1;

            for (int i = 0; i < userCount; i++) {
                if (users[i].uid == currentUID) {
                    userIndex = i;
                    break;
                }
            }

            if (userIndex == -1) {
                if (userCount < MAX_USERS) {
                    // Add a new user to the array
                    users[userCount].uid = currentUID;
                    users[userCount].denied_count = 0;
                    userCount++;
                } else {
                    // Handle array full condition (optional)
                }
            }
        } else if (strstr(buffer, "Is-action-denied flag: 1") != NULL) {
            // Increment the denied count for the current user
            if (currentUID != -1) {
                for (int i = 0; i < userCount; i++) {
                    if (users[i].uid == currentUID) {
                        users[i].denied_count++;
                        break;
                    }
                }
            }
        }
    }  


    // Print malicious users
    for (int i = 0; i < userCount; i++) {
        if (users[i].denied_count >= 7) {
            printf("UID: %d, denied_count: %d\n", users[i].uid, users[i].denied_count);
        }
    }

    fclose(logFile);
}


void printHelp() {
    printf("Usage:\n");
    printf("-m: List of malicious users\n");
    printf("-i <filename>: List of users that modified the file and the number of modifications\n");
    printf("-h: Display help\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("No options provided. Use -h for help.\n");
        return 1;
    }

    if (strcmp(argv[1], "-m") == 0) {
        printMalUsers();
    } else if (strcmp(argv[1], "-i") == 0 && argc >= 3) {
        printUsers(argv[2]);
    } else if (strcmp(argv[1], "-h") == 0) {
        printHelp();
    } else {
        printf("Invalid option. Use -h for help.\n");
        return 1;
    }

    return 0;
}
