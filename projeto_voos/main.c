#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "grafo.h"

/* utilitarios de entrada */

static void limpar_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

static void ler_str(const char *msg, char *buf, int n) {
    printf("%s", msg);
    if (fgets(buf, n, stdin))
        buf[strcspn(buf, "\n")] = '\0';
}

static void ler_str_upper(const char *msg, char *buf, int n) {
    int i;
    ler_str(msg, buf, n);
    for (i = 0; buf[i]; i++)
        buf[i] = (char)toupper((unsigned char)buf[i]);
}

/* Le um float positivo; rejeita valores <= 0 e input invalido */
static float ler_float_positivo(const char *msg) {
    float v;
    int   r;
    for (;;) {
        printf("%s", msg);
        r = scanf("%f", &v);
        limpar_buffer();
        if (r == EOF) { v = -1.0f; break; }
        if (r == 1 && v > 0.0f) break;
        printf("  Valor invalido. Introduza um numero maior que zero.\n");
    }
    return v;
}

/* Le um inteiro dentro de [minimo, INT_MAX]; rejeita o resto */
static int ler_int_minimo(const char *msg, int minimo) {
    int v, r;
    for (;;) {
        printf("%s", msg);
        r = scanf("%d", &v);
        limpar_buffer();
        if (r == EOF) { v = minimo; break; }
        if (r == 1 && v >= minimo) break;
        printf("  Valor invalido. Minimo: %d.\n", minimo);
    }
    return v;
}

/* interface */

static void cabecalho(void) {
    printf("\n");
    printf("  ============================================\n");
    printf("      SISTEMA DE GESTAO DE VOOS - ANGOLA\n");
    printf("      Estruturas de Dados II\n");
    printf("  ============================================\n");
}

static void menu(void) {
    printf("\n");
    printf("  1. Registar Aeroporto\n");
    printf("  2. Registar Voo\n");
    printf("  3. Listar Aeroportos\n");
    printf("  4. Listar Voos disponiveis\n");
    printf("  5. Procurar rota  [Menor Custo]\n");
    printf("  6. Procurar rota  [Menor Distancia]\n");
    printf("  7. Procurar rota  [Menor N. de Escalas]\n");
    printf("  8. Mostrar Rede de Voos\n");
    printf("  9. Sair\n");
    printf("\n  Opcao: ");
}

/* operacoes do menu */

static void op_registar_aeroporto(Grafo *g) {
    char codigo[5], nome[64], cidade[48], pais[48];
    int  ret;

    printf("\n  == Registar Aeroporto ==\n");
    ler_str_upper("  Codigo IATA (3 letras, ex: LAD): ", codigo, sizeof(codigo));
    ler_str      ("  Nome completo: ",                   nome,   sizeof(nome));
    ler_str      ("  Cidade: ",                          cidade, sizeof(cidade));
    ler_str      ("  Pais: ",                            pais,   sizeof(pais));

    ret = inserir_aeroporto(g, codigo, nome, cidade, pais);
    switch (ret) {
        case  1:  printf("\n  [OK] Aeroporto '%s' registado com sucesso.\n", codigo); break;
        case -1:  printf("\n  [ERRO] Codigo IATA invalido. Use exactamente 3 letras maiusculas (ex: LAD).\n"); break;
        case -2:  printf("\n  [ERRO] O codigo '%s' ja esta registado.\n", codigo); break;
        case -3:  printf("\n  [ERRO] Nome, cidade e pais nao podem estar vazios.\n"); break;
        default:  printf("\n  [ERRO] Limite de aeroportos atingido (%d).\n", MAX_AEROPORTOS); break;
    }
}

static void op_registar_voo(Grafo *g) {
    char  cod[10], orig[5], dest[5];
    float custo, distancia;
    int   duracao, ret;

    printf("\n  == Registar Voo ==\n");
    ler_str      ("  Codigo do voo (ex: DT401): ",           cod,  sizeof(cod));
    ler_str_upper("  Aeroporto de origem  (IATA): ",         orig, sizeof(orig));
    ler_str_upper("  Aeroporto de destino (IATA): ",         dest, sizeof(dest));
    custo     = ler_float_positivo("  Custo (KZ): ");
    distancia = ler_float_positivo("  Distancia (km): ");
    duracao   = ler_int_minimo    ("  Duracao (minutos, minimo 90): ", DURACAO_MINIMA);

    ret = inserir_voo(g, cod, orig, dest, custo, distancia, duracao);
    switch (ret) {
        case  1:  printf("\n  [OK] Voo '%s' registado com sucesso.\n", cod); break;
        case -1:  printf("\n  [ERRO] O codigo do voo nao pode estar vazio.\n"); break;
        case -2:  printf("\n  [ERRO] Codigo IATA invalido (origem ou destino).\n"); break;
        case -3:  printf("\n  [ERRO] Origem e destino nao podem ser iguais.\n"); break;
        case -4:  printf("\n  [ERRO] Custo e distancia devem ser valores positivos.\n"); break;
        case -5:  printf("\n  [ERRO] Duracao minima e de %d minutos (1h30).\n", DURACAO_MINIMA); break;
        case -6:  printf("\n  [ERRO] Aeroporto de origem ou destino nao encontrado.\n"); break;
        case -7:  printf("\n  [ERRO] O codigo de voo '%s' ja existe.\n", cod); break;
        case -8:  printf("\n  [ERRO] Ja existe um voo de %s para %s.\n", orig, dest); break;
        default:  printf("\n  [ERRO] Falha ao alocar memoria.\n"); break;
    }
}

static void op_rota(Grafo *g, int tipo) {
    static const char *titulos[] = {
        "Menor Custo",
        "Menor Distancia",
        "Menor Numero de Escalas"
    };
    char orig[5], dest[5];

    printf("\n  == Procurar Rota [%s] ==\n", titulos[tipo]);
    ler_str_upper("  Aeroporto de origem  (IATA): ", orig, sizeof(orig));
    ler_str_upper("  Aeroporto de destino (IATA): ", dest, sizeof(dest));

    switch (tipo) {
        case 0: rota_menor_custo     (g, orig, dest); break;
        case 1: rota_menor_distancia (g, orig, dest); break;
        case 2: rota_menor_escalas   (g, orig, dest); break;
    }
}

int main(void) {
    Grafo g;
    int   op;

    inicializar_grafo(&g);
    cabecalho();

    if (carregar_dados(&g, FICHEIRO_DADOS)) {
        printf("\n  [INFO] Dados carregados de '%s'.\n", FICHEIRO_DADOS);
    } else {
        printf("\n  [INFO] Ficheiro '%s' nao encontrado."
               " A carregar dados de exemplo...\n", FICHEIRO_DADOS);
        carregar_dados_exemplo(&g);
    }

    do {
        menu();
        if (scanf("%d", &op) != 1) {
            limpar_buffer();
            op = 0;
        } else {
            limpar_buffer();
        }

        switch (op) {
            case 1: op_registar_aeroporto(&g); break;
            case 2: op_registar_voo(&g);       break;
            case 3: listar_aeroportos(&g);      break;
            case 4: listar_voos(&g);            break;
            case 5: op_rota(&g, 0);             break;
            case 6: op_rota(&g, 1);             break;
            case 7: op_rota(&g, 2);             break;
            case 8: mostrar_rede(&g);           break;
            case 9: break;
            default: printf("\n  Opcao invalida. Tente novamente.\n"); break;
        }
    } while (op != 9);

    if (guardar_dados(&g, FICHEIRO_DADOS))
        printf("\n  [INFO] Dados guardados em '%s'.\n", FICHEIRO_DADOS);
    else
        printf("\n  [AVISO] Nao foi possivel guardar os dados.\n");

    printf("\n  Ate logo!\n\n");

    libertar_grafo(&g);
    return 0;
}
