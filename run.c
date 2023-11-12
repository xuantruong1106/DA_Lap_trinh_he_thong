#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <dirent.h>

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

// void getFileType(const char* path) {
//     char command[1024];
//     snprintf(command, sizeof(command), "file -b %s", path);

//     FILE *fp = popen(command, "r");
//     if (fp == NULL) {
//         perror("Error executing 'file' command");
//         return;
//     }

//     char buffer[1024];
//     if (fgets(buffer, sizeof(buffer), fp) != NULL) {
//         printf("File type: %s", buffer);
//     } else {
//         printf("File type not available\n");
//     }

//     pclose(fp);
// }

void getFileType(const char* path) {
    char command[1024];
    snprintf(command, sizeof(command), "file -b %s", path);

    FILE *fp = popen(command, "r");
    if (fp == NULL) {
        perror("Error executing 'file' command");
        return;
    }

    int c;
    while ((c = fgetc(fp)) != EOF) {
        putchar(c);
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

void CheckUserFilePermissions(const char *filename) {
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

void displayFilePermissions(const char *path){
    struct stat file_stat;

    // Lấy thông tin về tệp
    if (stat(path, &file_stat) != 0) {
        printf( "don't get file info");
    }

    // Dạng tệp/thư mục
    char type = (S_ISDIR(file_stat.st_mode)) ? 'd' : '-';

    // Quyền của Chủ Nhân
    char owner_permissions[4];
    owner_permissions[0] = (file_stat.st_mode & S_IRUSR) ? 'r' : '-';
    owner_permissions[1] = (file_stat.st_mode & S_IWUSR) ? 'w' : '-';
    owner_permissions[2] = (file_stat.st_mode & S_IXUSR) ? 'x' : '-';
    owner_permissions[3] = '\0';

    // Quyền của Nhóm
    char group_permissions[4];
    group_permissions[0] = (file_stat.st_mode & S_IRGRP) ? 'r' : '-';
    group_permissions[1] = (file_stat.st_mode & S_IWGRP) ? 'w' : '-';
    group_permissions[2] = (file_stat.st_mode & S_IXGRP) ? 'x' : '-';
    group_permissions[3] = '\0';

    // Quyền của Tất Cả Mọi Người
    char others_permissions[4];
    others_permissions[0] = (file_stat.st_mode & S_IROTH) ? 'r' : '-';
    others_permissions[1] = (file_stat.st_mode & S_IWOTH) ? 'w' : '-';
    others_permissions[2] = (file_stat.st_mode & S_IXOTH) ? 'x' : '-';
    others_permissions[3] = '\0';

    // Chuỗi mô tả
    static char result[100];
    sprintf(result, "Quyền truy cập của tệp %s: %c%s%s%s", path, type, owner_permissions, group_permissions, others_permissions);
    printf("%s\n", result);

}


void selectOption(const char *path) {
    int length = strlen("|3. Get file group and id|");
    for(int i=0; i <= length+1; i++){
        printf("-");
    }
    printf("\n|1. Read file content\n");
    printf("|2. Get file type\n");
    printf("|3. Get file group and id\n");
    printf("|4. Get file last access time\n");
    printf("|5. Number of links\n");
    printf("|6. Check user file permissions\n");
    printf("|8. Display file permissions\n");
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
            NumberofLink(path);
            break;
        case '6':
            printf("You selected option 6\n");
            CheckUserFilePermissions(path);
            break;
        case '8':
            printf("You selected option 8\n");
            displayFilePermissions(path);
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
