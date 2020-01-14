    /* ==========================================================================
     * Universidade Federal de São Carlos - Campus Sorocaba
     * Disciplina: Organizacao e Recuperacao da Informacao
     * Prof. Tiago A. Almeida
     *
     * Trabalho 03A - Hashing com reespalhamento linear
     *
     * RA: 726544
     * Aluno: Jesimiel Efraim Dias
     * ========================================================================== */
     
    /* Bibliotecas */
    #include <math.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <ctype.h>
    #include <string.h>
    #include <math.h>
     
    /* Tamanho dos campos dos registros */
    #define TAM_PRIMARY_KEY 11
    #define TAM_NOME 31
    #define TAM_GENERO 2
    #define TAM_NASCIMENTO 11
    #define TAM_CELULAR 16
    #define TAM_VEICULO 31
    #define TAM_PLACA 9
    #define TAM_DATA 9
    #define TAM_HORA 6
    #define TAM_TRAJETO 121
    #define TAM_VALOR 7
    #define TAM_VAGAS 2
     
    #define TAM_REGISTRO 256
    #define MAX_REGISTROS 1000
    #define TAM_ARQUIVO (MAX_REGISTROS * TAM_REGISTRO + 1)
     
    #define POS_OCUPADA "[%d] Ocupado: %s\n"
    #define POS_LIVRE "[%d] Livre\n"
    #define POS_REMOVIDA "[%d] Removido\n"
     
    /* Estado das posições da tabela hash */
    #define LIVRE 0
    #define OCUPADO 1
    #define REMOVIDO 2
     
    /* Saídas do usuário */
    #define OPCAO_INVALIDA "Opcao invalida!\n"
    #define MEMORIA_INSUFICIENTE "Memoria insuficiente!\n"
    #define REGISTRO_N_ENCONTRADO "Registro(s) nao encontrado!\n"
    #define CAMPO_INVALIDO "Campo invalido! Informe novamente.\n"
    #define ERRO_PK_REPETIDA "ERRO: Ja existe um registro com a chave primaria: %s.\n\n"
    #define ARQUIVO_VAZIO "Arquivo vazio!"
    #define INICIO_BUSCA "********************************BUSCAR********************************\n"
    #define INICIO_LISTAGEM "********************************LISTAR********************************\n"
    #define INICIO_ALTERACAO "********************************ALTERAR*******************************\n"
    #define INICIO_ARQUIVO "********************************ARQUIVO*******************************\n"
    #define INICIO_EXCLUSAO "**********************EXCLUIR*********************\n"
     
    #define SUCESSO "OPERACAO REALIZADA COM SUCESSO!\n"
    #define FALHA "FALHA AO REALIZAR OPERACAO!\n"
    #define ERRO_TABELA_CHEIA "ERRO: Tabela Hash esta cheia!\n\n"
    #define REGISTRO_INSERIDO "Registro %s inserido com sucesso. Numero de colisoes: %d.\n\n"
     
    /* Registro da Carona */
    typedef struct
    {
        char pk[TAM_PRIMARY_KEY];
        char nome[TAM_NOME];
        char genero[TAM_GENERO];
        char nascimento[TAM_NASCIMENTO]; /* DD/MM/AAAA */
        char celular[TAM_CELULAR];
        char veiculo[TAM_VEICULO];
        char placa[TAM_PLACA];
        char trajeto[TAM_TRAJETO];
        char data[TAM_DATA];   /* DD/MM/AA, ex: 24/09/19 */
        char hora[TAM_HORA];   /* HH:MM, ex: 07:30 */
        char valor[TAM_VALOR]; /* 999.99, ex: 004.95 */
        char vagas[TAM_VAGAS];
    } Carona;
     
    /* Registro da Tabela Hash
     * Contém o estado da posição, a chave primária e o RRN do respectivo registro */
    typedef struct
    {
        int estado;
        char pk[TAM_PRIMARY_KEY];
        int rrn;
    } Chave;
     
    /* Estrutura da Tabela Hash */
    typedef struct
    {
        int tam;
        Chave *v;
    } Hashtable;
     
    /* Variáveis globais */
    char ARQUIVO[TAM_ARQUIVO];
    int nregistros;
    int nregistros_ex;
    /* ==========================================================================
     * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
     * ========================================================================== */
     
    /* Recebe do usuário uma string simulando o arquivo completo. */
    void carregar_arquivo();
     
    /* Exibe a Carona */
    int exibir_registro(int rrn);
     
    /*Recupera um registro do arquivo de dados*/
    Carona recuperar_registro(int rrn);
     
    /*Gera Chave da struct Carona*/
    void gerarChave(Carona *novo);
     
    /*Retorna o primeiro número primo >= a*/
    int prox_primo(int a);
     
    /*Auxiliar para a função de hash*/
    //Basicamenete, pega um caracter e transforma no número apropriado
    short f(char x);
     
    /*Função de Hash*/
    short hash(const char *chave, int tam);
     
    /*Funcoes do menu*/
    void ler_entrada(char *registro, Carona *novo);
    void cadastrar(Hashtable *tabela);
    int alterar(Hashtable tabela);
    void buscar(Hashtable tabela);
    int remover(Hashtable *tabela);
    void imprimir_tabela(Hashtable tabela);
    void liberar_tabela(Hashtable *tabela);
     
    /*Funcoes auxiliares*/
    void criar_tabela(Hashtable *tabela, int tam);
    void carregar_tabela(Hashtable *tabela);
     
     
    /* <<< DECLARE AQUI OS PROTOTIPOS >>> */
    void complementar_registro(char* registro);
    //Retorna 1 quando inseriu em um determinado endereço ou 0 quando não inseriu.
    int inserir_tabela(Hashtable *tabela, char *chave, int rrn);
    int buscar_tabela(Hashtable tabela, char *chave);
    void gravarArquivo(char *registro, int nregistros);
    /* ==========================================================================
     * ============================ FUNÇÃO PRINCIPAL ============================
     * =============================== NÃO ALTERAR ============================== */
    int main()
    {
        /* Arquivo */
        int carregarArquivo = nregistros = nregistros_ex = 0;
        scanf("%d%*c", &carregarArquivo); // 1 (sim) | 0 (nao)
        if (carregarArquivo)
            carregar_arquivo();
     
        /* Tabela Hash */
        int tam;
        scanf("%d%*c", &tam);
        tam = prox_primo(tam);
     
        Hashtable tabela;
        criar_tabela(&tabela, tam);
     
        if(carregarArquivo)
            carregar_tabela(&tabela);
     
        /* Execução do programa */
        int opcao = 0;
        while (opcao != 6)
        {
            scanf("%d%*c", &opcao);
            switch (opcao)
            {
                case 1:
                    cadastrar(&tabela);
                    break;
                case 2:
                    printf(INICIO_ALTERACAO);
                    if (alterar(tabela))
                        printf(SUCESSO);
                    else
                        printf(FALHA);
                    break;
                case 3:
                    printf(INICIO_BUSCA);
                    buscar(tabela);
                    break;
                case 4:
                    printf(INICIO_EXCLUSAO);
                    if (remover(&tabela))
                        printf(SUCESSO);
                    else
                        printf(FALHA);
                    break;
                case 5:
                    printf(INICIO_LISTAGEM);
                    imprimir_tabela(tabela);
                    break;
                case 6:
                    liberar_tabela(&tabela);
                    break;
                case 7:
                    printf(INICIO_ARQUIVO);
                    printf("%s\n", (*ARQUIVO != '\0') ? ARQUIVO : ARQUIVO_VAZIO);
                    break;
                default:
                    printf(OPCAO_INVALIDA);
                    break;
            }
        }
        return 0;
    }
     
    /* Recebe do usuário uma string simulando o arquivo completo. */
    void carregar_arquivo()
    {
        scanf("%[^\n]%*c", ARQUIVO);
        nregistros = strlen(ARQUIVO) / TAM_REGISTRO;
    }
     
    /* Exibe a Carona */
    int exibir_registro(int rrn)
    {
        if (rrn < 0)
            return 0;
     
        Carona j = recuperar_registro(rrn);
        char *traj, trajeto[TAM_TRAJETO];
     
        printf("%s\n", j.pk);
        printf("%s\n", j.nome);
        printf("%s\n", j.genero);
        printf("%s\n", j.nascimento);
        printf("%s\n", j.celular);
        printf("%s\n", j.veiculo);
        printf("%s\n", j.placa);
        printf("%s\n", j.data);
        printf("%s\n", j.hora);
        printf("%s\n", j.valor);
        printf("%s\n", j.vagas);
     
        strcpy(trajeto, j.trajeto);
     
        traj = strtok(trajeto, "|");
     
        while (traj != NULL)
        {
            printf("%s", traj);
            traj = strtok(NULL, "|");
            if (traj != NULL)
            {
                printf(", ");
            }
        }
     
        printf("\n");
     
        return 1;
    }
     
    void liberar_tabela(Hashtable *tabela)
    {
        free(tabela->v);
        tabela->v = NULL;
    }
     
    /* Recupera do arquivo o registro com o rrn informado e retorna os dados na
     * struct Carona */
    Carona recuperar_registro(int rrn)
    {
        char temp[257], *p;
        strncpy(temp, ARQUIVO + ((rrn)*TAM_REGISTRO), TAM_REGISTRO);
        temp[TAM_REGISTRO] = '\0';
        Carona j;
     
        p = strtok(temp, "@");
        strcpy(j.nome, p);
        p = strtok(NULL, "@");
        strcpy(j.genero, p);
        p = strtok(NULL, "@");
        strcpy(j.nascimento, p);
        p = strtok(NULL, "@");
        strcpy(j.celular, p);
        p = strtok(NULL, "@");
        strcpy(j.veiculo, p);
        p = strtok(NULL, "@");
        strcpy(j.placa, p);
        p = strtok(NULL, "@");
        strcpy(j.trajeto, p);
        p = strtok(NULL, "@");
        strcpy(j.data, p);
        p = strtok(NULL, "@");
        strcpy(j.hora, p);
        p = strtok(NULL, "@");
        strcpy(j.valor, p);
        p = strtok(NULL, "@");
        strcpy(j.vagas, p);
     
        gerarChave(&j);
     
        return j;
    }
     
    short f(char x)
    {
        return (x < 59) ? x - 48 : x - 54;
    }
     
    /* <<< IMPLEMENTE AQUI AS FUNCOES >>> */
     
    /*Gera Chave da struct Carona*/
    void gerarChave(Carona *novo){
        novo->pk[0] = toupper(novo->nome[0]);
        novo->pk[1] = toupper(novo->placa[0]);
        novo->pk[2] = toupper(novo->placa[1]);
        novo->pk[3] = toupper(novo->placa[2]);
        novo->pk[4] = toupper(novo->data[0]);
        novo->pk[5] = toupper(novo->data[1]);
        novo->pk[6] = toupper(novo->data[3]);
        novo->pk[7] = toupper(novo->data[4]);
        novo->pk[8] = toupper(novo->hora[0]);
        novo->pk[9] = toupper(novo->hora[1]);
        novo->pk[10] = '\0';
    }
     
    /*Funcoes do menu*/
    void ler_entrada(char *registro, Carona *novo){
     
            //Recebemos as entradas até o usuário dar o \n.
        //O *c descarta o \n impedindo que continue no buffer.
        scanf("%[^\n]%*c", novo->nome);
        scanf("%[^\n]%*c", novo->genero);
        scanf("%[^\n]%*c", novo->nascimento);
        scanf("%[^\n]%*c", novo->celular);
        scanf("%[^\n]%*c", novo->veiculo);
        scanf("%[^\n]%*c", novo->placa);
        scanf("%[^\n]%*c", novo->trajeto);
        scanf("%[^\n]%*c", novo->data);
        scanf("%[^\n]%*c", novo->hora);
        scanf("%[^\n]%*c", novo->valor);
        scanf("%[^\n]%*c", novo->vagas);
     
        //Gravamos as entradas em uma string registro.
        sprintf(registro,"%s@%s@%s@%s@%s@%s@%s@%s@%s@%s@%s@",
        novo->nome,
        novo->genero,
        novo->nascimento,
        novo->celular,
        novo->veiculo,
        novo->placa,
        novo->trajeto,
        novo->data,
        novo->hora,
        novo->valor,
        novo->vagas);
    }
     
    //Complementamos o registro com #.
    void complementar_registro(char* registro){
        int i;
        for(i = strlen(registro); i < TAM_REGISTRO; i++){
            strcat(registro,"#");
        }
        registro[TAM_REGISTRO] = '\0';
    }
     
    int prox_primo(int a){
        int eh_primo = 0; 
        int n,i;
        //Primeiramente, verificamos se é par.
        if(a % 2 == 0){
            a+=1;
        }
        //Enquanto não achar o primo continuamos.
        while(!eh_primo){
            //Sabemos que temos de verificar até a raiz quadrada.
            //Coloquei +2 para garantir que verifique até o teto.
            n = (int)sqrt(a)+2;
            //Somamos +2 para verificar apenas para os ímpares.
            for(i = 3; i < n; i+=2){
                if(a % i == 0){
                    n = -1;
                }
            }
            if(i >= n && n != -1){
                eh_primo = 1;
            } else {
                a+=2;
            }
        }
     
        return a;
    }
     
    void criar_tabela(Hashtable *tabela, int tam){
        tabela->tam = tam;
        tabela->v = (Chave*) malloc(sizeof(Chave) * tam);

        for(int i = 0; i < tabela->tam; i++) tabela->v[i].estado = LIVRE;
    }
     
     
    short hash(const char *chave, int tam){
        int acumulador = 0;
     
        //Fazemos o endereço hash através dos 8 primeiros caracteres da chave pk.
        for(int i = 0; i < 8; i++){
            //E multiplicamos cada caracter pelo seu indice do vetor +1.
            acumulador += (i+1)*f(chave[i]);
        }
        //Retornamos o endereço.
        acumulador = (acumulador %tam);
        return acumulador;
    }
     
    void carregar_tabela(Hashtable *tabela){
        int endereco;
        Carona carona;
     
        for(int i = 0; i < nregistros; i ++){
            //Recuperamos o registro;
            carona = recuperar_registro(i);
            inserir_tabela(tabela,carona.pk,i);
        }
    }
     
    void gravarArquivo(char *registro, int nregistros){
        complementar_registro(registro);    
        strcat(ARQUIVO,registro);
    }
     
    void cadastrar(Hashtable *tabela){
        Carona novo;
        char registro[TAM_REGISTRO+1];
        
        ler_entrada(registro,&novo);
     
        gerarChave(&novo);
     
        if((nregistros-nregistros_ex) == tabela->tam){
            printf(ERRO_TABELA_CHEIA);
            return;
        }
     
        if(buscar_tabela(*tabela,novo.pk) != -1){
            printf(ERRO_PK_REPETIDA,novo.pk);
            return;
        }
     
        int colisoes = inserir_tabela(tabela,novo.pk, nregistros);
     
        gravarArquivo(registro,nregistros);
        nregistros+=1;
     
        printf(REGISTRO_INSERIDO, novo.pk, colisoes);
    }
     
    int buscar_tabela(Hashtable tabela,char *chave){
        int endereco = hash(chave, tabela.tam);
        int j = 0;
        //Verificao se o endereço da chave não está livre.
        while(tabela.v[endereco].estado != LIVRE && j < tabela.tam){
            //Verificando se o endereço está ocupado, e se estiver, verificamos se é a nossa chave.
            if(tabela.v[endereco].estado == OCUPADO 
            && strcmp(tabela.v[endereco].pk, chave) == 0){
                return endereco;
            }
            j++;
            endereco = (1+endereco)%tabela.tam;
        }
     
        return -1;
    }
     
    int inserir_tabela(Hashtable *tabela, char *chave, int rrn){
        int endereco = hash(chave,tabela->tam);
        int j = 0;
     
        while(tabela->v[endereco].estado == OCUPADO){
            j++;
            endereco = (1+endereco)%tabela->tam;
        }

       // if(tabela->v[endereco].estado == REMOVIDO) nregistros_ex-=1;
        
        strcpy(tabela->v[endereco].pk, chave);
        tabela->v[endereco].estado = OCUPADO;
        tabela->v[endereco].rrn = rrn;
        //j é utilizado como número de colisões.
        return j;    
    }
     
    void buscar(Hashtable tabela){
        char chave[TAM_PRIMARY_KEY];
        scanf("%[^\n]%*c", chave);
     
        int endereco = buscar_tabela(tabela, chave);
     
        if(endereco == -1){
            printf(REGISTRO_N_ENCONTRADO);
            return;
        }
     
        exibir_registro(tabela.v[endereco].rrn);
    }
     
    int alterar(Hashtable tabela){
        char chave[TAM_PRIMARY_KEY];
        int flag = 0;
     
        scanf("%[^\n]%*c", chave);
     
        int endereco = buscar_tabela(tabela, chave);
     
        if(endereco == -1){
            printf(REGISTRO_N_ENCONTRADO);
            return 0;
        }
     
        char vagaString[TAM_VAGAS];
        int vagaInt;
        
        do{
            //Se o loop retornar para o começo a flag vai ser == 1,
            //ou seja, o usuário digitou um valor inválido.
            if(flag){
                printf(CAMPO_INVALIDO);
                flag = 0;
            }
            //Recebemos o valor a ser alterado.
            scanf("%d%*c",&vagaInt);
            flag = 1;
        }while(vagaInt < 1 || vagaInt > 8);
        
        //Colocamos o valor alterado na string.
        sprintf(vagaString,"%d",vagaInt);
     
        //Achamos a posição do registro que será alterada no arquivo.
        int rrn = tabela.v[endereco].rrn * TAM_REGISTRO;
        
        //Para registrar o número de @.
        int ocorrencia = 0;
            
        //Quando ocorrencia == 10 teremos achado o campo de vagas.
        while(ocorrencia != 10){
            if(ARQUIVO[rrn] == '@'){
                ocorrencia++;
            }
            rrn++;
        }
            
        //Alteramos o campo vaga.
        ARQUIVO[rrn] = vagaString[0];
     
        return 1;
    }
     
    int remover(Hashtable *tabela){
        char chave[TAM_PRIMARY_KEY];
        scanf("%[^\n]%*c", chave);
     
        int endereco = buscar_tabela(*tabela,chave);
     
        //Verificamos se existe ou se seu rrn já está excluído.
        if(endereco == -1){
            printf(REGISTRO_N_ENCONTRADO);
            return 0;
        }
            
        //Alteramos o arquivo para evidenciar que o registro foi removido.
        ARQUIVO[tabela->v[endereco].rrn*TAM_REGISTRO] = '*';
        ARQUIVO[tabela->v[endereco].rrn*TAM_REGISTRO+1] = '|';
     
        //E marcamos como removido no índice primário.
        tabela->v[endereco].estado = REMOVIDO;        
        nregistros_ex+=1;
        return 1;    
    }
     
    void imprimir_tabela(Hashtable tabela){
        for(int i = 0; i < tabela.tam; i++){
            if(tabela.v[i].estado == OCUPADO){
                printf(POS_OCUPADA,i,tabela.v[i].pk);
            } else if(tabela.v[i].estado == LIVRE){
                printf(POS_LIVRE,i);
            }
            else{
                printf(POS_REMOVIDA,i);
            }
        }
    }
