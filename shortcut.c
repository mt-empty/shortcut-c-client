/*
A command line client for [shortcut-pages](), written in C.

Created: 12/02/2020
Last modified: 20/03/2020
Author: mt-empty
*/

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

#include "cJSON.h"

int handleArgs(int argc, char *argv[]);
int getShortcutPage(char *filename);
int getFileContent(char const *path, char **out);
int parseShortcutPage(char const *input);
int printFile(char const path[]);
// int listPrograms();
void printVersion();
void printHelp();
int parseJsonPage(const char* const input);

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
// static int list_flag = 0;
// static int verbose_flag;
// static int update_flag;
// static int platform_flag;

static struct option long_options[] = {
    {"help", no_argument, &help_flag, 1},
    {"version", no_argument, &version_flag, 1},
    // {"list", no_argument, &list_flag, 1},
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

        c = getopt_long(argc, argv, "hV",
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

        // case 'l':
        //     list_flag = 1;
        //     break;

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

    // if (list_flag)
    // {
    //     listPrograms();
    // }

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

    strcat(path, PAGES_BASE_DIR);
    strncat(path,
            filename,
            (strlen(filename) < BUFFER_SIZE) ?
                strlen(filename) :
                (size_t) BUFFER_SIZE
            );
    strcat(path, PAGES_FILE_EXT);

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

    if (!getFileContent(path, &output))
    {
        parseShortcutPage(output);
        free(output); // free memory that was malloc'd by getFileContent
        return 0;
    }
    free(output); // free memory that was malloc'd by getFileContent
    return 1;
}


int parseJsonPage(const char* const input){
    const cJSON *name = NULL;
    const cJSON *metadata = NULL;
    const cJSON *sourceUrl = NULL;
    const cJSON *aliases = NULL;
    const cJSON *alias = NULL;
    // const cJSON *description = NULL;
    const cJSON *section_order = NULL;
    const cJSON *sections = NULL;
    const cJSON *sectionName = NULL;
    const cJSON *section = NULL;
    const cJSON *shrtDescPair = NULL;

    int status = 0;

    cJSON *page_json = cJSON_Parse(input);
    if (page_json == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            fprintf(stderr, "JSON file error. Error before: %s\n", error_ptr);
        }
        status = 0;
        goto end;
    }

    name = cJSON_GetObjectItemCaseSensitive(page_json, "name");
    metadata = cJSON_GetObjectItemCaseSensitive(page_json, "metadata");
    sourceUrl = cJSON_GetObjectItemCaseSensitive(metadata, "sourceUrl");

    aliases = cJSON_GetObjectItemCaseSensitive(page_json, "aliases");

    if (cJSON_IsString(name) && (name->valuestring != NULL))
    {
        fprintf(stdout, "%s%s%s\n", ANSI_BOLD_ON, name->valuestring, ANSI_BOLD_OFF);
    }

    if (cJSON_IsString(sourceUrl) && (sourceUrl->valuestring != NULL))
    {
        fprintf(stdout, "Source: %s\n", sourceUrl->valuestring);
    }

    if (cJSON_IsArray(aliases))
    {
        fprintf(stdout, "Aliases: ");

        cJSON_ArrayForEach(alias, aliases){
            fprintf(stdout, "%s, ",alias->valuestring);
        }
        fprintf(stdout, "\n");

    }

    fprintf(stdout, "\n");

    section_order = cJSON_GetObjectItemCaseSensitive(page_json, "section_order");
    sections = cJSON_GetObjectItemCaseSensitive(page_json, "sections");

    cJSON_ArrayForEach(sectionName, section_order)
    {
        fprintf(stdout, "%s\n\n", sectionName->valuestring);
        
        section = cJSON_GetObjectItemCaseSensitive(sections, sectionName->valuestring);
        cJSON_ArrayForEach(shrtDescPair, section)
        {

            cJSON *val = cJSON_GetObjectItemCaseSensitive(shrtDescPair, "val");
            cJSON *key = cJSON_GetObjectItemCaseSensitive(shrtDescPair, "key");

            fprintf(stdout, "\t%s%s \t%s\t %s\n", ANSI_COLOR_SHORTCUT_FG, key->valuestring, ANSI_COLOR_RESET_FG, val->valuestring );
        }
        fprintf(stdout, "\n");
    }

end:
    cJSON_Delete(page_json);
    return status;
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

// int listPrograms()
// {
//     char path[50] = "programs.md";
//     if (printFile(path))
//     {
//         fprintf(stderr, "No pages available\n");
//         exit(EXIT_FAILURE);
//     }
//     return EXIT_SUCCESS;
// }

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
    // fprintf(stdout, "    %-20s %-30s\n", "-p, platform", "select platform, linux / osx / common");
}
