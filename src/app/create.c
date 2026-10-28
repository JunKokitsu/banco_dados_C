#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#define DATA_DIR "../../data"
#define ARQUIVO_TABELAS "../../data/tabelas.dat"
#define MAX_NOME 50
#define MAX_COLUNAS 20
#define MAX_CAMINHO 256

typedef enum {
    TIPO_INT = 1,
    TIPO_FLOAT,
    TIPO_DOUBLE,
    TIPO_CHAR,
    TIPO_STRING
} TipoColuna;

typedef enum {
    CHAVE_NENHUMA = 0,
    CHAVE_PRIMARIA = 1,
    CHAVE_ESTRANGEIRA = 2
} TipoChave;

typedef struct {
    char nome[MAX_NOME];
    TipoColuna tipo;
    int tamanho;
    TipoChave tipo_chave;
} Coluna;

typedef struct {
    char nome[MAX_NOME];
    int quantidade_colunas;
    Coluna colunas[MAX_COLUNAS];
} EsquemaTabela;

typedef struct {
    char nome[MAX_NOME];
    int id;
} RegistroTabela;

int arquivo_existe(const char *caminho) {
    struct stat st;
    return stat(caminho, &st) == 0 && S_ISREG(st.st_mode);
}

int diretorio_existe(const char *caminho) {
    struct stat st;
    return stat(caminho, &st) == 0 && S_ISDIR(st.st_mode);
}

void limpar_newline(char *texto) {
    texto[strcspn(texto, "\n")] = '\0';
}

int criar_diretorio(const char *caminho) {
    printf("Criando diretorio: %s\n", caminho);

    if (mkdir(caminho, 0777) == 0) {
        return 1;
    }

    return 0;
}

int garantir_data_dir(void) {
    if (diretorio_existe(DATA_DIR)) {
        return 1;
    }

    if (mkdir(DATA_DIR, 0777) == 0) {
        return 1;
    }

    return 0;
}

