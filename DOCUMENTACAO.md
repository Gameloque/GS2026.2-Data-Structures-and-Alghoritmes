# Citizen Assist AI - Documentação técnica (DSA)

Global Solution 2026.2 - Data Structures and Algorithms - Turma 1CC-FEV
Linguagem: C

Integrantes: (preencher com nome completo e matrícula de todos)

## 1. Recorte escolhido

Escolhemos o recorte de **documentação e cidadania**. O programa guarda informações sobre serviços e documentos que o cidadão pode precisar (CPF, RG, título de eleitor, CadÚnico, consulta na UBS, matrícula na rede pública, etc.) e permite cadastrar, consultar, pesquisar, atualizar, excluir, ordenar e ver indicadores desses registros.

Limitações:

- É um protótipo de estudo. Não é canal oficial e não confirma concessão de serviço ou direito nenhum. Esse aviso aparece quando o programa inicia.
- Os dados de exemplo são só para teste e podem estar incompletos ou desatualizados. O certo é confirmar no gov.br.
- Não coletamos dado pessoal, porque os registros são de serviços e não de pessoas.
- Os dados ficam só na memória e se perdem ao fechar o programa (não fizemos persistência em arquivo).

## 2. Modelo dos registros

Cada registro é um `Servico` com os campos abaixo.

| Campo | Tipo | Tamanho máximo | Observação |
|---|---|---|---|
| id | int | - | inteiro positivo e único |
| titulo | char[64] | 63 | obrigatório |
| categoria | char[32] | 31 | obrigatório |
| descricao | char[200] | 199 | obrigatório |
| requisitos | char[200] | 199 | obrigatório |
| local | char[64] | 63 | obrigatório |
| prox | ponteiro | - | aponta para o próximo registro |

```c
typedef struct Servico {
    int  id;
    char titulo[TAM_TITULO];
    char categoria[TAM_CATEGORIA];
    char descricao[TAM_DESCRICAO];
    char requisitos[TAM_REQUISITOS];
    char local[TAM_LOCAL];
    struct Servico *prox;
} Servico;

typedef struct {
    Servico *inicio;
    int total;
} Lista;
```

## 3. Lista encadeada

É uma lista simplesmente encadeada feita por nós, com `struct`, `typedef`, ponteiros e `malloc`/`free`.

- `lista_inserir_fim`: percorre a lista conferindo se o id já existe e guardando o último nó. Se o id for novo, aloca o nó com `malloc` e liga no final. Devolve 0 (ok), -1 (id repetido) ou -2 (sem memória).
- `lista_buscar_id`: anda pelos nós seguindo o `prox` até achar o id ou chegar em `NULL`.
- `lista_remover`: guarda o nó anterior e faz `ant->prox = p->prox`. Se for o primeiro nó, muda o `inicio`. Depois dá `free` e diminui o `total`.
- `lista_liberar`: guarda o `prox` antes de cada `free`, para não perder o resto da lista.

Para manter a lista íntegra, a remoção trata separadamente o primeiro, o do meio e o último nó (testamos os três). A atualização só muda os dados do nó, sem mexer no `prox`, e o id não pode ser alterado, então nunca aparece id repetido. O `total` é atualizado em toda inserção e remoção. Ao sair, toda a memória é liberada; o Valgrind confirmou que não há vazamento.

## 4. Algoritmos

### Busca

- Por id (`lista_buscar_id`): busca sequencial. Como a lista encadeada não tem acesso por posição, não dá para usar busca binária, então a sequencial é a que faz sentido aqui.
- Por título ou categoria (`op_pesquisar`): o usuário escolhe o campo e digita um termo. O programa percorre a lista e usa a função `contem_ci`, que procura o termo dentro do texto sem diferenciar maiúscula de minúscula. Mostra todos os registros que batem e, se nenhum bater, avisa que não teve resultado.

### Ordenação (Insertion Sort)

Está em `ordenar_insercao`. Não usamos `qsort` nem função pronta. Dá para ordenar por id, título ou categoria.

Funciona assim: pega cada nó da lista original e encaixa na posição certa de uma lista nova que já está ordenada. Só os ponteiros mudam, os dados não são copiados.

```c
Servico *ordenada = NULL;
Servico *atual = l->inicio;
while (atual != NULL) {
    Servico *prox = atual->prox;
    if (ordenada == NULL || comparar(atual, ordenada, c) < 0) {
        atual->prox = ordenada;
        ordenada = atual;
    } else {
        Servico *p = ordenada;
        while (p->prox != NULL && comparar(p->prox, atual, c) <= 0)
            p = p->prox;
        atual->prox = p->prox;
        p->prox = atual;
    }
    atual = prox;
}
l->inicio = ordenada;
```

O algoritmo é estável, porque o `<= 0` faz os iguais ficarem na ordem que estavam. Testamos ordenando os 6 exemplos por categoria: os dois de *Documentacao* (ids 1 e 2) continuaram nessa ordem.

### Indicadores

A opção de análise mostra o total de registros e quantos há em cada categoria. Para cada nó ela olha se a categoria já apareceu antes na lista; se não apareceu, percorre a lista contando.

## 5. Menu e tratamento de erros

| Opção | O que faz | O que é tratado |
|---|---|---|
| 1 | Cadastrar | id inválido (texto, zero, negativo, decimal), id repetido, campo vazio, texto grande demais |
| 2 | Consultar por id | lista vazia, id inválido, id que não existe |
| 3 | Pesquisar | lista vazia, submenu inválido, termo vazio, nenhum resultado |
| 4 | Atualizar | lista vazia, id inválido ou inexistente (Enter mantém o valor antigo) |
| 5 | Excluir | lista vazia, id inválido ou inexistente |
| 6 | Ordenar | lista vazia, critério inválido |
| 7 | Listar todos | lista vazia |
| 8 | Análise | lista vazia |
| 9 | Carregar exemplos | ids que já existem são ignorados |
| 0 | Sair | libera a memória |

Opção de menu inválida (texto, número fora de 0 a 9, negativo ou vazio) mostra uma mensagem e volta para o menu. A leitura é feita com `fgets` e os números com `strtol`, conferindo se a conversão deu certo. Se a entrada acabar (EOF), o programa encerra liberando a memória.

## 6. Análise Big-O

n é o número de registros. Os campos de texto têm tamanho máximo fixo, então comparar campos conta como custo constante.

| Operação | Melhor caso | Pior caso | Por quê |
|---|---|---|---|
| Inserção | O(1) | O(n) | percorre a lista para ver se o id existe e achar o último nó (não temos ponteiro para o fim) |
| Busca por id | O(1) | O(n) | busca sequencial; melhor caso é o primeiro nó, pior é o último ou id inexistente |
| Busca por título/categoria | O(n) | O(n) | sempre passa por todos os nós para mostrar todas as ocorrências; o custo da comparação de texto por nó é limitado pelo tamanho do campo |
| Remoção | O(1) | O(n) | melhor caso é remover o primeiro; pior é o último ou id inexistente; religar o ponteiro e dar free é O(1) |
| Atualização | O(1) | O(n) | o custo é o da busca pelo id |
| Ordenação (Insertion Sort) | O(n) | O(n²) | melhor caso: lista já em ordem decrescente (cada nó vira o primeiro da lista nova); pior caso: lista já crescente (cada nó percorre a lista nova toda, cerca de n²/2 comparações); memória extra O(1) |
| Listar | O(n) | O(n) | percorre todos os nós |
| Análise | O(n) | O(n²) | pior caso quando todas as categorias são diferentes |
| Liberar a lista | O(n) | O(n) | um free por nó |

Em espaço, a lista ocupa O(n) (um nó por registro) e nenhuma operação cria estrutura extra proporcional a n.

## 7. Testes

Os testes estão no `run_tests.sh`. Cada caso manda entradas para o programa e confere a saída. O script grava no `evidencias_testes.txt` a entrada, o resultado esperado, o observado e o status de cada um.

Resultado da última execução: 51 testes, 51 passaram.

| Grupo | Qtde | O que testa |
|---|---|---|
| Lista vazia | 7 | listar, consultar, pesquisar, atualizar, excluir, ordenar e analisar sem nenhum registro |
| Casos normais | 21 | cadastro, consulta, exemplos, pesquisa por título e categoria, atualização, exclusão do primeiro/meio/último, ordenação (conferindo a ordem dos ids), estabilidade, lista com 1 elemento e indicadores |
| Registros inexistentes | 4 | consultar, atualizar e excluir id que não existe; pesquisa sem resultado |
| Entradas inválidas | 13 | opção de menu inválida, id inválido, campo vazio, texto grande, submenus inválidos |
| Id duplicado | 3 | rejeita id repetido, total não muda, exemplos repetidos são ignorados |
| Encerramento | 3 | sair pelo 0, fim de entrada (EOF) e aviso inicial |

Alguns exemplos (do `evidencias_testes.txt`):

| Teste | Entrada | Esperado | Observado |
|---|---|---|---|
| Ordenar por título | cadastros 20, 10, 30 e ordenar por título | ids 30, 10, 20 | 30,10,20 |
| Estabilidade | 6 exemplos ordenados por categoria | ids 4, 3, 1, 2, 6, 5 | 4,3,1,2,6,5 |
| Id duplicado | cadastrar o id 10 duas vezes | rejeita, continua com 1 registro | "Ja existe um servico com o ID 10", Total: 1 |
| Id inexistente | consultar o id 999 | mensagem de não encontrado | "Nenhum servico encontrado com o ID 999" |
| Pesquisa sem resultado | título "xyzabc" | mensagem de sem resultado | `Nenhum resultado para "xyzabc"` |
| Lista vazia | opção 7 sem cadastros | mensagem de lista vazia | "A lista esta vazia..." |
| Opção inválida | digitar `abc` | rejeita e volta ao menu | "Opcao invalida..." |



