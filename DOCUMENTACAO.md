Global Solution 2026.2 - Data Structures and Algorithms - Turma 1CCA

## 1. Recorte do problema

Orientar o cidadão sobre documentação e serviços públicos básicos. Ele escolhe a categoria, diz quais documentos tem e o que quer fazer. O programa mostra o que falta e guarda o atendimento numa lista.

Limites: não é canal oficial, não agenda de verdade, não pede dados pessoais nem números de documentos. As categorias e os documentos de cada uma são exemplos.

## 2. Modelo do registro

```c
typedef struct Atendimento {
    int id;          // protocolo (automático, 1, 2, 3...)
    int categoria;   // 1 a 5
    int pedido;      // 1 a 3
    int faltam;      // quantos documentos faltam (0 = completo)
    int dia;         // dia do atendimento (0 = ainda sem data)
    int mes;         // mês do atendimento (0 = ainda sem data)
    struct Atendimento *prox;
} Atendimento;
```

Só números. A data é escolhida pelo cidadão (mês e dia de 2026; o ano é fixo na constante `ANO`). Os nomes (categoria, pedido, documentos) ficam em vetores de texto, então não guardamos nenhum dado pessoal.

O protocolo é gerado pelo programa com um contador (`proximo_id`) que só aumenta. Por isso não existe ID duplicado e um protocolo excluído não é reaproveitado.

## 3. Lista encadeada

Lista simples com ponteiro `inicio`. As funções são nossas, sem biblioteca:

| Função | O que faz |
|---|---|
| `inserir_fim` | cria o nó com malloc e coloca no fim |
| `buscar_id` | percorre a lista procurando o protocolo |
| `remover` | religa o anterior no próximo e dá free (trata primeiro, meio e último) |
| `liberar_lista` | dá free em todos os nós ao sair |
| `listar` | percorre e mostra todos |

## 4. Menu

| Opção | Função |
|---|---|
| 1 | Novo atendimento (categoria com switch, documentos s/n, pedido e data) |
| 2 | Consultar por protocolo |
| 3 | Pesquisar por categoria |
| 4 | Atualizar documentos de um atendimento |
| 5 | Excluir |
| 6 | Ordenar (protocolo, categoria, pedido ou data) |
| 7 | Listar |
| 8 | Análise (indicadores) |
| 9 | Carregar exemplos |
| 0 | Sair |

**Data:** só é pedida quando os documentos estão completos. O mês vai de 1 a 12 e o dia é conferido pelo mês (vetor `dias_do_mes`; 31 de novembro e 29/02 são rejeitados, 2026 não é bissexto). Se o atendimento está pendente, a data fica vazia e é pedida na opção 4, quando ele passar a completo. O programa não compara com a data de hoje, então não bloqueia datas passadas.

Tratamentos: opção inválida (texto, número fora da faixa, negativo, decimal, vazio), lista vazia, protocolo inexistente e pesquisa sem resultado. A leitura usa `fgets` e confere se só tem dígitos.

## 5. Algoritmos

**Busca sequencial.** Usada na busca por protocolo e na pesquisa por categoria. Busca binária não serve numa lista encadeada, porque não dá para ir direto ao meio.

**Selection Sort.** A cada passo acha o menor elemento do resto da lista e troca os dados com o da posição atual (os ponteiros `prox` não mudam). Ordena por protocolo, categoria, pedido ou data (mês e dia; atendimentos sem data vão para o fim). Não é estável: quando dois atendimentos têm o mesmo valor, a ordem entre eles pode mudar.

## 6. Indicadores (opção 8)

- total de atendimentos
- quantidade por categoria
- documentos completos
- documentos pendentes

## 7. Análise de complexidade (n = número de atendimentos)

| Operação | Tempo | Observação |
|---|---|---|
| Inserir no fim | O(n) | percorre até o último nó |
| Buscar por protocolo | O(n) pior caso, O(1) melhor | sequencial |
| Pesquisar por categoria | O(n) | vê todos |
| Atualizar | O(n) | busca + troca de campos |
| Excluir | O(n) | busca + religar ponteiros |
| Listar | O(n) | |
| Indicadores | O(n) | uma passada |
| Selection Sort | O(n²) sempre | n(n-1)/2 comparações, no máximo n-1 trocas |
| Espaço | O(n) | um nó por atendimento |

Para o tamanho de um protótipo, O(n²) no Selection Sort não pesa. Para uma base grande seria melhor um algoritmo O(n log n).

## 8. Testes

Rodar com `make test`. São 102 casos automáticos mais a verificação do Valgrind. O detalhe de cada um (entrada, esperado, resultado) está em `evidencias_testes.txt`.

| Grupo | Exemplos |
|---|---|
| Lista vazia | listar, consultar, pesquisar, atualizar, excluir, ordenar e análise avisam que está vazia |
| Normal | atendimento completo, pedidos 1 a 3, protocolos em sequência |
| Documentos faltando | mostra qual falta, fica pendente, orienta usar a opção 4 |
| Categorias | as 5 do switch, voltar sem cadastrar, cancelar o pedido |
| Consulta e pesquisa | protocolo existente, inexistente, categoria com e sem resultado |
| Atualização e exclusão | pendente vira completo e o contrário; excluir primeiro, meio e último; protocolo não reaproveitado |
| Data | mês e dia inválidos (0, 13, texto, 32), 31/11, 29/02, 28/02 válido, pendente sem data, data pedida ao completar |
| Ordenação | por categoria, por pedido, por data, lista de 1 elemento, lista já ordenada, voltar por protocolo |
| Análise | totais da carga de exemplos (6 atendimentos, 4 completos, 2 pendentes) |
| Entradas inválidas | `abc`, `99`, `-1`, `3.5`, vazio, linha muito longa, s/n inválido |
| Encerramento | opção 0, fim da entrada (EOF), avisos iniciais |
| Memória | Valgrind sem vazamentos (todos os blocos liberados, 0 erros) |

## 9. Roteiro de demonstração

1. `./citizen_assist` e mostrar o aviso inicial.
2. Opção 9: carrega os exemplos. Opção 7: lista (com a coluna de data).
3. Opção 1: escolher Saúde, responder `s` nos três documentos, escolher "Agendar atendimento" e digitar a data (mês 12, dia 10, por exemplo). Mostrar o protocolo.
4. Opção 1 de novo: responder `n` em um documento e mostrar a situação pendente.
5. Opção 4: atualizar esse atendimento com `s`, escolher a data e ver que virou completo.
6. Opção 2: consultar um protocolo. Depois um inexistente (999).
7. Opção 3: pesquisar categoria.
8. Opção 6: ordenar por data e listar.
9. Opção 5: excluir um protocolo e listar.
10. Opção 8: indicadores.
11. Tentar mês 13 ou 31/11 para mostrar a validação da data. Digitar `abc` no menu para mostrar o tratamento de erro. Opção 0 para sair.
