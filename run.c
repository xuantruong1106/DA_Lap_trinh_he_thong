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
#include <libgen.h>

char file_path[1024];
char destinationPath[1024];

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

    while ((entry = readdir(dir)) != NULL) {
        char filePath[1024];
        snprintf(filePath, sizeof(filePath), "%s/%s", path, entry->d_name);

        if (lstat(filePath, &fileStat) == -1) {
            perror("Error accessing file information");
            continue;
        }

        if (S_ISDIR(fileStat.st_mode) && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            printf("Directory: %s, Number of links: %ld\n", entry->d_name, (long)fileStat.st_nlink);
        }
    }

    closedir(dir);
}


void changeFileName(const char* filename) {
    char newFileName[1024];
    printf("Enter the new file name: ");
    scanf("%s", newFileName);
    
    if (rename(filename, newFileName) == 0) {
        printf("File name changed successfully\n");
    } else {
        perror("Error changing file name");
    }
}

void deleteFile(const char* filename) {
    if (remove(filename) == 0) {
        printf("File deleted successfully\n");
    } else {
        perror("Error deleting file");
    }
}

void displayPermissions(const char *filename) {
    
    struct stat fileStat;
    if (stat(filename, &fileStat) != 0) {
        perror("Error accessing file information");
        return;
    }

    printf("File permissions (in octal): %o\n", fileStat.st_mode & 0777);
}

void changePermissions(const char *filename){
    int mode;

    printf("Enter the permissions (in octal): ");
    scanf("%o", &mode);

    // Thay đổi quyền truy cập của tệp
    if (chmod(filename, mode) < 0) {
        perror("Error changing permissions");  
    }

    printf("Permissions changed successfully\n");
   
}

void getFileSize(const char *filename) {
    struct stat fileStat;
    if (stat(filename, &fileStat) != 0) {
        perror("Error accessing file information");
        return;
    }

    printf("File size: %ld bytes\n", fileStat.st_size);
}


void copyFile(const char *sourcePath, const char *destinationPath) {
    FILE *sourceFile, *destinationFile;
    char c;

    if ((sourceFile = fopen(sourcePath, "r")) == NULL) {
        printf("Error! Opening source file '%s'\n", sourcePath);
        return;
    }

    if ((destinationFile = fopen(destinationPath, "w")) == NULL) {
        printf("Error! Opening or creating destination file '%s'\n", destinationPath);
        fclose(sourceFile);
        return;
    }

    while ((c = fgetc(sourceFile)) != EOF) {
        fputc(c, destinationFile);
    }

    fclose(sourceFile);
    fclose(destinationFile);

    printf("File copied successfully from '%s' to '%s'\n", sourcePath, destinationPath);
}

void backupData() {
    printf("Enter file name to backup: ");
    scanf("%s", destinationPath);  
    copyFile(file_path, destinationPath);
    printf("Data backed up successfully from '%s' to '%s'\n", file_path, destinationPath);
}

void restoreData(const char *sourcePath, const char *destinationPath) {
    if (rename(sourcePath, destinationPath) == 0) {
        printf("Data restored successfully from '%s' to '%s'\n", sourcePath, destinationPath);
    } else {
        perror("Error while restoring file");
    }
}

void move_file(const char *source_path) {
    char destination_path[100];
    
    printf("Nhap duong dan dich (vd: user/admin/new_folder/a.txt): ");
    scanf(" %99s", destination_path); // Đảm bảo không vượt quá kích thước mảng

    // Kiểm tra đường dẫn đích hợp lệ
    if (strlen(destination_path) == 0) {
        printf("Duong dan khong hop le.\n");
    }

    if (rename(source_path, destination_path) == 0) {
        printf("Da di chuyen thanh cong tu %s den %s\n", source_path, destination_path);
    } else {
        perror("Loi khi di chuyen tep");
    }
}

void mergeFiles(const char *outputFile, char *inputFiles[], int numFiles) {
    FILE *outFile = fopen(outputFile, "w");
    if (outFile == NULL) {
        perror("Error opening output file");
        return;
    }

    for (int i = 0; i < numFiles; i++) {
        FILE *inFile = fopen(inputFiles[i], "r");
        if (inFile == NULL) {
            perror("Error opening input file");
            fclose(outFile);
            return;
        }

        int c;
        while ((c = fgetc(inFile)) != EOF) {
            fputc(c, outFile);
        }

        fclose(inFile);
    }

    fclose(outFile);
}

void changeFilePath(){
    printf("Enter new the full new file path: ");   
    while(getchar() != '\n'){
        printf("---");
    }
    
    if (fgets(file_path, sizeof(file_path), stdin) != NULL) {
        size_t length = strlen(file_path);
        if (length > 0 && file_path[length - 1] == '\n') {
            file_path[length - 1] = '\0';
        }
    } else {
        printf("Error entering file path.\n");
    }
}

void selectOption(const char *path) {

    int length = strlen("|6. Check user file permissions|"); // lấy chuỗi dài nhất để vẽ các dấu - chuẩn hơn

    for(int i=0; i <= length+1; i++){
        printf("-");
    }

    printf("\n|1. Read file content\n");
    printf("|2. Get file type\n");
    printf("|3. Get file group and id\n");
    printf("|4. Get file last access time\n");
    printf("|5. Number of links\n");
    printf("|6. Check user file permissions\n");
    printf("|7. Change file name\n");
    printf("|8. Delete file\n");
    printf("|9. Get file size\n");
    printf("|10. Backup data\n");
    printf("|11. Restore data\n");
    printf("|12. Move file\n");
    printf("|13. change file permissions\n");
    printf("|14. MergeFiles\n");
    printf("|15. Change file path\n");
    for(int i=0; i<= length+1; i++){    
        printf("-");
    }
    printf("\nPress a key (1-14) or 0 to exit: ");
   
    int key;
    // scanf(" %d", &key); // Sử dụng khoảng trắng trước %c để bỏ qua ký tự xuống dòng

    while(scanf(" %d", &key) != 1){
        while (getchar() != '\n')
        printf("xin chon so phu hop ");
        if(scanf(" %d", &key) == 1){
            break;
        }
    }

    switch (key) {
        case 1:
            printf("You selected option 1\n");
            readFileContent(path);
            break;
        case 2:
            printf("You selected option 2\n");
            getFileType(path);
            break;
        case 3:
            printf("You selected option 3\n");
            getFileGroupAndID(path);
            break;
        case 4:
            printf("You selected option 4\n");
            getFileLastAccessTime(path);
            break;
        case 5:
            printf("You selected option 5\n");
            NumberofLink(path);
            break;
        case 6:
            printf("You selected option 6\n");
            displayPermissions(path);
            break;
        case 7:
            printf("You selected option 7\n");
	        changeFileName(path);
            break;
        case 8:
                printf("You selected option 8\n");
                deleteFile(path);
            exit(0);
                break;
        case 9:
                printf("You selected option 9\n");
                getFileSize(file_path);
                break;
        case 10:
            printf("You selected option 10\n");
            backupData();
            break;
        case 11:
            printf("You selected option 11\n");
            restoreData(destinationPath, file_path);
            break;
         case 12:
            printf("You selected option 12\n");
            move_file(path);
            break;
        case 13:
            printf("You selected option 13\n");
            changePermissions(path);
            break;
	case 14: {
                printf("You selected option 14\n");
                printf("Enter the number of files to merge: ");
                int numFiles;
                scanf("%d", &numFiles);

                char *inputFiles[numFiles];
                for (int i = 0; i < numFiles; i++) {
                inputFiles[i] = (char *)malloc(1024 * sizeof(char));
                printf("Enter the path of file %d: ", i + 1);
                scanf("%s", inputFiles[i]);
                }

                mergeFiles("mergedFile.txt", inputFiles, numFiles);

                for (int i = 0; i < numFiles; i++) {
                    free(inputFiles[i]);
                }
                break;  
            }
        case 15:
            printf("You selected option 15\n");
            changeFilePath();
            break;
        case 0:
            exit(0);
            break;
        default:
            printf("Invalid option\n");
    }
}

int main() {
    

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
