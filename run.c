#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

void readFileContent(const char *path) {
    FILE *fptr;
    char c;

    if ((fptr = fopen(path, "r")) == NULL) {
        printf("Error! Opening file '%s'\n", path);
        exit(1);
    }

    printf("Data from the file '%s'\n", path);

    // Đọc và hiển thị toàn bộ nội dung của tệp
    while ((c = fgetc(fptr)) != EOF) {
        printf("%c", c);
    }

    fclose(fptr);
}

void getFileType(const char* filename) {
     char command[1024];
    snprintf(command, sizeof(command), "file -b %s", filename); // Sử dụng lệnh 'file -b' để lấy loại tệp

    FILE *fp = popen(command, "r");
    if (fp == NULL) {
        perror("Error executing 'file' command");
        return;
    }

    char buffer[1024];
    if (fgets(buffer, sizeof(buffer), fp) != NULL) {
        printf("File type: %s", buffer);
    } else {
        printf("File type not available\n");
    }

    pclose(fp);
}

void getFileGroupAndID(const char *filename) {
    struct stat fileStat;
    if (stat(filename, &fileStat) != 0) {
        perror("Error accessing file information");
        return;
    }

    struct group *grp = getgrgid(fileStat.st_gid);
    if (grp != NULL) {
        printf("File group: %s (ID: %d)\n", grp->gr_name, grp->gr_gid);
    }

    struct passwd *pwd = getpwuid(fileStat.st_uid);
    if (pwd != NULL) {
        printf("Owner user: %s (ID: %d)\n", pwd->pw_name, pwd->pw_uid);
    }
}

void getFileLastAccessTime(const char *filename) {
    struct stat fileStat;
    if (stat(filename, &fileStat) != 0) {
        perror("Error accessing file information");
        return;
    }

    struct tm *access_time = localtime(&fileStat.st_atime);
    char time_str[30];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", access_time);

    printf("Last access time: %s\n", time_str);
}

void NumberofLink(const char *path) {
    DIR *dir;
    struct dirent *entry;
    struct stat fileStat;

    if ((dir = opendir(path)) == NULL) {
        perror("Error opening directory");
        return;
    }

    int totalLinks = 0;

    while ((entry = readdir(dir)) != NULL) {
        char filePath[1024];
        snprintf(filePath, sizeof(filePath), "%s/%s", path, entry->d_name);

        if (lstat(filePath, &fileStat) == -1) {
            perror("Error accessing file information");
            continue;
        }

        totalLinks += fileStat.st_nlink;
    }

    closedir(dir);

    printf("Total number of links: %d\n", totalLinks);
}

void displayPermissions(const char *filename) {
    struct stat fileStat;
    if (stat(filename, &fileStat) != 0) {
        perror("Error accessing file information");
        return;
    }

    printf("File permissions: ");

    // Kiểm tra quyền truy cập của người chủ
    printf((fileStat.st_mode & S_IRUSR) ? "The file owner has access\n" : "The file owner don't have access\n");

    // Kiểm tra quyền truy cập của người khác
    printf((fileStat.st_mode & S_IROTH) ? "Guests have access\n" : "Guests do not have access\n");

    printf("\n");
}


void selectOption(const char *path) {
    int length = strlen("|3. Get file group and id|");
    for(int i=0; i <= length+1; i++){
        printf("-");
    }
    printf("\n|1. Read file content\n");
    printf("|2. Get file type\n");
    printf("|3. Get file group and id\n");
    printf("|4. Get File Last Access Time\n");
    printf("|5. Option 5\n");
    for(int i=0; i<= length+1; i++){    
        printf("-");
    }
    printf("\nPress a key (1-5) or 0 to exit: ");
   
    char key;
    scanf(" %c", &key); // Sử dụng khoảng trắng trước %c để bỏ qua ký tự xuống dòng

    switch (key) {
        case '1':
            printf("You selected option 1\n");
            readFileContent(path);
            break;
        case '2':
            printf("You selected option 2\n");
            getFileType(path);
            break;
        case '3':
            printf("You selected option 3\n");
            getFileGroupAndID(path);
            break;
        case '4':
            printf("You selected option 4\n");
            getFileLastAccessTime(path);
            break;
        case '5':
            printf("You selected option 5\n");
            NumberOfLinks(path);
            break;
        case '6':
            printf("You selected option 6\n");
            displayPermissions(path);
            break;
        case '0':
            exit(0);
            break;
        default:
            printf("Invalid option\n");
    }
}

int main() {
    char file_path[1024];

    printf("Enter the full file path: ");
    if (fgets(file_path, sizeof(file_path), stdin) != NULL) {
        size_t length = strlen(file_path);
        if (length > 0 && file_path[length - 1] == '\n') {
            file_path[length - 1] = '\0';
            while (1) {
                selectOption(file_path);
            }
        }
    } else {
        printf("Error entering file path.\n");
    }

    return 0;
}
