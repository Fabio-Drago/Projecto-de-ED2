#include "grafo.h"

/*  Heap minimo (Dijkstra)                                             */

static void trocar(NoHeap *a, NoHeap *b) {
    NoHeap t = *a; *a = *b; *b = t;
}

static void subir(HeapMin *h, int i) {
    while (i > 0) {
        int pai = (i - 1) / 2;
        if (h->nos[pai].peso <= h->nos[i].peso) break;
        trocar(&h->nos[pai], &h->nos[i]);
        i = pai;
    }
}

static void descer(HeapMin *h, int i) {
    for (;;) {
        int menor = i;
        int esq   = 2 * i + 1;
        int dir   = 2 * i + 2;
        if (esq < h->tamanho && h->nos[esq].peso < h->nos[menor].peso) menor = esq;
        if (dir < h->tamanho && h->nos[dir].peso < h->nos[menor].peso) menor = dir;
        if (menor == i) break;
        trocar(&h->nos[i], &h->nos[menor]);
        i = menor;
    }
}

static void push_heap(HeapMin *h, int indice, float peso) {
    if (h->tamanho >= MAX_FILA) return;
    h->nos[h->tamanho].indice = indice;
    h->nos[h->tamanho].peso   = peso;
    subir(h, h->tamanho++);
}

static NoHeap pop_heap(HeapMin *h) {
    NoHeap topo = h->nos[0];
    h->nos[0]   = h->nos[--h->tamanho];
    descer(h, 0);
    return topo;
}

/*  Validacao                                                          */

/* Retorna 1 se o codigo IATA e valido: exactamente 3 letras maiusculas */
int validar_iata(const char *codigo) {
    int i;
    if (!codigo) return 0;
    for (i = 0; i < 3; i++) {
        if (!isupper((unsigned char)codigo[i])) return 0;
    }
    if (codigo[3] != '\0') return 0;
    return 1;
}

/*  Inicializacao e libertacao                                         */

void inicializar_grafo(Grafo *g) {
    int i;
    g->num_aeroportos = 0;
    for (i = 0; i < MAX_AEROPORTOS; i++)
        g->adjacencias[i] = NULL;
}

void libertar_grafo(Grafo *g) {
    int i;
    for (i = 0; i < MAX_AEROPORTOS; i++) {
        Voo *v = g->adjacencias[i];
        while (v) {
            Voo *tmp = v->proximo;
            free(v);
            v = tmp;
        }
        g->adjacencias[i] = NULL;
    }
    g->num_aeroportos = 0;
}

/*  Aeroportos                                                         */

int encontrar_aeroporto(const Grafo *g, const char *codigo) {
    int i;
    for (i = 0; i < g->num_aeroportos; i++)
        if (strcmp(g->aeroportos[i].codigo, codigo) == 0) return i;
    return -1;
}

int inserir_aeroporto(Grafo *g, const char *codigo, const char *nome,
                      const char *cidade, const char *pais) {
    Aeroporto *a;
    if (g->num_aeroportos >= MAX_AEROPORTOS)       return 0;
    if (!validar_iata(codigo))                      return -1;
    if (encontrar_aeroporto(g, codigo) != -1)       return -2;
    if (!nome   || nome[0]   == '\0')               return -3;
    if (!cidade || cidade[0] == '\0')               return -3;
    if (!pais   || pais[0]   == '\0')               return -3;

    a = &g->aeroportos[g->num_aeroportos++];
    strncpy(a->codigo, codigo, 3);  a->codigo[3]  = '\0';
    strncpy(a->nome,   nome,   63); a->nome[63]   = '\0';
    strncpy(a->cidade, cidade, 47); a->cidade[47] = '\0';
    strncpy(a->pais,   pais,   47); a->pais[47]   = '\0';
    return 1;
}

void listar_aeroportos(const Grafo *g) {
    int i;
    if (!g->num_aeroportos) {
        printf("\n  Nenhum aeroporto registado.\n");
        return;
    }
    printf("\n  %-5s  %-34s  %-22s  %-20s\n",
           "Cod.", "Nome", "Cidade", "Pais");
    printf("  %s\n",
           "------------------------------------------------------------------------");
    for (i = 0; i < g->num_aeroportos; i++)
        printf("  %-5s  %-34s  %-22s  %-20s\n",
               g->aeroportos[i].codigo, g->aeroportos[i].nome,
               g->aeroportos[i].cidade, g->aeroportos[i].pais);
}

/*  Voos                                                               */

int inserir_voo(Grafo *g, const char *cod, const char *orig, const char *dest,
                float custo, float distancia, int duracao) {
    int  io, id;
    Voo *v, *novo;

    /* Validacoes basicas */
    if (!cod || cod[0] == '\0')             return -1;
    if (!validar_iata(orig))                return -2;
    if (!validar_iata(dest))                return -2;
    if (strcmp(orig, dest) == 0)            return -3;
    if (custo     <= 0.0f)                  return -4;
    if (distancia <= 0.0f)                  return -4;
    if (duracao   < DURACAO_MINIMA)         return -5;

    io = encontrar_aeroporto(g, orig);
    id = encontrar_aeroporto(g, dest);
    if (io < 0 || id < 0)                   return -6;

    /* Verificar codigo de voo duplicado e rota duplicada */
    for (v = g->adjacencias[io]; v; v = v->proximo) {
        if (strcmp(v->codigo, cod) == 0)    return -7;
        if (v->destino == id)               return -8;
    }

    novo = malloc(sizeof(Voo));
    if (!novo)                              return 0;

    strncpy(novo->codigo, cod, 9); novo->codigo[9] = '\0';
    novo->destino   = id;
    novo->custo     = custo;
    novo->distancia = distancia;
    novo->duracao   = duracao;
    novo->proximo   = g->adjacencias[io];
    g->adjacencias[io] = novo;
    return 1;
}

void listar_voos(const Grafo *g) {
    int i, algum = 0;
    for (i = 0; i < g->num_aeroportos && !algum; i++)
        if (g->adjacencias[i]) algum = 1;
    if (!algum) {
        printf("\n  Nenhum voo registado.\n");
        return;
    }
    printf("\n  %-10s  %-5s  %-5s  %16s  %14s  %9s\n",
           "Voo", "Orig.", "Dest.", "Custo (KZ)", "Distancia(km)", "Duracao");
    printf("  %s\n",
           "----------------------------------------------------------------------");
    for (i = 0; i < g->num_aeroportos; i++) {
        Voo *v;
        for (v = g->adjacencias[i]; v; v = v->proximo) {
            printf("  %-10s  %-5s  %-5s  %16.2f  %14.0f  %02dh%02dm\n",
                   v->codigo,
                   g->aeroportos[i].codigo,
                   g->aeroportos[v->destino].codigo,
                   v->custo, v->distancia,
                   v->duracao / 60, v->duracao % 60);
        }
    }
}

/*  Algoritmos de rota                                                 */

static void imprimir_caminho(const Grafo *g, const int *ant, int destino) {
    int seq[MAX_AEROPORTOS];
    int tam = 0, i;
    for (i = destino; i != -1 && tam < MAX_AEROPORTOS; i = ant[i])
        seq[tam++] = i;
    for (i = tam - 1; i >= 0; i--) {
        printf("%s", g->aeroportos[seq[i]].codigo);
        if (i) printf(" -> ");
    }
    printf("\n");
}

/* Procura o voo que liga u->v na lista de adjacencias */
static Voo *encontrar_voo_aresta(const Grafo *g, int u, int v) {
    Voo *voo;
    for (voo = g->adjacencias[u]; voo; voo = voo->proximo)
        if (voo->destino == v) return voo;
    return NULL;
}

/* Imprime os detalhes segmento a segmento e devolve o total acumulado */
static float imprimir_segmentos(const Grafo *g, const int *ant, int destino, int tipo) {
    int   seq[MAX_AEROPORTOS];
    int   tam = 0, i;
    float total = 0.0f;

    for (i = destino; i != -1 && tam < MAX_AEROPORTOS; i = ant[i])
        seq[tam++] = i;

    for (i = tam - 1; i > 0; i--) {
        int  u   = seq[i];
        int  v   = seq[i - 1];
        Voo *voo = encontrar_voo_aresta(g, u, v);
        if (!voo) continue;

        printf("\n  %s -> %s\n", g->aeroportos[u].codigo, g->aeroportos[v].codigo);
        printf("  Voo: %s\n", voo->codigo);
        if (tipo == 0) {
            printf("  Custo: %.0f KZ\n", voo->custo);
            total += voo->custo;
        } else if (tipo == 1) {
            printf("  Distancia: %.0f km\n", voo->distancia);
            total += voo->distancia;
        } else {
            printf("  Custo: %.0f KZ\n",    voo->custo);
            printf("  Distancia: %.0f km\n", voo->distancia);
        }
        printf("  Duracao: %02dh%02dm\n", voo->duracao / 60, voo->duracao % 60);
    }
    return total;
}

static void dijkstra(Grafo *g, int origem, float *dist, int *ant, int tipo) {
    int     vis[MAX_AEROPORTOS];
    HeapMin heap;
    int     i;

    heap.tamanho = 0;
    for (i = 0; i < g->num_aeroportos; i++) {
        dist[i] = INFINITO;
        ant[i]  = -1;
        vis[i]  = 0;
    }
    dist[origem] = 0.0f;
    push_heap(&heap, origem, 0.0f);

    while (heap.tamanho) {
        NoHeap cur = pop_heap(&heap);
        int    u   = cur.indice;
        Voo   *v;
        if (vis[u]) continue;
        vis[u] = 1;
        for (v = g->adjacencias[u]; v; v = v->proximo) {
            float w  = tipo ? v->distancia : v->custo;
            float nd = dist[u] + w;
            if (nd < dist[v->destino]) {
                dist[v->destino] = nd;
                ant[v->destino]  = u;
                push_heap(&heap, v->destino, nd);
            }
        }
    }
}

void rota_menor_custo(Grafo *g, const char *orig, const char *dest) {
    float dist[MAX_AEROPORTOS];
    int   ant[MAX_AEROPORTOS];
    int io = encontrar_aeroporto(g, orig);
    int id = encontrar_aeroporto(g, dest);
    if (io < 0 || id < 0) { printf("\n  Aeroporto nao encontrado.\n"); return; }

    dijkstra(g, io, dist, ant, 0);
    if (dist[id] == INFINITO) {
        printf("\n  Nao existe rota disponivel entre %s e %s.\n", orig, dest);
        return;
    }
    printf("\n  Rota de menor custo:\n  ");
    imprimir_caminho(g, ant, id);
    imprimir_segmentos(g, ant, id, 0);
    printf("\n  --------------------------------\n");
    printf("  Custo total: %.0f KZ\n", dist[id]);
}

void rota_menor_distancia(Grafo *g, const char *orig, const char *dest) {
    float dist[MAX_AEROPORTOS];
    int   ant[MAX_AEROPORTOS];
    int io = encontrar_aeroporto(g, orig);
    int id = encontrar_aeroporto(g, dest);
    if (io < 0 || id < 0) { printf("\n  Aeroporto nao encontrado.\n"); return; }

    dijkstra(g, io, dist, ant, 1);
    if (dist[id] == INFINITO) {
        printf("\n  Nao existe rota disponivel entre %s e %s.\n", orig, dest);
        return;
    }
    printf("\n  Rota de menor distancia:\n  ");
    imprimir_caminho(g, ant, id);
    imprimir_segmentos(g, ant, id, 1);
    printf("\n  --------------------------------\n");
    printf("  Distancia total: %.0f km\n", dist[id]);
}

void rota_menor_escalas(Grafo *g, const char *orig, const char *dest) {
    int niv[MAX_AEROPORTOS];
    int ant[MAX_AEROPORTOS];
    int vis[MAX_AEROPORTOS];
    int fila[MAX_AEROPORTOS];
    int ini = 0, fim = 0, i;

    int io = encontrar_aeroporto(g, orig);
    int id = encontrar_aeroporto(g, dest);
    if (io < 0 || id < 0) { printf("\n  Aeroporto nao encontrado.\n"); return; }

    for (i = 0; i < g->num_aeroportos; i++) {
        niv[i] = -1; ant[i] = -1; vis[i] = 0;
    }
    niv[io] = 0; vis[io] = 1; fila[fim++] = io;

    while (ini < fim) {
        int  u = fila[ini++];
        Voo *v;
        for (v = g->adjacencias[u]; v; v = v->proximo) {
            if (!vis[v->destino]) {
                vis[v->destino] = 1;
                niv[v->destino] = niv[u] + 1;
                ant[v->destino] = u;
                fila[fim++]     = v->destino;
            }
        }
    }
    if (niv[id] < 0) {
        printf("\n  Nao existe rota disponivel entre %s e %s.\n", orig, dest);
        return;
    }
    printf("\n  Rota com menor numero de escalas:\n  ");
    imprimir_caminho(g, ant, id);
    imprimir_segmentos(g, ant, id, 2);
    {
        int escalas = niv[id] - 1;
        printf("\n  --------------------------------\n");
        printf("  Escalas: %d\n", escalas < 0 ? 0 : escalas);
    }
}

void mostrar_rede(const Grafo *g) {
    int i;
    if (!g->num_aeroportos) {
        printf("\n  Nenhum aeroporto registado.\n");
        return;
    }
    printf("\n  Rede de Voos\n\n");
    for (i = 0; i < g->num_aeroportos; i++) {
        Voo *v = g->adjacencias[i];
        printf("  %s (%s)\n", g->aeroportos[i].codigo, g->aeroportos[i].cidade);
        if (!v) {
            printf("   (sem voos de saida)\n");
            continue;
        }
        while (v) {
            const char *ramo = v->proximo ? " +-- " : " +-- ";
            printf("  %s%s\n", ramo, g->aeroportos[v->destino].codigo);
            v = v->proximo;
        }
        printf("\n");
    }
}

/*  Persistencia - dados.txt                                           */
/*                                                                     */
/*  Formato:                                                           */
/*    AEROPORTOS <n>                                                   */
/*    <codigo> <nome_com_underscores> <cidade_com_underscores> <pais>  */
/*    VOOS <n>                                                         */
/*    <cod_voo> <orig> <dest> <custo> <distancia> <duracao_min>        */

/* Substitui espacos por '_' para facilitar fscanf */
static void espacos_para_underscores(const char *src, char *dst, int maxlen) {
    int i;
    for (i = 0; i < maxlen - 1 && src[i]; i++)
        dst[i] = (src[i] == ' ') ? '_' : src[i];
    dst[i] = '\0';
}

static void underscores_para_espacos(char *s) {
    for (; *s; s++)
        if (*s == '_') *s = ' ';
}

int guardar_dados(const Grafo *g, const char *ficheiro) {
    FILE *fp;
    int   i, total_voos = 0;
    char  tmp[64];

    fp = fopen(ficheiro, "w");
    if (!fp) return 0;

    /* --- Aeroportos --- */
    fprintf(fp, "AEROPORTOS %d\n", g->num_aeroportos);
    for (i = 0; i < g->num_aeroportos; i++) {
        const Aeroporto *a = &g->aeroportos[i];
        char nome_u[64], cidade_u[48], pais_u[48];
        espacos_para_underscores(a->nome,   nome_u,   sizeof(nome_u));
        espacos_para_underscores(a->cidade, cidade_u, sizeof(cidade_u));
        espacos_para_underscores(a->pais,   pais_u,   sizeof(pais_u));
        fprintf(fp, "%s %s %s %s\n",
                a->codigo, nome_u, cidade_u, pais_u);
    }

    /* contar voos */
    for (i = 0; i < g->num_aeroportos; i++) {
        Voo *v;
        for (v = g->adjacencias[i]; v; v = v->proximo) total_voos++;
    }

    /* --- Voos --- */
    fprintf(fp, "VOOS %d\n", total_voos);
    for (i = 0; i < g->num_aeroportos; i++) {
        Voo *v;
        for (v = g->adjacencias[i]; v; v = v->proximo) {
            espacos_para_underscores(v->codigo, tmp, sizeof(tmp));
            fprintf(fp, "%s %s %s %.2f %.2f %d\n",
                    tmp,
                    g->aeroportos[i].codigo,
                    g->aeroportos[v->destino].codigo,
                    v->custo, v->distancia, v->duracao);
        }
    }

    fclose(fp);
    return 1;
}

int carregar_dados(Grafo *g, const char *ficheiro) {
    FILE *fp;
    int   n, i, ret;
    char  secao[32];

    fp = fopen(ficheiro, "r");
    if (!fp) return 0;

    /* --- Aeroportos --- */
    ret = fscanf(fp, "%31s %d", secao, &n);
    if (ret != 2 || strcmp(secao, "AEROPORTOS") != 0) { fclose(fp); return 0; }

    for (i = 0; i < n; i++) {
        char codigo[4], nome[64], cidade[48], pais[48];
        if (fscanf(fp, "%3s %63s %47s %47s",
                   codigo, nome, cidade, pais) != 4) break;
        underscores_para_espacos(nome);
        underscores_para_espacos(cidade);
        underscores_para_espacos(pais);
        inserir_aeroporto(g, codigo, nome, cidade, pais);
    }

    /* --- Voos --- */
    ret = fscanf(fp, "%31s %d", secao, &n);
    if (ret != 2 || strcmp(secao, "VOOS") != 0) { fclose(fp); return 1; }

    for (i = 0; i < n; i++) {
        char  cod[10], orig[4], dest[4];
        float custo, distancia;
        int   duracao;
        if (fscanf(fp, "%9s %3s %3s %f %f %d",
                   cod, orig, dest, &custo, &distancia, &duracao) != 6) break;
        inserir_voo(g, cod, orig, dest, custo, distancia, duracao);
    }

    fclose(fp);
    return 1;
}

void carregar_dados_exemplo(Grafo *g) {
    /* aeroportos angolanos com codigos IATA oficiais */
    inserir_aeroporto(g, "NBJ", "Aeroporto Int. Dr. A. Agostinho Neto", "Luanda",    "Angola");
    inserir_aeroporto(g, "CAB", "Aeroporto do Cabinda",                  "Cabinda",   "Angola");
    inserir_aeroporto(g, "SZA", "Aeroporto do Soyo",                     "Soyo",      "Angola");
    inserir_aeroporto(g, "NOV", "Aeroporto de Huambo - Nova Lisboa",     "Huambo",    "Angola");
    inserir_aeroporto(g, "CBT", "Aeroporto de Catumbela",                "Catumbela", "Angola");
    inserir_aeroporto(g, "SDD", "Aeroporto de Lubango Mukanka",          "Lubango",   "Angola");
    inserir_aeroporto(g, "MSZ", "Aeroporto do Namibe",                   "Namibe",    "Angola");
    inserir_aeroporto(g, "VPE", "Aeroporto Geral Mariano Nhany",         "Ondjiva",   "Angola");
    inserir_aeroporto(g, "SPP", "Aeroporto de Menongue",                 "Menongue",  "Angola");

    /* --- Rotas NBJ (hub) <-> provincias --- */
    inserir_voo(g,"DT101","NBJ","CAB", 42000.0f,  480.0f,  90);
    inserir_voo(g,"DT102","CAB","NBJ", 42000.0f,  480.0f,  90);
    inserir_voo(g,"DT111","NBJ","SZA", 48000.0f,  560.0f,  90);
    inserir_voo(g,"DT112","SZA","NBJ", 48000.0f,  560.0f,  90);
    inserir_voo(g,"DT201","NBJ","NOV", 52000.0f,  530.0f,  90);
    inserir_voo(g,"DT202","NOV","NBJ", 52000.0f,  530.0f,  90);
    inserir_voo(g,"DT211","NBJ","CBT", 50000.0f,  560.0f,  90);
    inserir_voo(g,"DT212","CBT","NBJ", 50000.0f,  560.0f,  90);
    inserir_voo(g,"DT401","NBJ","SDD", 75000.0f, 1240.0f, 120);
    inserir_voo(g,"DT402","SDD","NBJ", 75000.0f, 1240.0f, 120);
    inserir_voo(g,"DT411","NBJ","MSZ", 68000.0f, 1110.0f, 110);
    inserir_voo(g,"DT412","MSZ","NBJ", 68000.0f, 1110.0f, 110);
    inserir_voo(g,"DT501","NBJ","VPE", 92000.0f, 1530.0f, 135);
    inserir_voo(g,"DT502","VPE","NBJ", 92000.0f, 1530.0f, 135);
    inserir_voo(g,"DT511","NBJ","SPP", 72000.0f, 1200.0f, 115);
    inserir_voo(g,"DT512","SPP","NBJ", 72000.0f, 1200.0f, 115);

    /* --- Rotas inter-provinciais sul de Angola --- */
    inserir_voo(g,"DT601","SDD","NOV", 28000.0f,  300.0f,  90);
    inserir_voo(g,"DT602","NOV","SDD", 28000.0f,  300.0f,  90);
    inserir_voo(g,"DT611","SDD","CBT", 30000.0f,  330.0f,  90);
    inserir_voo(g,"DT612","CBT","SDD", 30000.0f,  330.0f,  90);
    inserir_voo(g,"DT621","SDD","VPE", 32000.0f,  370.0f,  90);
    inserir_voo(g,"DT622","VPE","SDD", 32000.0f,  370.0f,  90);
    inserir_voo(g,"DT631","SDD","MSZ", 25000.0f,  270.0f,  90);
    inserir_voo(g,"DT632","MSZ","SDD", 25000.0f,  270.0f,  90);
    inserir_voo(g,"DT641","VPE","NOV", 36000.0f,  430.0f,  90);
    inserir_voo(g,"DT642","NOV","VPE", 36000.0f,  430.0f,  90);
    inserir_voo(g,"DT651","VPE","CBT", 35000.0f,  420.0f,  90);
    inserir_voo(g,"DT652","CBT","VPE", 35000.0f,  420.0f,  90);
    inserir_voo(g,"DT661","VPE","SPP", 31000.0f,  360.0f,  90);
    inserir_voo(g,"DT662","SPP","VPE", 31000.0f,  360.0f,  90);
}
