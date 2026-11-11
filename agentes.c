
#include <stdio.h>
#include <string.h>

/* ------------------------- Constantes------------------------- */

#define MAXAGENTES 50               
#define TAMNOME    81        
#define TAMSCRIPT  1001   
#define TAMLINHA   1200   
#define ARQUIVO    "agentes.txt" 

/* --------------------------- Dados do catalogo --------------------------- */


int   ids[MAXAGENTES];                   /* identificador de cada agente   */
char  nomes[MAXAGENTES][TAMNOME];        /* nome de cada agente            */
char  scripts[MAXAGENTES][TAMSCRIPT];    /* instrucoes de cada agente      */
float notas[MAXAGENTES];                 /* nota de 0.0 a 5.0              */
int   total = 0;                         /* quantos agentes estao em uso   */

/* ------------------------- Prototipos das funcoes ------------------------- */


void  exibirmenu();                       
int   lertexto(char destino[], int limite); 
int   lerinteiro();                       
int   notavalida(float nota);             
float textoparanota(char texto[]);        
int   textoparainteiro(char texto[]);     
void  copiartexto(char destino[], char origem[]); 
void  removerquebra(char texto[]);        
int   contem(char texto[], char busca[]); 
int   buscarindiceporid(int id);          
void  exibiragente(int pos);              
void  menuagente(int pos);                
void  cadastrar();                        
void  listar();                          
void  listarordenado();                  
void  buscarporid();                      
void  buscarpornome();                    
void  editar(int pos);                    
int   excluir(int pos);                 
void  salvar();                           
void  carregar();                         

/* ==========================================================================
 * LEITURA DE DADOS
 * ========================================================================== */

/* --------------------------------------------------------------------------
 * lertexto: le uma linha inteira do teclado e copia para "destino".
 * ------------------------------------------------------------------------- */
int lertexto(char destino[], int limite)
{
    char linha[TAMLINHA];
    int  i;
    int  c;
    int  achouquebra;

    /* Le a linha do teclado. Se nao conseguir ler nada, devolve vazio. */
    if (fgets(linha, TAMLINHA, stdin) == NULL) {
        destino[0] = '\0';
        return 0;
    }

    /* Procura o '\n' do fim da linha e o substitui por '\0'. */
    achouquebra = 0;
    for (i = 0; linha[i] != '\0'; i++) {
        if (linha[i] == '\n') {
            linha[i] = '\0';
            achouquebra = 1;
            break;
        }
    }

    /* Se nao achou o '\n', a linha era maior que o buffer de leitura.
       Nesse caso, descarta o resto da entrada e informa erro. */
    if (!achouquebra) {
        while ((c = getchar()) != '\n' && c != EOF) {
            /* descarta o restante da linha */
        }
        destino[0] = '\0';
        return -1;
    }

    /* Copia o conteudo lido para o destino, respeitando o limite. */
    i = 0;
    while (linha[i] != '\0' && i < limite - 1) {
        destino[i] = linha[i];
        i++;
    }
    destino[i] = '\0';

    /* Se sobrou texto, o usuario digitou mais do que o destino comporta. */
    if (linha[i] != '\0') {
        destino[0] = '\0';
        return -1;
    }

    /* String vazia: o usuario apenas apertou ENTER. */
    if (i == 0) {
        return 0;
    }
    return 1;
}

/* --------------------------------------------------------------------------
 * lerinteiro: le um numero inteiro do teclado.
 *
 * ------------------------------------------------------------------------- */
int lerinteiro()
{
    int numero;
    int resultado;
    int c;

    resultado = scanf("%d", &numero);

    /* Descarta o '\n' (e tudo o que sobrou) da linha. */
    while ((c = getchar()) != '\n' && c != EOF) {
        /* descarta o restante da linha */
    }

    if (resultado != 1) {
        return -1;
    }
    return numero;
}

/* --------------------------------------------------------------------------
 * notavalida: retorna 1 se a nota esta entre 0.0 e 5.0, ou 0 se nao estiver.
 * ------------------------------------------------------------------------- */
int notavalida(float nota)
{
    return nota >= 0.0 && nota <= 5.0;
}

/* --------------------------------------------------------------------------
 * textoparanota: converte um texto digitado (ex: "4.5" ou "4,5") em float.
 * ------------------------------------------------------------------------- */
float textoparanota(char texto[])
{
    int   i       = 0;
    int   digitos = 0;
    int   pontos  = 0;
    float numero  = 0.0;
    float divisor = 1.0;

    while (texto[i] != '\0') {
        if (texto[i] >= '0' && texto[i] <= '9') {
            digitos++;
            if (pontos == 0) {
                /* Parte inteira do numero. */
                numero = numero * 10.0 + (texto[i] - '0');
            } else {
                /* Parte decimal: cada digito vale uma fracao menor. */
                divisor = divisor * 10.0;
                numero  = numero + (texto[i] - '0') / divisor;
            }
        } else if (texto[i] == '.' || texto[i] == ',') {
            pontos++;
            if (pontos > 1) {
                return -1.0; /* mais de um separador decimal */
            }
        } else {
            return -1.0; /* caractere que nao pode fazer parte de uma nota */
        }
        i++;
    }

    if (digitos == 0) {
        return -1.0; /* nao havia nenhum numero no texto */
    }
    if (!notavalida(numero)) {
        return -1.0; /* fora do intervalo 0.0 a 5.0 */
    }
    return numero;
}

/* --------------------------------------------------------------------------
 * textoparainteiro: converte um texto em int ignorando espacos iniciais.
 * ------------------------------------------------------------------------- */
int textoparainteiro(char texto[])
{
    int i      = 0;
    int numero = 0;

    /* Pula espacos e tabulacoes do inicio. */
    while (texto[i] == ' ' || texto[i] == '\t') {
        i++;
    }

    /* Acumula os digitos encontrados. */
    while (texto[i] >= '0' && texto[i] <= '9') {
        numero = numero * 10 + (texto[i] - '0');
        i++;
    }
    return numero;
}

/* ==========================================================================
 * FUNCOES AUXILIARES DE TEXTO
 * ========================================================================== */

/* --------------------------------------------------------------------------
 * copiartexto: copia "origem" para "destino" caractere por caractere.
 * O destino precisa ter espaco suficiente para o texto + '\0'.
 * ------------------------------------------------------------------------- */
void copiartexto(char destino[], char origem[])
{
    int i = 0;

    while (origem[i] != '\0') {
        destino[i] = origem[i];
        i++;
    }
    destino[i] = '\0';
}

/* --------------------------------------------------------------------------
 * removerquebra: Usada nas linhas lidas do arquivo com fgets.
 * ------------------------------------------------------------------------- */
void removerquebra(char texto[])
{
    texto[strcspn(texto, "\r\n")] = '\0';
}

/* --------------------------------------------------------------------------
 * contem: verifica se "busca" aparece em algum lugar de "texto".
 * ------------------------------------------------------------------------- */
int contem(char texto[], char busca[])
{
    int  i;
    int  j;
    char a;
    char b;

    for (i = 0; texto[i] != '\0'; i++) {
        j = 0;
        for (;;) {
            if (busca[j] == '\0') {
                return 1; /* chegou ao fim da busca: encontrou tudo */
            }
            if (texto[i + j] == '\0') {
                break; /* o texto acabou antes da busca terminar */
            }

            a = texto[i + j];
            b = busca[j];

            /* Converte letras maiusculas para minusculas. */
            if (a >= 'A' && a <= 'Z') {
                a = a + 32;
            }
            if (b >= 'A' && b <= 'Z') {
                b = b + 32;
            }

            if (a != b) {
                break; /* diferenca: tenta comecar na proxima posicao */
            }
            j++;
        }
    }
    return 0;
}

/* --------------------------------------------------------------------------
 * buscarindiceporid: procura o ID no catalogo.
 * Retorno: a posicao do agente (0..total-1) ou -1 se o ID nao existir.
 * ------------------------------------------------------------------------- */
int buscarindiceporid(int id)
{
    int i;

    for (i = 0; i < total; i++) {
        if (ids[i] == id) {
            return i;
        }
    }
    return -1;
}

/* ==========================================================================
 * PERSISTENCIA (ARQUIVO)
 * ========================================================================== */



void salvar()
{
    FILE *arquivo;
    int   i;

    arquivo = fopen(ARQUIVO, "w");
    if (arquivo == NULL) {
        printf("ERRO: nao foi possivel gravar o arquivo %s.\n", ARQUIVO);
        return;
    }

    fprintf(arquivo, "%d\n", total);

    for (i = 0; i < total; i++) {
        fprintf(arquivo, "%d\n",    ids[i]);
        fprintf(arquivo, "%s\n",    nomes[i]);
        fprintf(arquivo, "%s\n",    scripts[i]);
        fprintf(arquivo, "%.2f\n",  notas[i]);
    }

    fclose(arquivo);
}

/* --------------------------------------------------------------------------
 * carregar: le o catalogo do arquivo para a memoria, no inicio do programa.
 * ------------------------------------------------------------------------- */
void carregar()
{
    FILE *arquivo;
    char  linha[TAMLINHA];
    int   quantidade;
    int   i;

    arquivo = fopen(ARQUIVO, "r");
    if (arquivo == NULL) {
        return; /* primeira execucao: ainda nao existe arquivo */
    }

    /* Primeira linha: quantidade de agentes gravados. */
    if (fgets(linha, TAMLINHA, arquivo) == NULL) {
        fclose(arquivo);
        return;
    }

    quantidade = textoparainteiro(linha);
    if (quantidade > MAXAGENTES) {
        quantidade = MAXAGENTES; /* protege o limite dos vetores */
    }

    for (i = 0; i < quantidade; i++) {
        /* ID */
        if (fgets(linha, TAMLINHA, arquivo) == NULL) break;
        ids[i] = textoparainteiro(linha);

        /* Nome */
        if (fgets(nomes[i], TAMNOME, arquivo) == NULL) break;
        removerquebra(nomes[i]);

        /* Instrucoes */
        if (fgets(scripts[i], TAMSCRIPT, arquivo) == NULL) break;
        removerquebra(scripts[i]);

        /* Nota */
        if (fgets(linha, TAMLINHA, arquivo) == NULL) break;
        removerquebra(linha);
        notas[i] = textoparanota(linha);
        if (!notavalida(notas[i])) {
            notas[i] = 0.0; /* valor de seguranca para nota corrompida */
        }

        total++;
    }

    fclose(arquivo);
}

/* ==========================================================================
 * EXIBICAO E MENUS
 * ========================================================================== */

/* --------------------------------------------------------------------------
 * exibiragente: mostra todos os dados de um agente na posicao "pos".
 * ------------------------------------------------------------------------- */
void exibiragente(int pos)
{
    printf("\n==========================================\n");
    printf("ID.........: %d\n", ids[pos]);
    printf("Nome.......: %s\n", nomes[pos]);
    printf("Nota.......: %.1f de 5.0\n", notas[pos]);
    printf("Instrucoes : %s\n", scripts[pos]);
    printf("==========================================\n");
}

/* --------------------------------------------------------------------------
 * menuagente: menu de acoes de um agente especifico (editar / excluir).
 * ------------------------------------------------------------------------- */
void menuagente(int pos)
{
    int opcao;

    for (;;) {
        exibiragente(pos);

        printf("1 - Editar\n");
        printf("2 - Excluir\n");
        printf("3 - Retornar ao menu anterior\n");
        printf("Opcao: ");

        opcao = lerinteiro();

        if (opcao == 1) {
            editar(pos);
        } else if (opcao == 2) {
            if (excluir(pos)) {
                return; /* o agente foi excluido: nao ha mais o que editar */
            }
        } else if (opcao == 3) {
            return;
        } else {
            printf("Opcao invalida. Digite 1, 2 ou 3.\n");
        }
    }
}

/* --------------------------------------------------------------------------
 * exibirmenu: mostra o menu principal do sistema.
 * ------------------------------------------------------------------------- */
void exibirmenu()
{
    printf("\n========== CATALOGO DE AGENTES IA APOLLO ==========\n");
    printf("1 - Cadastrar um novo agente\n");
    printf("2 - Listar os agentes\n");
    printf("3 - Buscar pelo ID\n");
    printf("4 - Buscar pelo nome\n");
    printf("5 - Listar ordenado por nota\n");
    printf("6 - Encerrar o programa\n");
    printf("Opcao: ");
}

/* ==========================================================================
 * OPERACOES DO CATALOGO
 * ========================================================================== */

/* --------------------------------------------------------------------------
 * cadastrar: cadastra um novo agente no fim dos vetores.
 * ------------------------------------------------------------------------- */
void cadastrar()
{
    int   id;
    float nota;
    char  nome[TAMNOME];
    char  instrucoes[TAMSCRIPT];
    char  digitado[32];

    printf("\n--- NOVO AGENTE APOLLO ---\n");

    if (total >= MAXAGENTES) {
        printf("Limite de %d agentes atingido. Exclua um agente para cadastrar outro.\n",
               MAXAGENTES);
        return;
    }

    /* ID: precisa ser positivo e nao existir ainda. */
    printf("ID: ");
    id = lerinteiro();
    if (id <= 0) {
        printf("ID invalido. Nada foi cadastrado.\n");
        return;
    }
    if (buscarindiceporid(id) >= 0) {
        printf("Ja existe um agente com o ID %d. Nada foi cadastrado.\n", id);
        return;
    }

    /* Nome: obrigatorio e dentro do tamanho maximo. */
    printf("Nome: ");
    if (lertexto(nome, TAMNOME) != 1) {
        printf("Nome vazio ou com mais de %d caracteres. Nada foi cadastrado.\n",
               TAMNOME - 1);
        return;
    }

    /* Instrucoes: obrigatorias e dentro do tamanho maximo. */
    printf("Instrucoes do agente:\n> ");
    if (lertexto(instrucoes, TAMSCRIPT) != 1) {
        printf("Instrucoes vazias ou com mais de %d caracteres. Nada foi cadastrado.\n",
               TAMSCRIPT - 1);
        return;
    }

    /* Nota: lida como texto para aceitar '.' ou ',' como separador. */
    printf("Nota: ");
    if (lertexto(digitado, 32) != 1) {
        printf("Nota invalida. Nada foi cadastrado.\n");
        return;
    }
    nota = textoparanota(digitado);
    if (!notavalida(nota)) {
        printf("Nota invalida. Nada foi cadastrado.\n");
        return;
    }

    /* Copia os dados validados para a posicao "total" e soma o contador. */
    ids[total] = id;
    copiartexto(nomes[total], nome);
    copiartexto(scripts[total], instrucoes);
    notas[total] = nota;
    total++;

    salvar();
    printf("Agente %d cadastrado com sucesso.\n", id);
}

/* --------------------------------------------------------------------------
 * listar: lista todos os agentes na ordem em que foram cadastrados.
 * ------------------------------------------------------------------------- */
void listar()
{
    int i;

    if (total == 0) {
        printf("\nO catalogo esta vazio. Use a opcao 1 para cadastrar um agente.\n");
        return;
    }

    printf("\n--- AGENTES CADASTRADOS ---\n");
    for (i = 0; i < total; i++) {
        printf("%2d) ID %d | Nota %.1f | %s\n",
               i + 1, ids[i], notas[i], nomes[i]);
    }
    printf("\nTotal: %d agente(s) de %d possiveis.\n", total, MAXAGENTES);
}

/* --------------------------------------------------------------------------
 * listarordenado: lista os agentes da maior nota para a menor.
 * ------------------------------------------------------------------------- */
void listarordenado()
{
    int mostrado[MAXAGENTES];
    int i;
    int j;
    int melhor;

    if (total == 0) {
        printf("\nO catalogo esta vazio. Use a opcao 1 para cadastrar um agente.\n");
        return;
    }

    /* Nenhum agente foi mostrado ainda. */
    for (i = 0; i < total; i++) {
        mostrado[i] = 0;
    }

    printf("\n--- AGENTES ORDENADOS POR NOTA (maior primeiro) ---\n");
    for (i = 0; i < total; i++) {
        melhor = -1;

        /* Procura entre os agentes ainda nao mostrados o de maior nota. */
        for (j = 0; j < total; j++) {
            if (mostrado[j] == 0) {
                if (melhor == -1 || notas[j] > notas[melhor]) {
                    melhor = j;
                }
            }
        }

        mostrado[melhor] = 1;
        printf("%2d) ID %d | Nota %.1f | %s\n",
               i + 1, ids[melhor], notas[melhor], nomes[melhor]);
    }
}

/* --------------------------------------------------------------------------
 * buscarporid: pede um ID, encontra o agente e abre o menu dele.
 * ------------------------------------------------------------------------- */
void buscarporid()
{
    int id;
    int pos;

    if (total == 0) {
        printf("\nO catalogo esta vazio. Use a opcao 1 para cadastrar um agente.\n");
        return;
    }

    printf("\nID do agente: ");
    id = lerinteiro();
    if (id <= 0) {
        printf("ID invalido. Digite um numero inteiro.\n");
        return;
    }

    pos = buscarindiceporid(id);
    if (pos < 0) {
        printf("Nenhum agente encontrado com o ID %d.\n", id);
        return;
    }

    menuagente(pos);
}

/* --------------------------------------------------------------------------
 * buscarpornome: pede parte do nome, mostra todos os agentes que contem
 * ------------------------------------------------------------------------- */
void buscarpornome()
{
    char busca[TAMNOME];
    int  i;
    int  achou = 0;
    int  id;
    int  pos;

    if (total == 0) {
        printf("\nO catalogo esta vazio. Use a opcao 1 para cadastrar um agente.\n");
        return;
    }

    printf("\nDigite parte do nome: ");
    if (lertexto(busca, TAMNOME) != 1) {
        printf("Busca vazia ou com mais de %d caracteres.\n", TAMNOME - 1);
        return;
    }

    /* Mostra todos os nomes que contem o trecho buscado. */
    printf("\n--- RESULTADOS DA BUSCA ---\n");
    for (i = 0; i < total; i++) {
        if (contem(nomes[i], busca)) {
            printf("ID %d | %s\n", ids[i], nomes[i]);
            achou++;
        }
    }

    if (achou == 0) {
        printf("Nenhum agente com \"%s\" no nome.\n", busca);
        return;
    }

    /* Pede o ID de um dos agentes encontrados. */
    printf("\n%d agente(s) encontrado(s).\n", achou);
    printf("Digite o ID do agente para ver os dados (0 para voltar): ");
    id = lerinteiro();
    if (id <= 0) {
        return;
    }

    pos = buscarindiceporid(id);
    if (pos < 0) {
        printf("O ID %d nao existe no catalogo.\n", id);
        return;
    }
    if (!contem(nomes[pos], busca)) {
        printf("O ID %d nao esta entre os resultados desta busca.\n", id);
        return;
    }

    menuagente(pos);
}

/* --------------------------------------------------------------------------
 * editar: altera os dados de um agente na posicao "pos".
 * ------------------------------------------------------------------------- */
void editar(int pos)
{
    char  buffer[TAMSCRIPT]; /* buffer grande o bastante para qualquer campo */
    int   leitura;
    float nota;

    printf("\n--- EDITAR AGENTE %d ---\n", ids[pos]);
    printf("Aperte ENTER sem digitar nada para manter o valor atual.\n");

    /* Nome */
    printf("Nome atual: %s\n", nomes[pos]);
    printf("Novo nome: ");
    leitura = lertexto(buffer, TAMNOME);
    if (leitura == 1) {
        copiartexto(nomes[pos], buffer);
    } else if (leitura == -1) {
        printf("Nome com mais de %d caracteres. O valor anterior foi mantido.\n",
               TAMNOME - 1);
    }

    /* Nota */
    printf("Nota atual: %.1f\n", notas[pos]);
    printf("Nova nota (0 a 5, ENTER mantem): ");
    leitura = lertexto(buffer, 32);
    if (leitura == 1) {
        nota = textoparanota(buffer);
        if (notavalida(nota)) {
            notas[pos] = nota;
        } else {
            printf("Nota invalida. O valor anterior foi mantido.\n");
        }
    } else if (leitura == -1) {
        printf("Nota invalida. O valor anterior foi mantido.\n");
    }

    /* Instrucoes */
    printf("Instrucoes atuais: %s\n", scripts[pos]);
    printf("Novas instrucoes (ate %d caracteres, ENTER mantem):\n> ",
           TAMSCRIPT - 1);
    leitura = lertexto(buffer, TAMSCRIPT);
    if (leitura == 1) {
        copiartexto(scripts[pos], buffer);
    } else if (leitura == -1) {
        printf("Instrucoes com mais de %d caracteres. O valor anterior foi mantido.\n",
               TAMSCRIPT - 1);
    }

    salvar();
    printf("Alteracoes salvas.\n");
}

/* --------------------------------------------------------------------------
 * excluir: remove o agente na posicao "pos", depois de confirmar.
 * ------------------------------------------------------------------------- */
int excluir(int pos)
{
    int resposta;
    int i;

    printf("Tem certeza que deseja excluir o agente %d (%s)?\n",
           ids[pos], nomes[pos]);
    printf("1 - Sim   0 - Nao: ");

    resposta = lerinteiro();
    if (resposta != 1) {
        printf("Exclusao cancelada.\n");
        return 0;
    }

    /* Desloca os agentes seguintes uma posicao para tras. */
    for (i = pos; i < total - 1; i++) {
        ids[i]     = ids[i + 1];
        notas[i]   = notas[i + 1];
        copiartexto(nomes[i], nomes[i + 1]);
        copiartexto(scripts[i], scripts[i + 1]);
    }
    total--;

    salvar();
    printf("Agente excluido. O catalogo agora tem %d registro(s).\n", total);
    return 1;
}

/* ==========================================================================
 * PROGRAMA PRINCIPAL
 * ========================================================================== */

int main()
{
    int opcao;

    /* Carrega os dados salvos antes de mostrar o menu. */
    carregar();

    printf("Sistema iniciado. %d agente(s) carregado(s) do arquivo %s.\n",
           total, ARQUIVO);

    /* Repete o menu principal ate o usuario escolher a opcao 6. */
    do {
        exibirmenu();
        opcao = lerinteiro();

        switch (opcao) {
            case 1:
                cadastrar();
                break;
            case 2:
                listar();
                break;
            case 3:
                buscarporid();
                break;
            case 4:
                buscarpornome();
                break;
            case 5:
                listarordenado();
                break;
            case 6:
                printf("\nEncerrando o programa...\n");
                break;
            default:
                printf("Opcao invalida. Digite um numero de 1 a 6.\n");
        }
    } while (opcao != 6);

    /* Garante que os dados estao gravados antes de encerrar. */
    salvar();
    printf("Dados gravados em %s. Ate logo!\n", ARQUIVO);
    return 0;
}

