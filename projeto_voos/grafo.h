#ifndef GRAFO_H
#define GRAFO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <float.h>

#define MAX_AEROPORTOS  100
#define MAX_FILA        5000
#define INFINITO        FLT_MAX
#define FICHEIRO_DADOS  "dados.txt"
#define DURACAO_MINIMA  90      /* minutos — menor voo real em Angola */

/* ------------------------------------------------------------------ */
/*  Estruturas                                                          */
/* ------------------------------------------------------------------ */

typedef struct Aeroporto {
    char codigo[4];   /* IATA: 3 letras + '\0' */
    char nome[64];
    char cidade[48];
    char pais[48];
} Aeroporto;

typedef struct Voo {
    char        codigo[10];
    int         destino;
    float       custo;       /* Kuanzas (KZ) */
    float       distancia;   /* km           */
    int         duracao;     /* minutos       */
    struct Voo *proximo;
} Voo;

typedef struct Grafo {
    Aeroporto  aeroportos[MAX_AEROPORTOS];
    Voo       *adjacencias[MAX_AEROPORTOS];
    int        num_aeroportos;
} Grafo;

typedef struct NoHeap {
    int   indice;
    float peso;
} NoHeap;

typedef struct HeapMin {
    NoHeap nos[MAX_FILA];
    int    tamanho;
} HeapMin;

/* ------------------------------------------------------------------ */
/*  Protótipos                                                          */
/* ------------------------------------------------------------------ */

/* Grafo */
void  inicializar_grafo(Grafo *g);
void  libertar_grafo(Grafo *g);

/* Aeroportos */
int   inserir_aeroporto(Grafo *g, const char *codigo, const char *nome,
                        const char *cidade, const char *pais);
int   encontrar_aeroporto(const Grafo *g, const char *codigo);
void  listar_aeroportos(const Grafo *g);

/* Voos */
int   inserir_voo(Grafo *g, const char *cod, const char *origem,
                  const char *destino, float custo, float distancia,
                  int duracao);
/* retornos de inserir_voo:
 *   1  = OK
 *   0  = malloc falhou
 *  -1  = codigo de voo vazio
 *  -2  = IATA invalido
 *  -3  = origem == destino
 *  -4  = custo ou distancia <= 0
 *  -5  = duracao < DURACAO_MINIMA
 *  -6  = aeroporto nao encontrado
 *  -7  = codigo de voo ja existe nessa origem
 *  -8  = ja existe voo desta origem para este destino
 */
void  listar_voos(const Grafo *g);

/* Rotas */
void  rota_menor_custo(Grafo *g, const char *origem, const char *destino);
void  rota_menor_distancia(Grafo *g, const char *origem, const char *destino);
void  rota_menor_escalas(Grafo *g, const char *origem, const char *destino);
void  mostrar_rede(const Grafo *g);

/* Persistência */
int   guardar_dados(const Grafo *g, const char *ficheiro);
int   carregar_dados(Grafo *g, const char *ficheiro);
void  carregar_dados_exemplo(Grafo *g);

/* Validação */
int   validar_iata(const char *codigo);

#endif /* GRAFO_H */
