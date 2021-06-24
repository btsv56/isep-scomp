typedef struct {
    int id;
    char name[50], address[100];
} User;

typedef struct {
    User records[100];
    int count;         /* record count */
    int writercount;   /* number of writers acting on struct */
    int readcount;     /* number of readers acting on struct */
} Shm_struct;