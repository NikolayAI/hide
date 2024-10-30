#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include <pwd.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argv[1] == NULL) {
        puts("You should pass file path as argument");
        exit(1);
    }

    struct stat dark_dir_stat;
    const int dark_dir_permissions = 16585;
    const char* dark_dir_name = "darkcatalog";
    const char* dark_dir_parent = "/home/";
    const char* divider = "/";
    int uid = geteuid();
    struct passwd* user = getpwuid(uid);

    if (user == NULL) {
        puts("Cant get username");
        exit(1);
    }

    char dark_dir_path[strlen(dark_dir_parent) + strlen(user->pw_name) + strlen(divider) + strlen(dark_dir_name)];
    char file_move_dest[sizeof(dark_dir_path) / sizeof(dark_dir_path[0]) + strlen(divider) + strlen(argv[1])];

    sprintf(dark_dir_path, "%s%s%s%s",dark_dir_parent, user->pw_name, divider, dark_dir_name);
    sprintf(file_move_dest, "%s%s%s",dark_dir_path, divider, argv[1]);

    if (mkdir(dark_dir_path, dark_dir_permissions) != 0) {
        bool is_file_exists = errno == EEXIST;
        bool is_directory = stat(dark_dir_path, &dark_dir_stat) == 0 && S_ISDIR(dark_dir_stat.st_mode);

        if (is_file_exists && !is_directory) {
            perror("Existed file is not a directory");
            exit(1);
        } else if (!is_file_exists) {
            perror("Error create darkcatalog");
            exit(1);
        }
    }

    if (dark_dir_stat.st_mode != dark_dir_permissions && chmod(dark_dir_path, dark_dir_permissions) != 0) {
        perror("Error set darkcatalog permissions");
        exit(1);
    }

    if(rename(argv[1], file_move_dest) != 0) {
        perror("Error move file to darkcatalog");
        exit(1);
    };

    return 0;
}
