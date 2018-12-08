#ifndef COMMOM_H_
#define COMMOM_H_
#include <sys/sem.h>   
#define check_error(err)                                                 \
    do                                                                   \
    {                                                                    \
        if (err < 0)                                                     \
        {                                                                \
            fprintf(stderr, "错误为第%d行,错误码为%d\n", __LINE__, err); \
            exit(1);                                                     \
        }                                                                \
    } while (0);

#define check_NULL(err)                                             \
    do                                                              \
    {                                                               \
        if (err == NULL)                                            \
        {                                                           \
            fprintf(stderr, "错误为第%d行,不能为NULL\n", __LINE__); \
            exit(1);                                                \
        }                                                           \
    } while (0);

#define name "mutex"
#define MAX_SIZE 512
#define key 1024

typedef struct
{
    char text[MAX_SIZE];
} message;

#endif
