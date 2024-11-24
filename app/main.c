#include <stdio.h>

/*int main() {
   printf("$ Input string:  ");
   fflush(stdout);
  // Wait for user input
  char input[100];
  fgets(input, 100, stdin);


  input[strlen(input) - 1] = '\0';

  printf("%s: command not found\n", input);
  return 0;
}*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/mount.h>
#include <stdint-gcc.h>
#include <fcntl.h>

// #define S_IFMT  0xF000   
// #define S_IFREG 0x8000
// #define _GNU_SOURCE

#define BUFFER_SIZE 1024 //буфер для ввода
#define HISTORY_SIZE 1000 //для истории
#define HISTORY_FILE "history.txt"

// Функция для записи истории команд в файл
void save_history_to_file(char history[][BUFFER_SIZE], int count) {
    FILE *file = fopen(HISTORY_FILE, "a");
    if (file == NULL) {
        perror("Не удалось открыть файл для записи истории");
        return;
    }
    for (int i = 0; i < count; i++) {
        fprintf(file, "%s\n", history[i]);
    }
    fclose(file);
}

//8 чтение бинарника
void read_binary(const char *filename){
  FILE *file = fopen(filename, "rb");
  if (file == NULL){
    perror ("Не удалось открыть бинарный файл");
    return;
  }
  // определяем размер файла
  fseek(file, 0, SEEK_END);
  long file_s = ftell(file);
  rewind(file); // устанавливает указатель на начло файла
  
  unsigned char *buffer = (unsigned char *)malloc(file_s);
  if (buffer == NULL){
    perror("Ошибка выделения памяти");
    fclose(file);
    return;
  }
  
  size_t count = fread(buffer, 1, file_s, file);
  if (count != file_s){
    perror("ошибка чтения данных");
    free(buffer); //освобождаем память
    fclose(file);
    return;
  }
  
  //выводд
  printf("содержимое бинарного файла %s:\n", filename);
  for (long i=0;i<file_s;i++){
    printf("%02X ", buffer[i]);
  }
  printf("\n");
  free(buffer);
  fclose(file);
}

void handle_SIGHUP(int signal){
  if (signal == SIGHUP){
    printf("The program interrupted\n");
    exit(0);
  }
}

// 10 определить является ли диск загрузочным
// Функция для проверки загрузочного диска
void is_bootable_device(char* device_name) {
    // Удаляем лишние пробелы
    while (*device_name == ' ') {
        device_name++;
    }

    // Формируем полный путь к устройству
    char full_path[128];
    snprintf(full_path, sizeof(full_path), "/dev/%s", device_name);

    printf("path: %s\n", full_path);

    // Открываем устройство для чтения
    FILE* device_file = fopen(full_path, "rb");
    if (device_file == NULL) {
        printf("Не удалось открыть диск %s!\n", device_name);
        //return;
    }

    // Переходим к 510 байту
    if (fseek(device_file, 510, SEEK_SET) != 0) {
        printf("Ошибка при смещении к сигнатуре на диске!\n");
        fclose(device_file);
        return;
    }

    // Считываем последние два байта
    uint8_t signature[2];
    if (fread(signature, 1, 2, device_file) != 2) {
        printf("Ошибка при чтении сигнатуры диска!\n");
        fclose(device_file);
        return;
    }
    fclose(device_file);

    // Проверка сигнатуры на 55 AA
    if (signature[0] == 0x55 && signature[1] == 0xAA) {
        printf("Диск %s является загрузочным.\n", device_name);
    } else {
        printf("Диск %s не является загрузочным.\n", device_name);
    }
}

//12
void makeDump(DIR* dir, char* path) {
    FILE* res = fopen("res.txt", "w+");
    fclose(res);
    struct dirent* ent;
    char* file_path;
    while ((ent = readdir(dir)) != NULL) {

        asprintf(&file_path, "%s/%s", path, ent->d_name); // asprintf работает
        if(!appendToFile("res.txt", file_path)) {
            return;
        }
    }
    printf("Dump completed!\n");
}

// //cron
// bool appendToFile(char* path1, char* path2) {
//     FILE *f1 = fopen(path1, "a");
//     FILE *f2 = fopen(path2, "r");
//     if (!f1 || !f2) {
//         printf("Error while reading file %s\n", path2);
//         return false;
//     }
//     char buf[256];

//     while (fgets(buf, 256, f2) != NULL) {
//         fputs(buf, f1);
//     }
//     fclose(f1);
//     fclose(f2);
//     return true;
// }

// char* concat(const char *s1, const char *s2) {
//     char *result = malloc(strlen(s1) + strlen(s2) + 1);
//     strcpy(result, s1);
//     memset (s1,'\0',sizeof(s1));
//     strcat(result, s2);
//     return result;
// }



// bool stringEquals(const char* string1, const char* string2) {
//     return !strcmp(string1, string2);
// }

// const char* vfs = "/tmp/cronvfs";
// const char* crontab = "/var/spool/cron/crontabs";

// void mountVFS(char* SOURCE) {
//     char* MOUNT_POINT = "/tmp/vfs";
//     struct stat st;
//     if (stat(MOUNT_POINT, &st) == -1) {
//         if (mkdir(MOUNT_POINT, 0755) == -1) {
//             printf("Ошибка при создании каталога\n");
//             return;
//         }
//     }

//     // Монтируем VFS
//     if (mount(SOURCE, MOUNT_POINT, "vfs", 0, NULL) == -1) {
//         printf("Ошибка при монтировании VFS\n");
//         return;
//     }

//     printf("VFS успешно смонтирован в %s\n", MOUNT_POINT);
//     return;
// }

// static void cron_create(int i, char *path, char *str) {
//     FILE *f;
//     char *filename;

//     sprintf(filename, "task%d", i);
//     concat(path, filename);
//     free(filename);

//     f = fopen(path, "w");
//     if (f) {
//         fprintf(f, "%s\n", str);
//         fclose(f);
//     }

// }


// static void cron_list(const char *filename, char *path) {
//     int i = 0;
//     int c;
//     char* str;
//     FILE *f = fopen(filename, "r");
//     if (!f) {
//         perror(filename);
//         return;
//     }

//     while ((c = fgetc(f)) != EOF) {
//         if (c != '\n') {
//             concat(&str, (const char *) c);
//             continue;
//         }
//         if (strlen(str) && str[0] != '#') {
//             cron_create(i, path, str);
//             i++;
//         }
//         str = "";
//     }
//     fclose(f);
// }


// void cron_mount() {
//     int mr;

//     mr = mkdir(vfs, 0777);
//     if (mr == -1) {
//         perror(vfs);
//         return;
//     }

//     mr = mount("tmpfs", vfs, "tmpfs", 0, NULL);
//     if (mr == -1) {
//         perror("mount");
//         return;
//     }
// }

// void cron_umount() {
//     umount(vfs);
//     rmdir(vfs);
// }

// void cron() {
//     DIR *dir;
//     struct dirent *dirent;
//     char* path = crontab;
//     char* vfs_path = vfs;

//     dir = opendir(crontab);
//     if (!dir) {
//         perror(crontab);
//         return;
//     }

//     while ((dirent = readdir(dir)) != NULL) {
//         struct stat buf;

//         asprintf(path, "%s", dirent->d_name);

//         lstat(path, &buf);
//         if ((buf.st_mode & S_IFMT) == S_IFREG) {
//             cron_list(path, &vfs_path);
//         }

//     }

//     closedir(dir);
// }

int main() {
    
    char input[BUFFER_SIZE];               // Буфер для ввода команд
    char history[HISTORY_SIZE][BUFFER_SIZE]; // История команд
    int history_count = 0;                  // Счётчик команд в истории
    signal(SIGHUP, handle_SIGHUP);
    do {  
        bool f = false;  
        printf("ALINA$ ");
        fflush(stdout);
        // Чтение ввода с клавиатуры, проверка на EOF (Ctrl+D)
        if (fgets(input, BUFFER_SIZE, stdin) == NULL) {
            printf("\nЗавершение работы (Ctrl+D)\n");
            break; // выход из цикла при EOF
        }

        // Убираем символ новой строки, если он есть
        input[strcspn(input, "\n")] = '\0';

        // Проверяем на команды выхода
        if (strcmp(input, "exit") == 0 || strcmp(input, "\\q") == 0) {
            printf("Завершение работы (exit/\\q)\n");
            break;
        }

        // Сохраняем введённую команду в историю (если есть место)
        if (history_count < HISTORY_SIZE) {
            strcpy(history[history_count], input);
            history_count++;
        }
        if (strcmp(input, "echo $PATH") == 0){
            char *path = getenv("PATH");
            if (path != NULL){
                printf("%s\n", path);
            } else{
                printf("Переменная PATH не найдена\n");
            }
            f = true;
            continue;
        }
        //чтение бинарника
        if (strncmp(input, "cat ", 4) == 0){
            read_binary(input + 4);
            f = true;
            continue;
        }
        //выполнение бинарника
        if (strncmp(input, "run ", 4) == 0){
            pid_t p = fork();
            if (p == 0){
              char *argv[] = { "sh", "-c", input + 4, 0 };
              execvp(argv[0], argv);
              fprintf(stderr, "Failed to exec shell on %s\n", input + 4);
            
              f = true;
              exit(1);
              //continue;
            }
            
        }
        
        // 10 определить является ли диск загрузочным
        if (strncmp(input, "\\l", 2) == 0) {
            // Проверка, является ли диск загрузочным

            char* device_name = input + 3; // Извлечение имени устройства, например, "sda"

            is_bootable_device(device_name);

        }
        
        // Проверяем команду echo
        if (strncmp(input, "echo ", 5) == 0) {
            printf("%s\n", input + 5); // Выводим всё, что после "echo "
            f = true;
            continue;
        }

        if(strncmp(input, "cron", 4) == 0) {
            printf("Подключение vfs с планировщиком\n");
            mountVFS(input);
            f=true;
            continue;
        }


        if (f == false){
          printf("There is no such command!\n");
          
        }
        
        
        
        
    }
    while (!feof(stdin));
    // Сохраняем историю команд в файл перед выходом
    save_history_to_file(history, history_count);

    return 0;
}

//execv - бинарник
//fork
