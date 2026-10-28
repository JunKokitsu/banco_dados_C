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

int verifica_tabela_registrada(const char *nome_tabela) {
    FILE *arquivo;
    RegistroTabela registro;

    arquivo = fopen(ARQUIVO_TABELAS, "rb");
    if (arquivo == NULL) {
        arquivo = fopen(ARQUIVO_TABELAS, "wb");
        if (arquivo == NULL) {
            printf("Erro ao criar arquivo de registros das tabelas.\n");
            return -1;
        }

        fclose(arquivo);
        return 0;
    }

    while (fread(&registro, sizeof(RegistroTabela), 1, arquivo) == 1) {
        if (strcmp(registro.nome, nome_tabela) == 0) {
            fclose(arquivo);
            return registro.id;
        }
    }

    fclose(arquivo);
    return 0;
}

int obter_proximo_id_tabela(void) {
    FILE *arquivo;
    RegistroTabela registro;
    int ultimo_id = 0;

    arquivo = fopen(ARQUIVO_TABELAS, "rb");
    if (arquivo == NULL) {
        return 1;
    }

    while (fread(&registro, sizeof(RegistroTabela), 1, arquivo) == 1) {
        ultimo_id = registro.id;
    }

    fclose(arquivo);
    return ultimo_id + 1;
}

int registra_tabela(const char *nome_tabela) {
    FILE *arquivo;
    RegistroTabela registro;

    arquivo = fopen(ARQUIVO_TABELAS, "ab");
    if (arquivo == NULL) {
        return 0;
    }

    memset(&registro, 0, sizeof(RegistroTabela));
    strncpy(registro.nome, nome_tabela, MAX_NOME - 1);
    registro.nome[MAX_NOME - 1] = '\0';
    registro.id = obter_proximo_id_tabela();

    if (fwrite(&registro, sizeof(RegistroTabela), 1, arquivo) != 1) {
        fclose(arquivo);
        return 0;
    }

    fclose(arquivo);
    return 1;
}

TipoColuna escolher_tipo(void) {
    int opcao;

    while (1) {
        printf("Tipo da coluna:\n");
        printf("1. INT\n");
        printf("2. FLOAT\n");
        printf("3. DOUBLE\n");
        printf("4. CHAR\n");
        printf("5. STRING\n");
        printf("Opcao: ");

        if (scanf("%d", &opcao) != 1) {
            printf("Opcao invalida.\n");
            while (getchar() != '\n');
            continue;
        }

        getchar();

        switch (opcao) {
            case 1: return TIPO_INT;
            case 2: return TIPO_FLOAT;
            case 3: return TIPO_DOUBLE;
            case 4: return TIPO_CHAR;
            case 5: return TIPO_STRING;
            default:
                printf("Opcao invalida.\n");
        }
    }
}

TipoChave escolher_tipo_chave_coluna(void) {
    int opcao;

    while (1) {
        printf("Definicao da coluna:\n");
        printf("0. Nenhuma\n");
        printf("1. Chave primaria\n");
        printf("2. Chave estrangeira\n");
        printf("Opcao: ");

        if (scanf("%d", &opcao) != 1) {
            printf("Opcao invalida.\n");
            while (getchar() != '\n');
            continue;
        }

        getchar();

        switch (opcao) {
            case 0: return CHAVE_NENHUMA;
            case 1: return CHAVE_PRIMARIA;
            case 2: return CHAVE_ESTRANGEIRA;
            default:
                printf("Opcao invalida.\n");
        }
    }
}

int existe_chave_primaria(EsquemaTabela *tabela) {
    for (int i = 0; i < tabela->quantidade_colunas; i++) {
        if (tabela->colunas[i].tipo_chave == CHAVE_PRIMARIA) {
            return 1;
        }
    }
    return 0;
}

void definir_chave_coluna(EsquemaTabela *tabela, Coluna *coluna) {
    coluna->tipo_chave = escolher_tipo_chave_coluna();

    if (coluna->tipo_chave == CHAVE_PRIMARIA) {
        if (existe_chave_primaria(tabela)) {
            printf("A tabela ja possui uma chave primaria. Cadastrando coluna normal.\n");
            coluna->tipo_chave = CHAVE_NENHUMA;
            return;
        }
    }
}

void adicionar_colunas(EsquemaTabela *tabela) {
    char entrada[MAX_NOME];

    while (tabela->quantidade_colunas < MAX_COLUNAS) {
        printf("Nome da coluna (ou ] para encerrar): ");

        if (fgets(entrada, sizeof(entrada), stdin) == NULL) {
            break;
        }

        limpar_newline(entrada);

        if (strcmp(entrada, "]") == 0) {
            break;
        }

        if (strlen(entrada) == 0) {
            printf("Nome invalido.\n");
            continue;
        }

        Coluna *coluna = &tabela->colunas[tabela->quantidade_colunas];
        memset(coluna, 0, sizeof(Coluna));

        strncpy(coluna->nome, entrada, MAX_NOME - 1);
        coluna->nome[MAX_NOME - 1] = '\0';

        coluna->tipo = escolher_tipo();
        definir_chave_coluna(tabela, coluna);

        switch (coluna->tipo) {
            case TIPO_INT:
                coluna->tamanho = sizeof(int);
                break;
            case TIPO_FLOAT:
                coluna->tamanho = sizeof(float);
                break;
            case TIPO_DOUBLE:
                coluna->tamanho = sizeof(double);
                break;
            case TIPO_CHAR:
                coluna->tamanho = sizeof(char);
                break;
            case TIPO_STRING:
                printf("Tamanho maximo da STRING: ");
                if (scanf("%d", &coluna->tamanho) != 1) {
                    coluna->tamanho = 50;
                }
                getchar();

                if (coluna->tamanho <= 0) {
                    coluna->tamanho = 50;
                }
                break;
        }

        tabela->quantidade_colunas++;
    }
}

