#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int main() {
    char files[][20] = {
        "file1.txt",
        "file2.txt",
        "file3.txt",
        "file4.txt",
        "file5.txt",
        "file6.txt",
        "file7.txt",
        "file8.txt"
    };

    for (int i = 0; i < sizeof(files) / sizeof(files[0]); i++) {
        FILE *file = fopen(files[i], "r");
        if (file == NULL) {
            printf("Error: %s\n", strerror(errno));
        } else {
            // Acll should fail
            fclose(file);
        }
    }

    // Create a test file
    FILE *test_file = fopen("test_file.txt", "w");
    if (test_file) {
        fprintf(test_file, "This is a test file.\n");
        fclose(test_file);
    } else {
        perror("Failed to create the test file");
        return 1;
    }

    // Read the test file
    test_file = fopen("test_file.txt", "r");
    if (test_file) {
        char buffer[256];
        while (fgets(buffer, sizeof(buffer), test_file) != NULL) {
            printf("%s", buffer);
        }
        fclose(test_file);
    } else {
        perror("Failed to read the test file");
        return 1;
    }

    // Write to the test file
    test_file = fopen("test_file.txt", "a");
    if (test_file) {
        fprintf(test_file, "Apollo the dog.\n");
        fclose(test_file);
    } else {
        perror("Failed to write to the test file");
        return 1;
    }

    return 0;


}
