/*
  Citizen Assist AI - Global Solution 2026.2 - DSA
  Recorte: Documentacao e Cidadania
O cidadao escolhe a categoria (Documentacao, Saude...), diz quais documentos tem em maos e escolhe o que quer fazer (agendar, 2a via...).
  Cada atendimento fica guardado numa lista encadeada simples.
  Ordenacao: Selection Sort.
 
 Obs: prototipo de estudo, nao e canal oficial e nao agenda de verdade.
 Nao guarda dado pessoal: so guarda quantos documentos faltam.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define TAM 50

/* ---------------- nomes que aparecem na tela ---------------- */

const char *categorias[6] = {
    "", "Documentacao", "Saude", "Educacao", "Assistencia social", "Cidadania"
};

const char *pedidos[4] = {
    "", "Agendar atendimento", "Pedir 2a via", "Emitir documento novo"
};

/* os 3 documentos pedidos em cada categoria */
const char *docs[6][3] = {
    {"", "", ""},
    {"Documento com foto", "Comprovante de residencia", "Certidao de nascimento ou casamento"},
    {"Documento com foto", "Cartao SUS", "Comprovante de residencia"},
    {"Documento do aluno ou responsavel", "Comprovante de residencia", "Historico escolar ou certidao"},
    {"Documento com foto", "CPF", "Comprovante de residencia"},
    {"Documento com foto", "CPF", "Comprovante de residencia"}
};

/* ---------------- lista encadeada ---------------- */

/* cada no da lista e um atendimento */
typedef struct Atendimento {
    int id;          /* protocolo (gerado pelo sistema) */
    int categoria;   /* 1 a 5 */
    int pedido;      /* 1 a 3 */
    int faltam;      /* quantos documentos faltam (0 = completo) */
    struct Atendimento *prox;
} Atendimento;

Atendimento *inicio = NULL;   /* primeiro no da lista */
int total = 0;                /* quantos nos tem */
int proximo_id = 1;           /* proximo protocolo */

/* procura o no com esse protocolo. Devolve NULL se nao achar */
Atendimento *buscar_id(int id) {
    Atendimento *p = inicio;
    while (p != NULL) {
        if (p->id == id) {
            return p;
        }
        p = p->prox;
    }
    return NULL;
}

/* cria um no novo e coloca no fim da lista. Devolve o protocolo (0 se faltou memoria) */
int inserir_fim(int categoria, int pedido, int faltam) {
    Atendimento *novo = (Atendimento *) malloc(sizeof(Atendimento));
    if (novo == NULL) {
        return 0;
    }
    novo->id = proximo_id;
    novo->categoria = categoria;
    novo->pedido = pedido;
    novo->faltam = faltam;
    novo->prox = NULL;
    proximo_id++;

    if (inicio == NULL) {
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

/* remove o no com esse protocolo. Devolve 1 se removeu, 0 se nao achou */
int remover(int id) {
    Atendimento *anterior = NULL;
    Atendimento *atual = inicio;

    while (atual != NULL && atual->id != id) {
        anterior = atual;
        atual = atual->prox;
    }
    if (atual == NULL) {
        return 0;
    }
    if (anterior == NULL) {
        inicio = atual->prox;       /* era o primeiro */
    } else {
        anterior->prox = atual->prox;
    }
    free(atual);
    total--;
    return 1;
}

/* da free em todos os nos */
void liberar_lista(void) {
    while (inicio != NULL) {
        Atendimento *proximo = inicio->prox;
        free(inicio);
        inicio = proximo;
    }
    total = 0;
}

/* libera a memoria, mostra a mensagem e fecha o programa */
void sair(const char *mensagem) {
    liberar_lista();
    printf("%s", mensagem);
    exit(0);
}

/* ---------------- ordenacao (Selection Sort) ---------------- */

/* devolve o valor que vai ser comparado: 1 = protocolo, 2 = categoria, 3 = pedido */
int valor(Atendimento *a, int criterio) {
    if (criterio == 1) {
        return a->id;
    }
    if (criterio == 2) {
        return a->categoria;
    }
    return a->pedido;
}

/* troca os dados de dois nos (os ponteiros prox ficam como estao) */
void trocar_dados(Atendimento *a, Atendimento *b) {
    int temp;

    temp = a->id;
    a->id = b->id;
    b->id = temp;

    temp = a->categoria;
    a->categoria = b->categoria;
    b->categoria = temp;

    temp = a->pedido;
    a->pedido = b->pedido;
    b->pedido = temp;

    temp = a->faltam;
    a->faltam = b->faltam;
    b->faltam = temp;
}

/* Selection Sort: pra cada posicao i, acha o menor entre i e o fim da lista
 * e troca com o no da posicao i */
void ordenar(int criterio) {
    Atendimento *i = inicio;
    while (i != NULL) {
        Atendimento *menor = i;
        Atendimento *j = i->prox;
        while (j != NULL) {
            if (valor(j, criterio) < valor(menor, criterio)) {
                menor = j;
            }
            j = j->prox;
        }
        trocar_dados(i, menor);
        i = i->prox;
    }
}

/* ---------------- leitura do teclado ---------------- */

/* le uma linha e tira o \n. Se a entrada acabar (EOF), fecha o programa */
void ler_linha(char *linha) {
    if (fgets(linha, TAM, stdin) == NULL) {
        sair("\n  Entrada encerrada. Memoria liberada.\n");
    }
    if (strchr(linha, '\n') == NULL) {
        /* linha maior que o espaco: joga o resto fora */
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {
        }
    } else {
        linha[strcspn(linha, "\n")] = '\0';
    }
}

/* le um numero inteiro (so digitos). Se digitar outra coisa, devolve -1 */
int ler_numero(void) {
    char linha[TAM];
    ler_linha(linha);
    if (linha[0] == '\0' || strlen(linha) > 6) {
        return -1;
    }
    for (int i = 0; linha[i] != '\0'; i++) {
        if (!isdigit((unsigned char) linha[i])) {
            return -1;
        }
    }
    return atoi(linha);
}

/* pede um numero maior que zero, repete ate digitar certo */
int ler_positivo(const char *pergunta) {
    int numero;
    while (1) {
        printf("%s", pergunta);
        numero = ler_numero();
        if (numero > 0) {
            return numero;
        }
        printf("  [!] Entrada invalida. Digite um numero inteiro positivo.\n");
    }
}

/* pede s ou n. Devolve 1 para sim e 0 para nao */
int ler_sim_nao(void) {
    char linha[TAM];
    while (1) {
        ler_linha(linha);
        if (strcmp(linha, "s") == 0 || strcmp(linha, "S") == 0) {
            return 1;
        }
        if (strcmp(linha, "n") == 0 || strcmp(linha, "N") == 0) {
            return 0;
        }
        printf("  [!] Resposta invalida. Digite s ou n.\n  > ");
    }
}

/* ---------------- mostrar na tela ---------------- */

void mostrar_situacao(Atendimento *a) {
    if (a->faltam == 0) {
        printf("Completo");
    } else {
        printf("Pendente (faltam %d)", a->faltam);
    }
}

void mostrar_detalhe(Atendimento *a) {
    printf("  Protocolo ..: %d\n", a->id);
    printf("  Categoria ..: %s\n", categorias[a->categoria]);
    printf("  Pedido .....: %s\n", pedidos[a->pedido]);
    printf("  Situacao ...: ");
    mostrar_situacao(a);
    printf("\n");
}

void mostrar_linha(Atendimento *a) {
    printf("  [%3d] %-18s | %-21s | ", a->id, categorias[a->categoria], pedidos[a->pedido]);
    mostrar_situacao(a);
    printf("\n");
}

void listar(void) {
    if (inicio == NULL) {
        printf("  A lista esta vazia. Faca um novo atendimento ou carregue exemplos.\n");
        return;
    }
    printf("  %-5s %-18s | %-21s | %s\n", "ID", "CATEGORIA", "PEDIDO", "SITUACAO");
    Atendimento *p = inicio;
    while (p != NULL) {
        mostrar_linha(p);
        p = p->prox;
    }
    printf("  Total: %d registro(s).\n", total);
}

/* ---------------- perguntas do atendimento ---------------- */

/* menu de categorias com switch. Devolve 1 a 5 (a categoria) ou 0 para voltar */
int escolher_categoria(void) {
    while (1) {
        printf("\n  Escolha a categoria do atendimento:\n");
        printf("  1) Documentacao\n");
        printf("  2) Saude\n");
        printf("  3) Educacao\n");
        printf("  4) Assistencia social\n");
        printf("  5) Cidadania\n");
        printf("  0) Voltar\n");
        printf("  Categoria > ");

        switch (ler_numero()) {
            case 1:
                printf("  Categoria: Documentacao (RG, CPF, certidoes).\n");
                return 1;
            case 2:
                printf("  Categoria: Saude (consulta na UBS, Cartao SUS).\n");
                return 2;
            case 3:
                printf("  Categoria: Educacao (matricula, historico escolar).\n");
                return 3;
            case 4:
                printf("  Categoria: Assistencia social (CadUnico, CRAS).\n");
                return 4;
            case 5:
                printf("  Categoria: Cidadania (titulo de eleitor).\n");
                return 5;
            case 0:
                return 0;
            default:
                printf("  [!] Opcao invalida. Digite um numero de 0 a 5.\n");
        }
    }
}

/* pergunta, um por um, se a pessoa tem os documentos da categoria.
 * Devolve quantos documentos faltam */
int perguntar_documentos(int categoria) {
    int faltam = 0;
    printf("\n  Voce tem estes documentos em maos? Responda s (sim) ou n (nao).\n");
    printf("  Nao digite numeros de documentos, so responda s ou n.\n");
    for (int i = 0; i < 3; i++) {
        printf("  %s? > ", docs[categoria][i]);
        if (!ler_sim_nao()) {
            printf("    Falta: %s\n", docs[categoria][i]);
            faltam++;
        }
    }
    return faltam;
}

/* pergunta o que a pessoa quer fazer. Devolve 1 a 3 ou 0 para voltar */
int escolher_pedido(void) {
    int opcao;
    while (1) {
        printf("\n  O que voce deseja fazer?\n");
        printf("  1) Agendar atendimento\n");
        printf("  2) Pedir 2a via de documento\n");
        printf("  3) Emitir documento novo\n");
        printf("  0) Voltar\n");
        printf("  Opcao > ");
        opcao = ler_numero();
        if (opcao >= 0 && opcao <= 3) {
            return opcao;
        }
        printf("  [!] Opcao invalida. Digite um numero de 0 a 3.\n");
    }
}

/* ---------------- opcoes do menu ---------------- */

void op_novo(void) {
    printf("\n-- Novo atendimento --\n");

    int categoria = escolher_categoria();
    if (categoria == 0) {
        printf("  Voltando ao menu.\n");
        return;
    }

    int faltam = perguntar_documentos(categoria);

    int pedido = escolher_pedido();
    if (pedido == 0) {
        printf("  Atendimento cancelado.\n");
        return;
    }

    if (faltam == 0) {
        printf("\n  Situacao: COMPLETO - voce tem todos os documentos.\n");
        printf("  Pedido registrado: %s (%s).\n", pedidos[pedido], categorias[categoria]);
        printf("  Aviso: e um prototipo, nada foi agendado de verdade. Confirme no canal oficial.\n");
    } else {
        printf("\n  Situacao: PENDENTE - faltam %d documento(s).\n", faltam);
        printf("  Reuna os documentos que faltam e use a opcao 4 para atualizar este atendimento.\n");
    }

    int protocolo = inserir_fim(categoria, pedido, faltam);
    if (protocolo > 0) {
        printf("  Atendimento registrado. Protocolo: %d\n", protocolo);
    } else {
        printf("  [!] Memoria insuficiente.\n");
    }
}

void op_consultar(void) {
    printf("\n-- Consultar por protocolo --\n");
    if (inicio == NULL) {
        printf("  A lista esta vazia.\n");
        return;
    }
    int id = ler_positivo("  Protocolo: ");
    Atendimento *a = buscar_id(id);
    if (a == NULL) {
        printf("  Nenhum atendimento encontrado com o protocolo %d.\n", id);
    } else {
        mostrar_detalhe(a);
    }
}

void op_pesquisar(void) {
    printf("\n-- Pesquisar por categoria --\n");
    if (inicio == NULL) {
        printf("  A lista esta vazia.\n");
        return;
    }
    int categoria;
    while (1) {
        printf("  1) Documentacao  2) Saude  3) Educacao  4) Assistencia social  5) Cidadania\n");
        printf("  Categoria > ");
        categoria = ler_numero();
        if (categoria >= 1 && categoria <= 5) {
            break;
        }
        printf("  [!] Opcao invalida. Digite um numero de 1 a 5.\n");
    }

    int achados = 0;
    Atendimento *p = inicio;
    while (p != NULL) {
        if (p->categoria == categoria) {
            if (achados == 0) {
                printf("  Resultados:\n");
            }
            mostrar_linha(p);
            achados++;
        }
        p = p->prox;
    }
    if (achados == 0) {
        printf("  Nenhum atendimento da categoria \"%s\".\n", categorias[categoria]);
    } else {
        printf("  %d resultado(s).\n", achados);
    }
}

/* atualiza os documentos de um atendimento (a pessoa conseguiu o que faltava) */
void op_atualizar(void) {
    printf("\n-- Atualizar documentos --\n");
    if (inicio == NULL) {
        printf("  A lista esta vazia.\n");
        return;
    }
    int id = ler_positivo("  Protocolo do atendimento: ");
    Atendimento *a = buscar_id(id);
    if (a == NULL) {
        printf("  Nenhum atendimento encontrado com o protocolo %d.\n", id);
        return;
    }
    printf("  Atendimento atual:\n");
    mostrar_detalhe(a);

    a->faltam = perguntar_documentos(a->categoria);
    if (a->faltam == 0) {
        printf("\n  Situacao: COMPLETO - voce tem todos os documentos.\n");
    } else {
        printf("\n  Situacao: PENDENTE - faltam %d documento(s).\n", a->faltam);
    }
    printf("  Atendimento %d atualizado.\n", id);
}

void op_excluir(void) {
    printf("\n-- Excluir atendimento --\n");
    if (inicio == NULL) {
        printf("  A lista esta vazia.\n");
        return;
    }
    int id = ler_positivo("  Protocolo do atendimento: ");
    if (remover(id)) {
        printf("  Atendimento %d excluido.\n", id);
    } else {
        printf("  Nenhum atendimento encontrado com o protocolo %d.\n", id);
    }
}

void op_ordenar(void) {
    printf("\n-- Ordenar (Selection Sort) --\n");
    if (inicio == NULL) {
        printf("  A lista esta vazia.\n");
        return;
    }
    int criterio;
    while (1) {
        printf("  Criterio: 1) Protocolo  2) Categoria  3) Pedido  > ");
        criterio = ler_numero();
        if (criterio >= 1 && criterio <= 3) {
            break;
        }
        printf("  [!] Opcao invalida. Digite um numero de 1 a 3.\n");
    }
    ordenar(criterio);
    printf("  Lista ordenada. Resultado:\n");
    listar();
}

/* indicadores: total, quantos por categoria e completos x pendentes */
void op_analise(void) {
    printf("\n-- Analise --\n");
    if (inicio == NULL) {
        printf("  A lista esta vazia. Nada a analisar.\n");
        return;
    }
    printf("  Total de atendimentos: %d\n", total);

    printf("  Por categoria:\n");
    for (int c = 1; c <= 5; c++) {
        int quantidade = 0;
        Atendimento *p = inicio;
        while (p != NULL) {
            if (p->categoria == c) {
                quantidade++;
            }
            p = p->prox;
        }
        printf("    - %-20s %d\n", categorias[c], quantidade);
    }

    int completos = 0;
    Atendimento *p = inicio;
    while (p != NULL) {
        if (p->faltam == 0) {
            completos++;
        }
        p = p->prox;
    }
    printf("  Documentos completos: %d\n", completos);
    printf("  Documentos pendentes: %d\n", total - completos);
}

/* atendimentos so de exemplo, pra testar e apresentar */
void op_exemplos(void) {
    int cats[6]   = {1, 2, 3, 4, 5, 1};
    int peds[6]   = {2, 1, 3, 1, 2, 3};
    int faltas[6] = {0, 0, 1, 0, 2, 0};

    for (int i = 0; i < 6; i++) {
        inserir_fim(cats[i], peds[i], faltas[i]);
    }
    printf("\n  6 atendimento(s) de exemplo carregado(s).\n");
}

/* ---------------- menu ---------------- */

void mostrar_menu(void) {
    printf("\n==============================================\n");
    printf("  CITIZEN ASSIST AI  -  Documentacao e Cidadania\n");
    printf("==============================================\n");
    printf("  1) Novo atendimento\n");
    printf("  2) Consultar atendimento por protocolo\n");
    printf("  3) Pesquisar por categoria\n");
    printf("  4) Atualizar documentos de um atendimento\n");
    printf("  5) Excluir atendimento\n");
    printf("  6) Ordenar lista\n");
    printf("  7) Listar todos\n");
    printf("  8) Analise (indicadores)\n");
    printf("  9) Carregar atendimentos de exemplo\n");
    printf("  0) Sair\n");
    printf("  Opcao > ");
}

int main(void) {
    printf("\n*** AVISO ***\n");
    printf("Prototipo educacional. NAO e um canal oficial, NAO confirma a\n");
    printf("concessao de servicos ou direitos e NAO agenda de verdade.\n");
    printf("Nao informe dados pessoais nem numeros de documentos.\n");
    printf("Consulte sempre as fontes oficiais (gov.br).\n");

    while (1) {
        mostrar_menu();

        switch (ler_numero()) {
            case 1:
                op_novo();
                break;
            case 2:
                op_consultar();
                break;
            case 3:
                op_pesquisar();
                break;
            case 4:
                op_atualizar();
                break;
            case 5:
                op_excluir();
                break;
            case 6:
                op_ordenar();
                break;
            case 7:
                printf("\n-- Atendimentos --\n");
                listar();
                break;
            case 8:
                op_analise();
                break;
            case 9:
                op_exemplos();
                break;
            case 0:
                sair("\n  Encerrando. Memoria liberada. Ate logo!\n");
                break;
            default:
                printf("  [!] Opcao invalida. Digite um numero de 0 a 9.\n");
        }
    }
    return 0;
}
