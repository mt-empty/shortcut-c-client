/*
A command line client for [shortcut-pages](), written in C.

Created: 12/02/2020
Last modified: 20/03/2020
Author: mt-empty
*/

/* Feature test macro for realpath */
#define _DEFAULT_SOURCE

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <dirent.h>

int handleArgs(int argc, char *argv[]);
int getShortcutPage(char *filename);
int isValidShortcutPath(const char *path);
int getFileContent(char const *path, char **out);
int parseShortcutPage(char const *input);
int printFile(char const path[]);
int listPrograms();
void printVersion();
void printHelp();

#define VERSION "0.1.0"
#define BUFFER_SIZE 1000
#define PAGES_BASE_DIR "/opt/shortcut/pages/"
#define PAGES_FILE_EXT ".md"

#define ANSI_COLOR_RESET_FG "\x1b[39m"
#define ANSI_COLOR_TITLE_FG "\x1b[39m"
#define ANSI_COLOR_EXPLANATION_FG "\x1b[37m"
#define ANSI_COLOR_DESCRIPTION_FG "\x1b[37m"
#define ANSI_COLOR_SHORTCUT_FG "\x1b[32m"
#define ANSI_COLOR_CATEGORY_FG "\x1b[37m"
#define ANSI_BOLD_ON "\x1b[1m"
#define ANSI_BOLD_OFF "\x1b[22m"

static int help_flag = 0;
static int version_flag = 0;
static int list_flag = 0;
// static int verbose_flag;
// static int update_flag;
// static int platform_flag;

static struct option long_options[] = {
    {"help", no_argument, &help_flag, 1},
    {"version", no_argument, &version_flag, 1},
    {"list", no_argument, &list_flag, 1},
    // { "verbose", no_argument, &verbose_flag, 1 },
    // { "update", no_argument, &update_flag, 1 },
    // { "platform", required_argument, &platform_flag, 'p' },
};

int main(int argc, char *argv[])
{
    char *filename = (char *)malloc(sizeof(char *) * 25);

    if (handleArgs(argc, argv) != 0)
    {
        free(filename);
        return 1;
    }

    free(filename);
    exit(0);
}

int handleArgs(int argc, char *argv[])
{
    int c;
    // int digit_optind = 0;

    while (1)
    {
        // int this_option_optind = optind ? optind : 1;

        int option_index = 0;

        c = getopt_long(argc, argv, "hVl",
                        long_options, &option_index);
        if (c == -1)
            break;

        switch (c)
        {
        case 0:
            // fprintf(stdout, "option %s", long_options[option_index].name);
            // if (optarg)
            //     fprintf(stdout, " with arg %s", optarg);
            // fprintf(stdout, "\n");
            break;

        case 'h':
            help_flag = 1;
            break;

        case 'V':
            version_flag = 1;
            break;

        case 'l':
            list_flag = 1;
            break;

        case '?': //throws getopt library defined error
            break;
        default:
            abort();
        }
    }

    if (help_flag || argc == 1)
    {
        printHelp();
    }

    if (version_flag)
    {
        printVersion();
    }

    if (list_flag)
    {
        listPrograms();
    }

    if ((argc - optind) == 1) // optind is the index of the file argument in getOpt header file
    {
        return getShortcutPage(argv[optind]);
    }
    return EXIT_SUCCESS;
}

// prints shortcut page of a given program
int getShortcutPage(char *filename)
{
    // allocate memory for  the full path; making sure to not exceed
    // 24+BUFFER_SIZE
    int buf_size = strlen(PAGES_BASE_DIR) +
        ((strlen(filename) < BUFFER_SIZE) ? strlen(filename): BUFFER_SIZE) +
        strlen(PAGES_FILE_EXT) + 1;
    char* path = (char *) calloc(buf_size, sizeof(char));

    // don't prepend base dir if it's already provided
    if(strncmp(PAGES_BASE_DIR, filename, strlen(PAGES_BASE_DIR)-1) != 0) {
        strcat(path, PAGES_BASE_DIR);
    }
    // might need to add the length of path to filename when checking lenght against BUFFER_SIZE
    strncat(path,
            filename,
            (strlen(filename) < BUFFER_SIZE) ?
                strlen(filename) :
                (size_t) BUFFER_SIZE
            );

    // similarly don't append file extension if it's already given
    if(strlen(filename) < strlen(PAGES_FILE_EXT) ||
            strcmp(
                &filename[strlen(filename) - strlen(PAGES_FILE_EXT)],
                PAGES_FILE_EXT)
            != 0)  {
        strcat(path, PAGES_FILE_EXT);
    }

    if (printFile(path))
    {
        fprintf(stderr, "No page available for \"%s\"\n", filename);
        free(path);
        return EXIT_FAILURE;
    }
    free(path);
    return EXIT_SUCCESS;
}

int printFile(char const path[])
{
    char *output;

    // check if the path resides in PAGES_BASE_DIR
    if(!isValidShortcutPath(path)) {
        return 1;
    }

    if (!getFileContent(path, &output))
    {
        parseShortcutPage(output);
        free(output); // free memory that was malloc'd by getFileContent
        return 0;
    }
    free(output); // free memory that was malloc'd by getFileContent
    return 1;
}

/*
 * Checks if the given path is a real path, that is inside the PAGES_BASE_DIR
 *
 * Returns 1 if it is; 0 if it is not
 *
 */
int isValidShortcutPath(const char* path) {

    char* abspath = realpath(path, NULL);

    if(abspath) {
        int dirsAreEqual =  strncmp(PAGES_BASE_DIR, abspath, strlen(PAGES_BASE_DIR));

        free(abspath);
        return (dirsAreEqual == 0);
    }
    free(abspath);
    return 0;

}

int parseShortcutPage(char const *input)
{
    char c;
    int i, len;
    int start = -1;

    len = (int)strlen(input);

    fprintf(stdout, "\n");
    for (i = 0; i < len; ++i)
    {
        c = input[i];
        if (start == -1)
        {
            switch (c)
            {
            case '#':
                start = i;
                fprintf(stdout, "%s", ANSI_BOLD_ON);
                fprintf(stdout, "%s", ANSI_COLOR_TITLE_FG);
                continue;

            case '$':
                start = i;
                fprintf(stdout, "%s", ANSI_COLOR_CATEGORY_FG);
                continue;

            case '`':
                start = i;
                fprintf(stdout, "%s", ANSI_COLOR_SHORTCUT_FG);
                fprintf(stdout, "    ");
                continue;

            case '>':
                start = i;
                fprintf(stdout, "%s", ANSI_COLOR_EXPLANATION_FG);
                continue;
            }
        }
        else if (start > -1)
        {
            if (input[i] == '{' && input[i + 1] == '{')
            {
                fprintf(stdout, "%.*s", i - (start + 1), input + (start + 1));
                fprintf(stdout, "%s", ANSI_BOLD_OFF);
                fprintf(stdout, "%s", ANSI_COLOR_RESET_FG);
                fprintf(stdout, "%s", ANSI_COLOR_DESCRIPTION_FG);

                start = i;
                for (i = i + 1; i < len; i++)
                {
                    if (input[i] == '}' && input[i + 1] == '}')
                    {
                        fprintf(stdout, "%.*s", i - (start + 2),
                                input + (start + 2));
                        fprintf(stdout, "%s", ANSI_COLOR_RESET_FG);
                        fprintf(stdout, "%s", ANSI_COLOR_CATEGORY_FG);
                        start = i + 1;
                        break;
                    }
                }

                continue;
            }
        }

        if (c == '\n' && start > -1)
        {
            if (input[start] == '`')
            {
                fprintf(stdout, "%.*s", i - (start + 1), input + (start + 1));
            }
            else if (input[start] == '$')
            {
                fprintf(stdout, "\n%.*s", i - (start), input + (start + 2));
            }
            else if (input[start] == '#' || input[start] == '>')
            {
                fprintf(stdout, "%.*s", i - (start + 2), input + (start + 2));
            }
            else
            {
                fprintf(stdout, "%.*s", i - (start + 2), input + (start + 2));
            }

            fprintf(stdout, "%s", ANSI_BOLD_OFF);
            fprintf(stdout, "%s", ANSI_COLOR_RESET_FG);
            fprintf(stdout, "\n");
            start = -1;
        }
    }
    fprintf(stdout, "\n");
    return 0;
}

int getFileContent(char const *path, char **out)
{
    size_t len, read;
    FILE *fp;

    // ((void)verbose);
    *out = NULL;
    fp = fopen(path, "rb");
    if (!fp)
        return 1;

    if (fseek(fp, 0, SEEK_END))
        goto error;
    if ((len = (size_t)ftell(fp)) == (size_t)EOF)
        goto error;
    if (fseek(fp, 0, SEEK_SET))
        goto error;

    *out = (char *)calloc(len+1, sizeof(char));
    if (*out == NULL)
        goto error;

    read = fread(*out, 1, len, fp);
    if (read != len)
        goto error;

    fclose(fp);
    return 0;

error:
    fclose(fp);
    free(*out);
    *out = NULL;
    return 1;
}

int listPrograms()
{
    struct dirent *file;
    DIR *dir = opendir(PAGES_BASE_DIR);
    if (dir == NULL)  // opendir returns NULL if couldn't open directory 
    { 
        fprintf(stdout, "Could not open current directory" ); 
        return 0; 
    } 

    // Go through the program pages in order to list them
    while ((file = readdir(dir)) != NULL) {
        struct stat buf;
        char path[BUFFER_SIZE] = PAGES_BASE_DIR;
        strncat(path, file->d_name, BUFFER_SIZE); 

        if (lstat(path, &buf) == -1) {
            return EXIT_FAILURE;
        }

        if (S_ISREG(buf.st_mode)){ // Is the file not a symlink?
            printf("%.*s\n", strlen(file->d_name)-3, file->d_name); //Print everything except the last 3 characters

        }
    }
    closedir(dir); 

    return EXIT_SUCCESS;
}

void printVersion()
{
    fprintf(stdout, "%s \n", VERSION);
}

void printHelp()
{
    fprintf(stdout, "shortcut program [option]\n\n");
    fprintf(stdout, "Outputs shortcut for a given program\n\n");
    fprintf(stdout, "Options:\n");

    fprintf(stdout, "\t%-20s %-40s\n", "-V, --version", "print program version");
    fprintf(stdout, "\t%-20s %-40s\n", "-h, --help", "print help information");
    fprintf(stdout, "\t%-20s %-40s\n", "-l, --list", "list all available pages");
    // fprintf(stdout, "    %-20s %-30s\n", "-p, platform", "select platform, linux / osx / common");
}
