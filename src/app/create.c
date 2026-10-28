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

