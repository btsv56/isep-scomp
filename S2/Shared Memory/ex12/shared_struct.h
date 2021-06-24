#define SHARED_NAME "/shmtest"
#define STR_SIZE 50
#define NR_DISC 10

struct aluno {
    int numero;
    char nome[STR_SIZE];
    int disciplinas[NR_DISC];
};

typedef struct {
    struct aluno a;
    int new_data;
} shared_data_type;