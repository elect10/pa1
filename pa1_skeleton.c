#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <grp.h>
#include <pwd.h>
#include <time.h>
#include <locale.h>



// 문자열 비교 함수
int stringcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

int compare(const void *a, const void *b) {
    return stringcmp(*(const char **)a, *(const char **)b);
}

char *str_chr(const char *s, int c) {
    while (*s) {
        if (*s == (char)c) {
            return (char *)s;
        }
        s++;
    }
    return NULL;
}

char *last;

char *str_tok(char *str, const char *delim) {
    if (str != NULL) {
        last = str;
    }
    if (last == NULL) {
        return NULL;
    }

    char *start = last;
    while (*start && str_chr(delim, *start)) {
        start++;
    }
    if (*start == '\0') {
        last = NULL;
        return NULL;
    }

    char *token = start;
    while (*last && !str_chr(delim, *last)) {
        last++;
    }

    if (*last) {
        *last = '\0';
        last++;
    } else {
        last = NULL;
    }

    return token;
}




int ls(char *dir_path, char *option);
int head(char *file_path, char *line);
int tail(char *file_path, char *line);
int mv(char *file_path1, char *file_path2);
int cp(char *file_path1, char *file_path2);
int pwd();

int main() {
    while (1) {
        int i, cmdrlt;
        char *cmd = NULL;
        char *argument[10];
        size_t size;

        

        getline(&cmd, &size, stdin);
        
        i = 0;
        argument[i] = str_tok(cmd, " \n");

        while (argument[i] != NULL && i < 9) {
            argument[++i] = str_tok(NULL, " \n");
        }
        if (stringcmp("ls", argument[0]) == 0){
			cmdrlt = ls(argument[1], argument[2]);		
		}
		else if (stringcmp("head", argument[0]) == 0){ 
			cmdrlt = head(argument[3], argument[2]);
		}
		else if (stringcmp("tail", argument[0]) == 0){
			cmdrlt = tail(argument[3], argument[2]);
		}
		else if (stringcmp("mv", argument[0]) == 0){
			cmdrlt = mv(argument[1], argument[2]);
		}
		else if (stringcmp("cp", argument[0]) == 0){
			cmdrlt = cp(argument[1], argument[2]);
		}
		else if (stringcmp("pwd", argument[0]) == 0){
			cmdrlt = pwd();
		}
		else if (stringcmp("quit", argument[0]) == 0){
			break;
		} else {
            printf("ERROR: invalid command\n");
        }

        if (cmdrlt == -1) {
            printf("ERROR: The command is executed abnormally\n");
        }
        printf("\n");
        free(cmd);
    }
    return 0;
}
//ls -al
void print_file_permissions(mode_t mode) {
    char type = '?';
    if (S_ISREG(mode)) type = '-';
    else if (S_ISDIR(mode)) type = 'd';
    else if (S_ISCHR(mode)) type = 'c';
    else if (S_ISBLK(mode)) type = 'b';
    else if (S_ISFIFO(mode)) type = 'p';
    else if (S_ISLNK(mode)) type = 'l';
    else if (S_ISSOCK(mode)) type = 's';

    printf("%c", type);

    printf("%c%c%c", (mode & S_IRUSR) ? 'r' : '-', (mode & S_IWUSR) ? 'w' : '-', (mode & S_IXUSR) ? 'x' : '-');
    printf("%c%c%c", (mode & S_IRGRP) ? 'r' : '-', (mode & S_IWGRP) ? 'w' : '-', (mode & S_IXGRP) ? 'x' : '-');
    printf("%c%c%c", (mode & S_IROTH) ? 'r' : '-', (mode & S_IWOTH) ? 'w' : '-', (mode & S_IXOTH) ? 'x' : '-');
}

int ls(char *dir_path, char *option) {
    struct dirent *entry;
    DIR *dp = opendir(dir_path ? dir_path : ".");
    if (!dp) {
        printf("ERROR: invalid path\n");
        return 0;
    }
    if(option && stringcmp(option, "-al") != 0) {
        return -1;
    }

    // 파일 이름을 저장할 배열 및 카운트 변수
    char *file_names[1024];
    int count = 0;
    int total_blocks = 0;
    
    // 파일 이름을 배열에 저장
    while ((entry = readdir(dp)) != NULL) {
        int len = 0;
        while (entry->d_name[len] != '\0') len++;  // 파일 이름 길이 계산

        // 파일 이름을 위한 메모리 할당 및 복사
        file_names[count] = (char *)malloc((len + 1) * sizeof(char));
        for (int i = 0; i <= len; i++) {
            file_names[count][i] = entry->d_name[i];
        }

        // 파일 블록 수를 누적
        struct stat file_stat;
        char path[1024];
        snprintf(path, sizeof(path), "%s/%s", dir_path ? dir_path : ".", entry->d_name);
        if (stat(path, &file_stat) == 0) {
            total_blocks += file_stat.st_blocks;
        }

        count++;
    }

    closedir(dp);
    
    
    if (option && stringcmp(option, "-al") == 0) {
        printf("total %d\n", total_blocks / 2);
    }

    // 파일 이름 배열 정렬
    qsort(file_names, count, sizeof(char *), compare);

    // 정렬된 파일 이름 출력
    for (int i = 0; i < count; i++) {
        if (option && stringcmp(option, "-al") == 0) {
            struct stat file_stat;
            char path[1024];
            
            snprintf(path, sizeof(path), "%s/%s", dir_path ? dir_path : ".", file_names[i]);
            
            if (stat(path, &file_stat) == 0) {
            
                // 파일 타입과 권한 출력
                print_file_permissions(file_stat.st_mode);

                // 링크 수, 소유자, 그룹, 파일 크기, 수정 시간, 파일 이름 출력
                printf("%2ld ", file_stat.st_nlink);

                struct passwd *pw = getpwuid(file_stat.st_uid);
                struct group *gr = getgrgid(file_stat.st_gid);

                // NULL 확인 후 대체 출력
                if (pw == NULL) {
                    printf("%d ", file_stat.st_uid);  // UID를 숫자로 출력
                } else {
                    printf("%s ", pw->pw_name);
                }

                if (gr == NULL) {
                    printf("%d ", file_stat.st_gid);  // GID를 숫자로 출력
                } else {
                    printf("%s ", gr->gr_name);
                }

                // 파일 크기 출력 추가
                printf("%6ld ", file_stat.st_size);

                // 수정 시간 출력
                char time_buf[20];
                strftime(time_buf, sizeof(time_buf), "%b %d %H:%M", localtime(&file_stat.st_mtime));
                printf("%s %s\n", time_buf, file_names[i]);
            }
        } else {
            if (file_names[i][0] != '.') {
                printf("%s ", file_names[i]);
            }
        }
        free(file_names[i]);  // 메모리 해제
    }
    printf("\n");
    return 0;
}

int head(char *file_path, char *line) {
    int fd = open(file_path, O_RDONLY);
    if (fd == -1) {
        printf("ERROR: invalid path\n");
        return 0;
    }

    int n = atoi(line); // 줄 수 파싱
    char buffer[1024];
    ssize_t bytes;
    int line_count = 0;

    while (line_count < n && (bytes = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes] = '\0'; // NULL-terminate the buffer
        char *buf_ptr = buffer;
        while (*buf_ptr) {
            if (line_count >= n) break;
            putchar(*buf_ptr);
            if (*buf_ptr++ == '\n') {
                line_count++;
            }
        }
    }

    close(fd);
    return 0;
}



int tail(char *file_path, char *line) {
    int fd = open(file_path, O_RDONLY); // Open the file for reading only.
    if (fd == -1) {
        perror("ERROR: invalid path");
        return 1;
    }

    int n = atoi(line); // Number of lines to output.
    off_t pos = lseek(fd, 0, SEEK_END); // Move to the end of the file.
    if (pos == -1) {
        return -1;
    }

    int line_count = 0;
    char c;

    // Read backwards from the end of the file to find the nth line.
    while (pos > 0 && line_count <= n) {
        pos = lseek(fd, --pos, SEEK_SET); // Move one byte left.
        if (pos == -1) {
            return -1;
        }

        if (read(fd, &c, 1) != 1) {
            return -1;
        }

        if (c == '\n') {
            line_count++; // Increment the line count when a newline is found.
        }
    }

    // Start reading from the found position.
    if (pos > 0) {
        lseek(fd, pos + 1, SEEK_SET); // Skip the newline character.
    } else {
        lseek(fd, 0, SEEK_SET); // Move to the beginning of the file.
    }

    // Output the lines from the current position to the end of the file.
    char buffer[1024];
    ssize_t bytes_read;
    while ((bytes_read = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0'; // Null-terminate the string.
        printf("%s", buffer);
    }

    close(fd); // Close the file descriptor.
    return 0;
}

int mv(char *file_path1, char *file_path2) {
    if (rename(file_path1, file_path2) == -1) {
        printf("ERROR: invalid path\n");
        return 0;
    }
    return 0;
}

int cp(char *file_path1, char *file_path2) {
    int src = open(file_path1, O_RDONLY);
    if (src == -1) {
        printf("ERROR: invalid path\n");
        return 0;
    }

    int dest = open(file_path2, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dest == -1) {
        printf("ERROR: invalid path\n");
        close(src);
        return 0;
    }

    char buffer[1024];
    ssize_t bytes;
    while ((bytes = read(src, buffer, sizeof(buffer))) > 0) {
        write(dest, buffer, bytes);
    }

    close(src);
    close(dest);
    return 0;
}

int pwd() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
        return 0;
    } else {
        return -1;
    }
}

