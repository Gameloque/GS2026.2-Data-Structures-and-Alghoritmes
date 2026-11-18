/*
 * Citizen Assist AI - Global Solution 2026.2 (DSA)
 * Recorte: Documentacao e Cidadania
 *
 * Prototipo educacional em C usando lista encadeada simples e Selection Sort.
 * Nao e canal oficial, nao realiza agendamentos reais e nao armazena dados pessoais
 * (apenas a quantidade de documentos pendentes).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_STR 50
#define ANO_ATUAL 2026

// Dias de cada mes em 2026 (nao bissexto)
const int dias_mes[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

const char *categorias[6] = {
    "", "Documentacao", "Saude", "Educacao", "Assistencia social", "Cidadania"
};

const char *pedidos[4] = {
    "", "Agendar atendimento", "Pedir 2a via", "Emitir documento novo"
};

// Documentos exigidos por categoria
const char *docs_por_categoria[6][3] = {
    {"", "", ""},
    {"Documento com foto", "Comprovante de residencia", "Certidao de nascimento ou casamento"},
    {"Documento com foto", "Cartao SUS", "Comprovante de residencia"},
    {"Documento do aluno ou responsavel", "Comprovante de residencia", "Historico escolar ou certidao"},
    {"Documento com foto", "CPF", "Comprovante de residencia"},
    {"Documento com foto", "CPF", "Comprovante de residencia"}
};

typedef struct Atendimento {
    int id;          // protocolo gerado automaticamente
    int categoria;   // 1 a 5
    int pedido;      // 1 a 3
    int faltam;      // qtd de docs faltando (0 = tudo certo)
    int dia;         // 0 enquanto nao tiver data marcada
    int mes;         // 0 enquanto nao tiver data marcada
    struct Atendimento *prox;
} Atendimento;

Atendimento *inicio = NULL;
int total = 0;
int proximo_id = 1;

// --- Manipulacao da lista encadeada ---

Atendimento *buscar_protocolo(int id) {
    for (Atendimento *p = inicio; p != NULL; p = p->prox) {
        if (p->id == id) return p;
    }
    return NULL;
}

int inserir_atendimento(int categoria, int pedido, int faltam, int dia, int mes) {
    Atendimento *novo = malloc(sizeof(Atendimento));
    if (!novo) return 0;

    novo->id = proximo_id++;
    novo->categoria = categoria;
    novo->pedido = pedido;
    novo->faltam = faltam;
    novo->dia = dia;
    novo->mes = mes;
    novo->prox = NULL;

    if (!inicio) {
        inicio = novo;
    } else {
        Atendimento *p = inicio;
        while (p->prox != NULL) {
            p = p->prox;
        }
        p->prox = novo;
    }

    total++;
    return novo->id;
}

int remover_atendimento(int id) {
    Atendimento *ant = NULL;
    Atendimento *atual = inicio;

    while (atual != NULL && atual->id != id) {
        ant = atual;
        atual = atual->prox;
    }

    if (!atual) return 0;

    if (!ant) {
        inicio = atual->prox;
    } else {
        ant->prox = atual->prox;
    }

    free(atual);
    total--;
    return 1;
}

void limpar_memoria(void) {
    while (inicio != NULL) {
        Atendimento *tmp = inicio->prox;
        free(inicio);
        inicio = tmp;
    }
    total = 0;
}

void encerrar(const char *msg) {
    limpar_memoria();
    printf("%s", msg);
    exit(0);
}

// --- Selection Sort ---

// Retorna a chave numerica usada na comparacao
int chave_ordenacao(Atendimento *a, int criterio) {
    switch (criterio) {
        case 1: return a->id;
        case 2: return a->categoria;
        case 3: return a->pedido;
        default:
            // Quem ainda nao tem data vai pro final da lista
            return (a->mes == 0) ? 9999 : (a->mes * 100 + a->dia);
    }
}

// Troca apenas os dados entre dois nos, mantendo os ponteiros intactos
void trocar_dados(Atendimento *a, Atendimento *b) {
    int tmp;

   tmp = a->id;        a->id = b->id;               b->id = tmp;
  tmp = a->categoria; a->categoria = b->categoria; b->categoria = tmp;
   tmp = a->pedido;    a->pedido = b->pedido;       b->pedido = tmp;
  tmp = a->faltam;    a->faltam = b->faltam;       b->faltam = tmp;
  tmp = a->dia;       a->dia = b->dia;             b->dia = tmp;
tmp = a->mes;       a->mes = b->mes;             b->mes = tmp;
}

void ordenar_lista(int criterio) {
    for (Atendimento *i = inicio; i != NULL; i = i->prox) {
        Atendimento *menor = i;
        for (Atendimento *j = i->prox; j != NULL; j = j->prox) {
            if (chave_ordenacao(j, criterio) < chave_ordenacao(menor, criterio)) {
                menor = j;
            }
        }
        if (menor != i) {
            trocar_dados(i, menor);
        }
    }
}

// --- Entrada de dados ---

void ler_texto(char *buf) {
    if (fgets(buf, MAX_STR, stdin) == NULL) {
        encerrar("\nEntrada encerrada. Memoria liberada.\n");
    }

    // Se passou do limite do buffer, limpa o restante da linha
    if (strchr(buf, '\n') == NULL) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
    } else {
        buf[strcspn(buf, "\n")] = '\0';
    }
}

int ler_int(void) {
    char buf[MAX_STR];
    ler_texto(buf);

    if (buf[0] == '\0' || strlen(buf) > 6) return -1;

    for (int i = 0; buf[i] != '\0'; i++) {
        if (!isdigit((unsigned char)buf[i])) return -1;
    }
    return atoi(buf);
}

int ler_positivo(const char *prompt) {
    while (1) {
        printf("%s", prompt);
        int n = ler_int();
        if (n > 0) return n;
        printf("  Valor invalido. Digite um numero inteiro maior que zero.\n");
    }
}

int ler_sn(void) {
    char buf[MAX_STR];
    while (1) {
        ler_texto(buf);
        if (buf[0] == 's' || buf[0] == 'S') {
            if (buf[1] == '\0') return 1;
        }
        if (buf[0] == 'n' || buf[0] == 'N') {
            if (buf[1] == '\0') return 0;
        }
        printf("  Responda apenas com 's' ou 'n': ");
    }
}

// --- Exibicao ---

void imprimir_situacao(Atendimento *a) {
    if (a->faltam == 0) {
        printf("Completo");
    } else {
        printf("Pendente (faltam %d)", a->faltam);
    }
}

void imprimir_data(Atendimento *a) {
    if (a->mes == 0) {
        printf("-");
    } else {
        printf("%02d/%02d/%d", a->dia, a->mes, ANO_ATUAL);
    }
}

void exibir_detalhes(Atendimento *a) {
    printf("  Protocolo : %d\n", a->id);
    printf("  Categoria : %s\n", categorias[a->categoria]);
    printf("  Pedido    : %s\n", pedidos[a->pedido]);
    printf("  Situacao  : ");
    imprimir_situacao(a);
    printf("\n  Data      : ");
    imprimir_data(a);
    printf("\n");
}

void exibir_linha(Atendimento *a) {
    printf("  [%3d] %-18s | %-21s | ", a->id, categorias[a->categoria], pedidos[a->pedido]);
    imprimir_data(a);
    printf(" | ");
    imprimir_situacao(a);
    printf("\n");
}

void listar_todos(void) {
    if (!inicio) {
        printf("  Nenhum atendimento cadastrado ainda.\n");
        return;
    }

    printf("  %-5s %-18s | %-21s | %-10s | %s\n", "ID", "CATEGORIA", "PEDIDO", "DATA", "SITUACAO");
    for (Atendimento *p = inicio; p != NULL; p = p->prox) {
        exibir_linha(p);
    }
    printf("  Total: %d registro(s).\n", total);
}

// --- Fluxo de perguntas ---

int escolher_categoria(void) {
    while (1) {
        printf("\n  Escolha a categoria:\n");
        printf("  1) Documentacao\n");
        printf("  2) Saude\n");
        printf("  3) Educacao\n");
        printf("  4) Assistencia social\n");
        printf("  5) Cidadania\n");
        printf("  0) Voltar\n");
        printf("  > ");

        switch (ler_int()) {
            case 1:
                printf("  -> Documentacao (RG, CPF, certidoes)\n");
                return 1;
            case 2:
                printf("  -> Saude (consulta na UBS, Cartao SUS)\n");
                return 2;
            case 3:
                printf("  -> Educacao (matricula, historico escolar)\n");
                return 3;
            case 4:
                printf("  -> Assistencia social (CadUnico, CRAS)\n");
                return 4;
            case 5:
                printf("  -> Cidadania (titulo de eleitor)\n");
                return 5;
            case 0:
                return 0;
            default:
                printf("  Opcao invalida. Escolha de 0 a 5.\n");
        }
    }
}

int checar_documentos(int categoria) {
    int faltam = 0;
    printf("\n  Voce esta com estes documentos em maos? (s/n)\n");
    printf("  (Nao digite numeros de documentos, apenas 's' ou 'n')\n");

    for (int i = 0; i < 3; i++) {
        printf("  - %s? ", docs_por_categoria[categoria][i]);
        if (!ler_sn()) {
            printf("    * Pendente: %s\n", docs_por_categoria[categoria][i]);
            faltam++;
        }
    }
    return faltam;
}

int escolher_pedido(void) {
    while (1) {
        printf("\n  O que voce precisa fazer?\n");
        printf("  1) Agendar atendimento\n");
        printf("  2) Pedir 2a via de documento\n");
        printf("  3) Emitir documento novo\n");
        printf("  0) Voltar\n");
        printf("  > ");

        int op = ler_int();
        if (op >= 0 && op <= 3) return op;
        printf("  Opcao invalida. Escolha de 0 a 3.\n");
    }
}

void escolher_data(int *dia, int *mes) {
    int m, d;
    printf("\n  Informe a data desejada (ano %d):\n", ANO_ATUAL);

    while (1) {
        printf("  Mes (1 a 12): ");
        m = ler_int();
        if (m >= 1 && m <= 12) break;
        printf("  Mes invalido.\n");
    }

    while (1) {
        printf("  Dia (1 a %d): ", dias_mes[m]);
        d = ler_int();
        if (d >= 1 && d <= dias_mes[m]) break;
        printf("  Dia invalido para o mes %d.\n", m);
    }

    *dia = d;
    *mes = m;
}

// --- Operacoes do menu ---

void novo_atendimento(void) {
    printf("\n-- Novo atendimento --\n");

    int categoria = escolher_categoria();
    if (categoria == 0) {
        printf("  Voltando ao menu principal.\n");
        return;
    }

    int faltam = checar_documentos(categoria);

    int pedido = escolher_pedido();
    if (pedido == 0) {
        printf("  Operacao cancelada.\n");
        return;
    }

    int dia = 0, mes = 0;
    if (faltam == 0) {
        printf("\n  Documentacao completa!\n");
        escolher_data(&dia, &mes);
        printf("\n  Resumo: %s (%s) em %02d/%02d/%d.\n",
               pedidos[pedido], categorias[categoria], dia, mes, ANO_ATUAL);
        printf("  Lembrete: este e apenas um prototipo, nenhum agendamento real foi feito.\n");
    } else {
        printf("\n  Atendimento pendente: faltam %d documento(s).\n", faltam);
        printf("  Quando reunir tudo, use a opcao 4 do menu para atualizar.\n");
    }

    int protocolo = inserir_atendimento(categoria, pedido, faltam, dia, mes);
    if (protocolo > 0) {
        printf("  Registrado com sucesso. Protocolo: %d\n", protocolo);
    } else {
        printf("  Erro: falha ao alocar memoria.\n");
    }
}

void consultar_protocolo(void) {
    printf("\n-- Consultar protocolo --\n");
    if (!inicio) {
        printf("  A lista esta vazia.\n");
        return;
    }

    int id = ler_positivo("  Numero do protocolo: ");
    Atendimento *a = buscar_protocolo(id);
    if (!a) {
        printf("  Protocolo %d nao encontrado.\n", id);
    } else {
        exibir_detalhes(a);
    }
}

void filtrar_categoria(void) {
    printf("\n-- Pesquisar por categoria --\n");
    if (!inicio) {
        printf("  A lista esta vazia.\n");
        return;
    }

    int cat;
    while (1) {
        printf("  1) Documentacao  2) Saude  3) Educacao  4) Assistencia social  5) Cidadania\n");
        printf("  Categoria: ");
        cat = ler_int();
        if (cat >= 1 && cat <= 5) break;
        printf("  Opcao invalida. Digite de 1 a 5.\n");
    }

    int achados = 0;
    for (Atendimento *p = inicio; p != NULL; p = p->prox) {
        if (p->categoria == cat) {
            if (achados == 0) printf("  Resultados encontrados:\n");
            exibir_linha(p);
            achados++;
        }
    }

    if (achados == 0) {
        printf("  Nenhum registro encontrado em \"%s\".\n", categorias[cat]);
    } else {
        printf("  %d registro(s) encontrado(s).\n", achados);
    }
}

void atualizar_documentos(void) {
    printf("\n-- Atualizar documentos --\n");
    if (!inicio) {
        printf("  A lista esta vazia.\n");
        return;
    }

    int id = ler_positivo("  Protocolo do atendimento: ");
    Atendimento *a = buscar_protocolo(id);
    if (!a) {
        printf("  Protocolo %d nao encontrado.\n", id);
        return;
    }

    printf("  Dados atuais:\n");
    exibir_detalhes(a);

    a->faltam = checar_documentos(a->categoria);
    if (a->faltam == 0) {
        printf("\n  Documentacao completa!\n");
        if (a->mes == 0) {
            escolher_data(&a->dia, &a->mes);
        }
    } else {
        printf("\n  Ainda faltam %d documento(s).\n", a->faltam);
    }
    printf("  Protocolo %d atualizado.\n", id);
}

void excluir_atendimento(void) {
    printf("\n-- Excluir atendimento --\n");
    if (!inicio) {
        printf("  A lista esta vazia.\n");
        return;
    }

    int id = ler_positivo("  Protocolo a remover: ");
    if (remover_atendimento(id)) {
        printf("  Protocolo %d removido.\n", id);
    } else {
        printf("  Protocolo %d nao encontrado.\n", id);
    }
}

void menu_ordenar(void) {
    printf("\n-- Ordenar atendimentos (Selection Sort) --\n");
    if (!inicio) {
        printf("  A lista esta vazia.\n");
        return;
    }

    int criterio;
    while (1) {
        printf("  Ordenar por: 1) Protocolo  2) Categoria  3) Pedido  4) Data > ");
        criterio = ler_int();
        if (criterio >= 1 && criterio <= 4) break;
        printf("  Opcao invalida. Digite de 1 a 4.\n");
    }

    ordenar_lista(criterio);
    printf("  Lista ordenada:\n");
    listar_todos();
}

void relatorio_analise(void) {
    printf("\n-- Resumo / Indicadores --\n");
    if (!inicio) {
        printf("  Lista vazia, nada para analisar.\n");
        return;
    }

    printf("  Total de atendimentos: %d\n", total);
    printf("  Distribuicao por categoria:\n");

    for (int c = 1; c <= 5; c++) {
        int qtd = 0;
        for (Atendimento *p = inicio; p != NULL; p = p->prox) {
            if (p->categoria == c) qtd++;
        }
        printf("    - %-20s %d\n", categorias[c], qtd);
    }

    int completos = 0;
    for (Atendimento *p = inicio; p != NULL; p = p->prox) {
        if (p->faltam == 0) completos++;
    }

    printf("  Documentos completos: %d\n", completos);
    printf("  Documentos pendentes: %d\n", total - completos);
}

// Popula a lista com dados ficticios para facilitar testes e apresentacao
void carregar_exemplos(void) {
    int cats[6]   = {1, 2, 3, 4, 5, 1};
    int peds[6]   = {2, 1, 3, 1, 2, 3};
    int faltas[6] = {0, 0, 1, 0, 2, 0};
    int dias[6]   = {10, 5, 0, 20, 0, 3};
    int meses[6]  = {12, 12, 0, 11, 0, 12};

    for (int i = 0; i < 6; i++) {
        inserir_atendimento(cats[i], peds[i], faltas[i], dias[i], meses[i]);
    }
    printf("\n  6 atendimentos de exemplo carregados.\n");
}

// --- Menu principal ---

void mostrar_menu(void) {
    printf("\n----------------------------------------------\n");
    printf("  CITIZEN ASSIST AI - Documentacao e Cidadania\n");
    printf("----------------------------------------------\n");
    printf("  1) Novo atendimento\n");
    printf("  2) Consultar por protocolo\n");
    printf("  3) Pesquisar por categoria\n");
    printf("  4) Atualizar documentos\n");
    printf("  5) Excluir atendimento\n");
    printf("  6) Ordenar lista\n");
    printf("  7) Listar todos\n");
    printf("  8) Analise (indicadores)\n");
    printf("  9) Carregar exemplos de teste\n");
    printf("  0) Sair\n");
    printf("  Opcao: ");
}

int main(void) {
    printf("\n[AVISO] Prototipo academico. Nao e um servico oficial e nao realiza\n");
    printf("agendamentos reais. Nao digite dados pessoais ou numeros de documentos.\n");
    printf("Para servicos reais, acesse os canais oficiais (gov.br).\n");

    while (1) {
        mostrar_menu();

        switch (ler_int()) {
            case 1: novo_atendimento();      break;
            case 2: consultar_protocolo();   break;
            case 3: filtrar_categoria();     break;
            case 4: atualizar_documentos();  break;
            case 5: excluir_atendimento();   break;
            case 6: menu_ordenar();          break;
            case 7:
                printf("\n-- Lista de atendimentos --\n");
                listar_todos();
                break;
            case 8: relatorio_analise();     break;
            case 9: carregar_exemplos();     break;
            case 0: encerrar("\nEncerrando o sistema. Memoria liberada.\n");
            default:
                printf("  Opcao invalida. Digite um numero de 0 a 9.\n");
        }
    }

    return 0;
}
