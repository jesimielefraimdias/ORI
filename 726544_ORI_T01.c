    /* ==========================================================================
     * Universidade Federal de São Carlos - Campus Sorocaba
     * Disciplina: Organização de Recuperação da Informação
     * Prof. Tiago A. de Almeida
     *
     * Trabalho 01
     *
     * RA: 726544
     * Aluno: Jesimiel Efraim Dias
     * ========================================================================== */
     
    /* Bibliotecas */
    #include <stdio.h>
    #include <stdlib.h>
    #include <ctype.h>
    #include <string.h>
     
    /* Tamanho dos campos dos registros */
    #define TAM_PRIMARY_KEY 11
    #define TAM_NOME         31
    #define TAM_GENERO         2
    #define TAM_NASCIMENTO     11
    #define TAM_CELULAR     16
    #define TAM_VEICULO     31
    #define TAM_PLACA         9
    #define TAM_DATA         9
    #define TAM_HORA         6
    #define TAM_TRAJETO     121
    #define TAM_VALOR         7
    #define TAM_VAGAS         2
    #define TAM_REGISTRO     256
    #define MAX_REGISTROS     1000
    #define MAX_TRAJETOS     30
    #define TAM_ARQUIVO     (MAX_REGISTROS * TAM_REGISTRO + 1)
     
    /* Saídas do usuário */
    #define OPCAO_INVALIDA                 "Opcao invalida!\n"
    #define MEMORIA_INSUFICIENTE         "Memoria insuficiente!"
    #define REGISTRO_N_ENCONTRADO         "Registro(s) nao encontrado!\n"
    #define CAMPO_INVALIDO                 "Campo invalido! Informe novamente.\n"
    #define ERRO_PK_REPETIDA             "ERRO: Ja existe um registro com a chave primaria: %s.\n"
    #define ARQUIVO_VAZIO                 "Arquivo vazio!\n"
    #define INICIO_BUSCA                 "**********************BUSCAR**********************\n"
    #define INICIO_LISTAGEM             "**********************LISTAR**********************\n"
    #define INICIO_ALTERACAO             "**********************ALTERAR*********************\n"
    #define INICIO_EXCLUSAO             "**********************EXCLUIR*********************\n"
    #define INICIO_ARQUIVO                 "**********************ARQUIVO*********************\n"
    #define INICIO_ARQUIVO_SECUNDARIO     "*****************ARQUIVO SECUNDARIO****************\n"
    #define SUCESSO                     "OPERACAO REALIZADA COM SUCESSO!\n"
    #define FALHA                         "FALHA AO REALIZAR OPERACAO!\n"
     
    /* Registro da Carona */
    typedef struct
    {
        char pk[TAM_PRIMARY_KEY];
        char nome[TAM_NOME];
        char genero[TAM_GENERO];
        char nascimento[TAM_NASCIMENTO];     /* DD/MM/AAAA */
        char celular[TAM_CELULAR];             /*(99) 99999-9999 */
        char veiculo[TAM_VEICULO];
        char placa[TAM_PLACA];                /* AAA-9999 */
        char trajeto[TAM_TRAJETO];
        char data[TAM_DATA];                   /* DD/MM/AA, ex: 24/09/19 */
        char hora[TAM_HORA];                   /* HH:MM, ex: 07:30 */
        char valor[TAM_VALOR];                 /* 999.99, ex: 004.95 */
        char vagas[TAM_VAGAS];
    } Carona;
     
    /*----- Registros dos Índices -----*/
     
    /* Struct para índice Primário */
    typedef struct primary_index
    {
        char pk[TAM_PRIMARY_KEY];
        int rrn;
    } Ip;
     
    /* Struct para índice secundário de nome */
    typedef struct secundary_index
    {
        char pk[TAM_PRIMARY_KEY];
        char nome[TAM_NOME];
    } Is;
     
    /* Struct para índice secundário de data */
    typedef struct secundary_index_of_date
    {
        char pk[TAM_PRIMARY_KEY];
        char data[TAM_DATA];
    } Isd;
     
    /* Struct para índice secundário de hora */
    typedef struct secundary_index_of_time
    {
        char pk[TAM_PRIMARY_KEY];
        char hora[TAM_HORA];
    } Ist;
     
    /* Lista ligada usada no índice com lista invertida */
    typedef struct linked_list
    {
        char pk[TAM_PRIMARY_KEY];
        struct linked_list *prox;
    } ll;
     
    /* Struct para lista invertida de trajetos*/
    typedef struct reverse_index
    {
        char trajeto[TAM_TRAJETO];
        ll *lista;
    } Ir;
     
    /* GLOBAL: ARQUIVO DE DADOS */
    char ARQUIVO[TAM_ARQUIVO];
     
    /* ==========================================================================
     * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
     * ========================================================================== */
     
    /* Recebe do usuário uma string simulando o arquivo completo e retorna o número
     * de registros. */
    int carregar_arquivo();
     
    /* Exibe a Carona */
    int exibir_registro(int rrn);
     
    /* Recupera do arquivo o registro com o rrn informado
     *  e retorna os dados na struct Carona */
    Carona recuperar_registro(int rrn);
     
    /* (Re)faz o índice respectivo */
    void criar_iprimary(Ip *indice_primario, int *nregistros);
     
    /* Realiza os scanfs na struct Carona*/
    void ler_entrada(char *registro, Carona *novo);
     
    /* Gera a chave para determinada struct de carona */ 
    void gerarChave(Carona *novo);
     
    /* Rotina para impressao de indice secundario */
    void imprimirSecundario(Is *idriver, Ir *iroute, Isd *idate, Ist *itime, int nregistros, int ntraj);
     
    /* <<< COLOQUE AQUI OS DEMAIS PROTÓTIPOS DE FUNÇÕES >>> */
    /* ================================================================================================== */
     
    //Completamos com # até que esteja com o tamanho adequado para ser gravado no arquivo.
    void complementar_registro(char* registro);
     
    //Inserindo no índice primário e nos índices secundários.
    void inserir_iprimary(char *pk, Ip *iprimary, int nregistros);
    void inserir_idriver(char *pk, Is *idriver, int nregistros, char *nome);
    void inserir_idate(char *pk, Isd *idate, int nregistros, char *data);
    void inserir_itime(char *pk, Ist *itime, int nregistros, char *hora);
     
    //Nessa função separamos os trajetos dos | e passamos um a um para o insererir_irout.
    int inserir_trajeto_iroute(char *pk, Ir *iroute, int *ntraj, char *trajeto);
    //Inserindo no vetor de trajetos.
    int inserir_iroute(char *pk, Ir *iroute, int *ntraj, char *trajeto);
    //Inserindo na lista apropriada do vetor de trajetos.
    int inserir_iroute_lista(char *pk, Ir *head);
     
    //Criamos todos os índices secundários pedidos.
    void criar_idriver(Is *idriver, int *nregistros);
    void criar_idate(Isd *idate, int *nregistros);
    void criar_itime(Ist *itime, int *nregistros);
    void criar_iroute(Ir *iroute, int *nregistros, int *ntraj);
     
    //Função que mostra as opções de busca.
    void buscar(Ip* iprimary, Is *idriver, Ir *iroute, Isd *idate, Ist *itime, int nregistros, int ntraj);
     
    //Função que mostra as opções de listar.
    void listar(Ip* iprimary, Is *idriver, Ir *iroute, Isd *idate, Ist *itime, int nregistros, int ntraj);
     
    //Buscamos a chave e retornamos -1 caso ela não exista ou esteja excluída.
    int busca_chave(char *pk, Ip *iprimary, int nregistros);
     
    //Faz a busca e verifica se o trajeto já existe.
    int busca_trajeto(Ir *iroute, int ntraj, char *trajeto);
     
    //Liberamos os excluídos nos índices e arquivo.
    void liberar_espaco(Ip* iprimary, Is *idriver, Ir *iroute, Isd *idate, Ist *itime, int *nregistros, int *nregistros_ex, int *ntraj);
     
    //Desalocamos toda memória alocada.
    void finalizar(Ip* iprimary, Is *idriver, Ir *iroute, Isd *idate, Ist *itime, int ntraj);
     
    //Grava na string arquivo.
    void gravarArquivo(char *registro, int nregistros);
     
    //Marca um registro como excluído.
    int remover_registro(char *pk, Ip *indice_primario, int nregistros);
     
    //Altera o campo de vagas no arquivo.
    int altera_vagas(char *pk, Ip *iprimary, int nregistros);
     
    //Desaloca todas as listas ligadas do trajeto.
    void descTrajeto(Ir* iroute, int ntraj);
     
    //Srcmp para comparar as datas e horas.
    int strcmpData(char *data1, char *data2);
    int strcmpHora(char *tempo1, char *tempo2);
     
    //Função auxiliar para fazer um índice organizado pela data, hora e por último pk.
    void inserir_idate_itime(char *pk, Isd *idate, Ip *iprimary, int nregistros, char *data);
     
    /* ==========================================================================
     * ============================ FUNÇÃO PRINCIPAL ============================
     * =============================== NÃO ALTERAR ============================== */
     
    int main()
    {
        /* Verifica se há arquivo de dados */
        int carregarArquivo = 0, nregistros = 0, nregistros_ex = 0, nregistrosAux = 0, ntraj = 0, indice;
        scanf("%d%*c", &carregarArquivo); /* 1 (sim) | 0 (nao) */
        if (carregarArquivo)
            nregistros = carregar_arquivo();
     
     
        /* === ALOCAÇÃO E CRIAÇÃO DOS ÍNDICES === */
        
        /* Índice primário */
        Ip *iprimary = (Ip *)malloc(MAX_REGISTROS * sizeof(Ip));
        if (!iprimary)
        {
            perror(MEMORIA_INSUFICIENTE);
            exit(1);
        }
     
        Is *idriver = (Is *)malloc(MAX_REGISTROS * sizeof(Is));
        if (!idriver)
        {
            perror(MEMORIA_INSUFICIENTE);
            exit(1);
        }
     
     
        Isd *idate = (Isd *)malloc(MAX_REGISTROS * sizeof(Isd));
        if (!idate)
        {
            perror(MEMORIA_INSUFICIENTE);
            exit(1);
        }
     
        Ist *itime = (Ist *)malloc(MAX_REGISTROS * sizeof(Ist));
        if (!itime)
        {
            perror(MEMORIA_INSUFICIENTE);
            exit(1);
        }
     
        Ir *iroute = (Ir *)malloc(MAX_TRAJETOS * sizeof(Ir));
        if (!iroute)
        {
            perror(MEMORIA_INSUFICIENTE);
            exit(1);
        }
     
        //Criando os índices.
        criar_iprimary(iprimary, &nregistros);
        criar_idriver(idriver, &nregistros);
        criar_idate(idate, &nregistros);
        criar_itime(itime, &nregistros);
        criar_iroute(iroute, &nregistros, &ntraj);
        char registro[TAM_REGISTRO];
        
        //Para receber as entradas.
        Carona novo;
        /* <<< COMPLETE AQUI A ALOCAÇÃO E CRIAÇÃO DOS INDICES SECUNDÁRIOS >>> */
     
     
        /* === ROTINA DE EXECUÇÃO DO PROGRAMA === */
        int opcao = 0;
        while (1)
        {
            //printf("escolha:");
            scanf("%d%*c", &opcao);
            switch (opcao)
            {
            case 1:
                /* cadastrar */
                ler_entrada(registro,&novo);
                gerarChave(&novo);
     
     
                //Verificamos se a chave existe, ou se ela foi excluída.
                if((indice = busca_chave(novo.pk, iprimary, nregistros-nregistros_ex)) == -1 
                || iprimary[indice].rrn == -1){
                    
                    //Caso a chave não exista ou foi excluída, gravamos nos índices secundários e no arquivo.
                    gravarArquivo(registro, nregistros-nregistros_ex);
                    
                    inserir_idriver(novo.pk, idriver, nregistros-nregistros_ex, novo.nome);
                    inserir_idate(novo.pk, idate, nregistros-nregistros_ex, novo.data);
                    inserir_itime(novo.pk, itime, nregistros-nregistros_ex, novo.hora);
                    inserir_trajeto_iroute(novo.pk,iroute,&ntraj,novo.trajeto);
     
                    //Caso a chave não exista inserimos normalmente no índice primário.
                    if(indice == -1){
                        inserir_iprimary(novo.pk, iprimary, nregistros-nregistros_ex);
                    //Se a chave já existe, então quer dizer que ela foi excluída.
                    //Logo, coloco o rrn válido para chave removida e
                    //incremento o nregistros_ex que controla o tamanho do meu índice primário
                    //em casos de reinserção de chaves.
                    } else {
                        iprimary[indice].rrn = nregistros;
                        nregistros_ex++;
                    }
                    nregistros++;
                } else {
                    printf(ERRO_PK_REPETIDA,novo.pk);
                }
     
                break;
     
            case 2:
                /* alterar */ 
                printf(INICIO_ALTERACAO);
                scanf("%[^\n]%*c", novo.pk);
     
                if(altera_vagas(novo.pk,iprimary,nregistros-nregistros_ex) == 1){                        
                    printf(SUCESSO);                              
                }else{
                    printf(REGISTRO_N_ENCONTRADO);
                    printf(FALHA);
                }
                break;
     
            case 3:
                /* excluir */
                printf(INICIO_EXCLUSAO);        
                scanf("%[^\n]%*c", novo.pk);
            
                if(remover_registro(novo.pk,iprimary,nregistros-nregistros_ex) == 1){
                    printf(SUCESSO);
                } else {
                    printf(REGISTRO_N_ENCONTRADO);
                    printf(FALHA);
                }
                break;
     
            case 4:
                /* buscar */
                printf(INICIO_BUSCA);
                buscar(iprimary, idriver, iroute, idate, itime, nregistros-nregistros_ex, ntraj);
                break;
     
            case 5:
                /* listar */
                printf(INICIO_LISTAGEM);
                listar(iprimary, idriver, iroute, idate, itime, nregistros-nregistros_ex, ntraj);
                break;
     
            case 6:
                /*    libera espaço */
                liberar_espaco(iprimary, idriver, iroute, idate, itime, &nregistros, &nregistros_ex, &ntraj);
                /* <<< COLOQUE AQUI A CHAMADA PARA A FUNCAO LIBERAR ESPACO >>> */
     
                break;
     
            case 7:
                /* imprime arquivo de dados */
                printf(INICIO_ARQUIVO);
     
                if (nregistros == 0)
                    printf(ARQUIVO_VAZIO);
                else
                    printf("%s\n", ARQUIVO);
                break;
     
            case 8:
                /*    imprime os índices secundários */
                imprimirSecundario(idriver, iroute, idate, itime, nregistros-nregistros_ex, ntraj);
                break;
     
            case 9:
                /* <<< LIBERE A MEMÓRIA DAS ESTRUTURAS CRIADAS >>> */
                finalizar(iprimary,idriver,iroute,idate,itime,ntraj);
                return (0);
     
                break;
     
            default:
                printf(OPCAO_INVALIDA);
                break;
            }
        }
        return 0;
    }
     
    /* ==========================================================================
     * =========================== CÓDIGO DAS FUNÇÕES ===========================
     * ========================================================================== */
     
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
     
    /* carrega dados do arquivo e retorna
     * a quantidade de registros lidos */
    int carregar_arquivo()
    {
        scanf("%[^\n]%*c", ARQUIVO);
        return strlen(ARQUIVO) / TAM_REGISTRO;
    }
     
     
    /* Recupera do arquivo o registro com o rrn
     * informado e retorna os dados na struct Carona */
    Carona recuperar_registro(int rrn)
    {
        char temp[257], *p;
        strncpy(temp, ARQUIVO + ((rrn)*TAM_REGISTRO), TAM_REGISTRO);
        temp[256] = '\0';
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
     
    /* ==========================================================================
     * ============================= CRIAR ÍNDICES ==============================
     * ========================================================================== */
     
    /* Imprimir indices secundarios */
    void imprimirSecundario(Is *idriver, Ir *iroute, Isd *idate, Ist *itime, int nregistros, int ntraj)
    {
        int opPrint = 0;
        ll *aux;
        printf(INICIO_ARQUIVO_SECUNDARIO);
        scanf("%d", &opPrint);
        if (!nregistros)
            printf(ARQUIVO_VAZIO);
        switch (opPrint)
        {
        case 1:
            for (int i = 0; i < nregistros; i++)
            {
                printf("%s %s\n", idriver[i].pk, idriver[i].nome);
            }
            break;
        case 2:
            for (int i = 0; i < nregistros; i++)
            {
                printf("%s %s\n", idate[i].pk, idate[i].data);
            }
            break;
        case 3:
            for (int i = 0; i < nregistros; i++)
            {
                printf("%s %s\n", itime[i].pk, itime[i].hora);
            }
            break;
        case 4:
            for (int i = 0; i < ntraj; i++)
            {
                printf("%s", iroute[i].trajeto);
                aux = iroute[i].lista;
                while (aux != NULL)
                {
                    printf(" %s", aux->pk);
                    aux = aux->prox;
                }
                printf("\n");
            }
            break;
        default:
            printf(OPCAO_INVALIDA);
            break;
        }
    }
     
    /* <<< IMPLEMENTE AQUI AS DEMAIS FUNÇÕES >>> */
     
    /* Gera a chave para determinada struct de carona */ 
    void gerarChave(Carona *novo){
        
        //Pego os dados byte a byte que formaram a chave e mudo para maiúscula.
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
     
     
     
    //Lemos as entradas.
    void ler_entrada(char* registro, Carona *novo){
        
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
        
        for(int i = strlen(registro); i < TAM_REGISTRO; i++){
            strcat(registro,"#");
        }
    }
     
    //Inserindo no indice primário.
    void inserir_iprimary(char *pk, Ip *iprimary, int nregistros){
     
        Ip trocar;
        int i = nregistros;
     
        //Colocamos os valores em uma struct que será utilizada para trocar.
        strcpy(iprimary[i].pk, pk);
        iprimary[i].rrn = i;
     
        //Inserção ordenada no vetor. (meu próprio código :D )
        while(i){
            if(strcmp(iprimary[i-1].pk,iprimary[i].pk) > 0){
                trocar = iprimary[i];
                iprimary[i] = iprimary[i-1]; 
                iprimary[i-1] = trocar;
            } else {
                break;
            }        
            i--;
        }
    }
     
    //Inserindo no índice secundário, data da carona.
    void inserir_idriver(char *pk, Is *idriver, int nregistros, char *nome){
        
        Is trocar;
        int i = nregistros;
        strcpy(idriver[i].pk, pk);
        strcpy(idriver[i].nome, nome);
        
     
        while(i){
            if(strcmp(idriver[i-1].nome,idriver[i].nome) > 0){
                        trocar = idriver[i];
                        idriver[i] = idriver[i - 1];
                        idriver[i - 1] = trocar;
            }
            else if(strcmp(idriver[i-1].nome,idriver[i].nome) == 0){
                if(strcmp(idriver[i-1].pk,idriver[i].pk) > 0){
                        trocar = idriver[i];
                        idriver[i] = idriver[i - 1];
                        idriver[i - 1] = trocar;
                }
            } else break;
            i--;
        }
     
    }
     
    void inserir_idate(char *pk, Isd *idate, int nregistros, char *data){
        
        
        Isd trocar;
        int i = nregistros;
        strcpy(idate[i].pk, pk);
        strcpy(idate[i].data, data);
        
     
        while(i){
            if(strcmpData(idate[i-1].data,idate[i].data) > 0){
                        trocar = idate[i];
                        idate[i] = idate[i - 1];
                        idate[i - 1] = trocar;
            }
            else if(strcmpData(idate[i-1].data,idate[i].data) == 0){
                if(strcmp(idate[i-1].pk,idate[i].pk) > 0){
                        trocar = idate[i];
                        idate[i] = idate[i - 1];
                        idate[i - 1] = trocar;
                }
            } else break;
            i--;
        }
    }
     
    //Inserindo no índice secundário, hora da carona.
    void inserir_itime(char *pk, Ist *itime, int nregistros, char *hora){
        Ist trocar;
        
        int i = nregistros;
        strcpy(itime[i].pk, pk);
        strcpy(itime[i].hora, hora);
        
     
        while(i){
            if(strcmp(itime[i-1].hora,itime[i].hora) > 0){
                        trocar = itime[i];
                        itime[i] = itime[i - 1];
                        itime[i - 1] = trocar;
            }
            else if(strcmp(itime[i-1].hora,itime[i].hora) == 0){
                if(strcmp(itime[i-1].pk,itime[i].pk) > 0){
                        trocar = itime[i];
                        itime[i] = itime[i - 1];
                        itime[i - 1] = trocar;
                }
            } else break;
            i--;
        }
    }
     
    //Separamos os trajetos e depois chamamos o inserir que coloca no índice secundário.
    int inserir_trajeto_iroute(char *pk, Ir *iroute, int *ntraj, char *trajeto){
        //Cópia do trajeto para não alterar o original.
        char copiaTrajeto[TAM_TRAJETO];
        //Passamos o dado para cópia.
        strcpy(copiaTrajeto,trajeto);
     
        //Desmenbramos trajeto a trajeto e mandamos inserir no vetor de índices
        //secundário e ele saberá se irá precisar colocar o trajeto em uma nova posição no 
        //vetor.
        char *p = strtok(copiaTrajeto,"|");
        inserir_iroute(pk,iroute, ntraj,p);
        
        while((p = strtok(NULL,"|")) != NULL){
            if(inserir_iroute(pk,iroute,ntraj,p) == -1) return -1;
        }
     
        return 1;
    }
     
    //Inserindo no índice secundário, trajeto da carona.
    int inserir_iroute(char *pk, Ir *iroute, int *ntraj, char *trajeto){
     
        Ir trocar;
        int indice = busca_trajeto(iroute,*ntraj,trajeto);
     
        //Se retornar uma posição então já temos a categoria registrada.
        if(indice != -1){
            //Mandamos inserir na lista ligada da categoria.
            if(inserir_iroute_lista(pk,&iroute[indice]) == -1) return -1;
        } else {
            //Se não tivermos, verificamos se o índice que será criado já não se encontra cheio.
            if(*ntraj == MAX_TRAJETOS) return -1;
     
            //Colocamos no final do índice secundário.
            strcpy(iroute[*ntraj].trajeto,    trajeto);
            if(inserir_iroute_lista(pk,&iroute[*ntraj]) == -1) return -1;
            int i = *ntraj;
     
            //Organizamos de trás para frente.
            while(i){
                if(strcmp(iroute[i-1].trajeto,iroute[i].trajeto) > 0){
                    trocar = iroute[i];
                    iroute[i] = iroute[i - 1];
                    iroute[i - 1] = trocar;
                } else break;
                i--;
            }
            //Incrementamos a nossa váriavel que controla a quantidade de trajetos.
            (*ntraj)+=1; 
        }  
        return 1;
    }
     
    //Inserindo no índice secundário, trajeto da carona.
    int inserir_iroute_lista(char *pk, Ir *head){
     
        ll *novo = (ll*)malloc(sizeof(ll));
     
        if(novo){//Verifica se alocou memória.
            strcpy(novo->pk,pk);//recebe a chave
            novo->prox = NULL;
     
            ll *ant = head->lista;
     
            //Se for o primeiro da lista ligada da categoria.
            if(head->lista == NULL) {
                head->lista = novo;    
            //Se for menor que o primeiro.
            } else if(strcmp(novo->pk, ant->pk) < 0){
                novo->prox = ant;
                head->lista = novo; 
            //Nos demais casos achamos a posição correta para inserir.
            } else {
                
                //Aponta para o próximo.
                ll *prox = ant->prox;
     
                while(prox){
                    //Se for maior que o anterior e menor que o próximo, então 
                    //achamos a posição correta.
                    if(strcmp(novo->pk,ant->pk) > 0 && strcmp(novo->pk,prox->pk) < 0){
                        ant->prox = novo;
                        novo->prox = prox;
                        break;
                    }
                    ant = prox;
                    prox = prox->prox;
     
                }
     
                //Se chegar no caso em que prox = NULL então é o último nó a ser inserido.
                if(!prox){
                    ant->prox = novo;
                    novo->prox = NULL;
                }
            }
            return 1;
        }
        return -1;
    }
     
    //Referência: http://www.larback.com.br/aula/254/Busca-Bin%C3%A1ria-em-C++
    int busca_trajeto(Ir *iroute, int ntraj, char *trajeto){
        int i = 0;
        int f = ntraj -1;
        int meio;
     
        while (i<=f) {
            meio = (i+f)/2;
            if (strcmp(iroute[meio].trajeto, trajeto) == 0){
                return meio;
            }else {
                if(strcmp(trajeto,iroute[meio].trajeto) > 0) {
                //if (chave>arranjo[meio])
                    i=meio+1;
                } else { 
                    f=meio-1;
                }
            }
        }
        return -1;
    }
     
    //Referência: http://www.larback.com.br/aula/254/Busca-Bin%C3%A1ria-em-C++
    int busca_chave(char *pk, Ip *iprimary, int nregistros){
        int i = 0;
        int f = nregistros -1;
        int meio;
     
        while (i<=f) {
            meio = (i+f)/2;
            if (strcmp(iprimary[meio].pk, pk) == 0){
                return meio;
            }else {
                if(strcmp(pk,iprimary[meio].pk) > 0) {
                //if (chave>arranjo[meio])
                    i=meio+1;
                } else { 
                    f=meio-1;
                }
            }
        }
        return -1;
    }
     
    //O iprimary define o tamanho dos nregistros para não ter redundância em todos os indices.
    void criar_iprimary(Ip *iprimary, int *nregistros){
                
        Carona car;
                
        //A cada registro recuperado mandamos inserir no indice_p.
        for(int i = 0; i < *nregistros; i++){
            car = recuperar_registro(i);
            inserir_iprimary(car.pk,iprimary,i);
        }
                
    }
     
    void criar_idriver(Is *idriver, int *nregistros){
                
        Carona car;
        //Mesma lógica do indice primário.
        for(int i = 0; i < *nregistros; i++){
            car = recuperar_registro(i);
            inserir_idriver(car.pk,idriver,i,car.nome);
        }        
    }
     
    void criar_idate(Isd *idate, int *nregistros){
                
        Carona car;
        //Mesma lógica do indice primário.
        for(int i = 0; i < *nregistros; i++){
            car = recuperar_registro(i);
            inserir_idate(car.pk,idate,i,car.data);
        }            
    }
     
    void criar_itime(Ist *itime, int *nregistros){
                
        Carona car;
        //Mesma lógica do indice primário.
        for(int i = 0; i < *nregistros; i++){
            car = recuperar_registro(i);
            inserir_itime(car.pk,itime,i,car.hora);
        }
    }
     
    void criar_iroute(Ir *iroute, int *nregistros, int *ntraj){
     
        descTrajeto(iroute, *ntraj);
                
        Carona car;
        //Mesma lógica do indice primário.
        for(int i = 0; i < *nregistros; i++){
            car = recuperar_registro(i);
            inserir_trajeto_iroute(car.pk,iroute,ntraj,car.trajeto);                    
        }
    }
     
    void gravarArquivo(char *registro, int nregistros){
        complementar_registro(registro);    
        strcat(ARQUIVO,registro);
    }
     
    int remover_registro(char *pk, Ip *iprimary, int nregistros){
            
            //Recuperamos a chave.
            int indice;
            indice = busca_chave(pk,iprimary,nregistros);
     
            //Verificamos se existe ou se seu rrn já está excluído.
            if(indice == -1 || iprimary[indice].rrn == -1){
                return -1;
             }
            
            
            //Alteramos o arquivo para evidenciar que o registro foi removido.
            ARQUIVO[iprimary[indice].rrn*TAM_REGISTRO] = '*';
            ARQUIVO[iprimary[indice].rrn*TAM_REGISTRO+1] = '|';
     
            //E marcamos como removido no índice primário.
            iprimary[indice].rrn = -1;        
        
            return 1;    
    }
     
    int altera_vagas(char *pk, Ip *iprimary, int nregistros){
     
     
        Ip *aux;
        int indice, flag = 0;
     
        indice = busca_chave(pk,iprimary,nregistros);
     
        //Se o índice for -1 ou seu rrn -1 então o registro não existe.
        if(indice == -1 || iprimary[indice].rrn == -1){
            return -1;
        }
        
        //Criamos uma string e um int vaga que irão receber o valor alterado.
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
        int rrn = iprimary[indice].rrn * TAM_REGISTRO;
        
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
     
    void buscar(Ip* iprimary, Is *idriver, Ir *iroute, Isd *idate, Ist *itime, int nregistros, int ntraj){
     
        ll *navegador; //Para navegar na lista invertida.
        //Novo para receber as entradas e o recuperado para receber os dados do arquivo.
        Carona novo, recuperado;
        //Opcao para o switch, recebeIndice para achar a posição no índice primário,
        //flag para saber se ocorreu algum erro e flagQL para controlar os \n.
        int opcao,recebeIndice, flag = 0, flagQL = 0;
        
        scanf("%d%*c",&opcao);
            
        switch(opcao) {
            case 1:
            
                scanf("%[^\n]%*c",novo.pk); //Leio a chave.
                recebeIndice = busca_chave(novo.pk,iprimary,nregistros); //Faço a busca
                //Verificamos se o registro solicitado existe e exibimos.
                if(recebeIndice != -1 && iprimary[recebeIndice].rrn != -1){
                    exibir_registro(iprimary[recebeIndice].rrn);
                    flag++;
                }
            break;
            
            case 2:
            
                scanf("%[^\n]%*c",novo.data); //Leio a data..    
                for(int i = 0; i < nregistros; i++){
                    
                //Verificamos se existem registros com a data em questão solicitada, se existir 
                //exibimos.
                    if(strcmp(novo.data,idate[i].data) == 0){
                        recebeIndice = busca_chave(idate[i].pk,iprimary,nregistros);
                        if(recebeIndice != -1 && iprimary[recebeIndice].rrn != -1){
                            flag++;//Flag para o erro.
                            flagQL++;//Flag para \n.
                            if(flagQL == 2){
                                flagQL = 1;
                                printf("\n");
                            }
                            exibir_registro(iprimary[recebeIndice].rrn);
                        }
                    }
                }
                
            break;
            
            case 3:
                scanf("%[^\n]%*c",novo.trajeto); //Leio o trajeto.
                recebeIndice = busca_trajeto(iroute,ntraj,novo.trajeto);
                
                //Verificamos se o trajeto existe.
                if(recebeIndice != -1){
                    navegador = iroute[recebeIndice].lista;
     
                    //Se existir, navegamos nas chaves que estão no trajeto.
                    //observe que a flag que controla se tem erro só irá desvalidar o erro
                    //caso seja inserida, ou seja, se realmente tenha algum registro no 
                    //trajeto solicitado.
                    while(navegador){
                        
                        recebeIndice = busca_chave(navegador->pk,iprimary,nregistros);
     
                        if(recebeIndice != -1 && iprimary[recebeIndice].rrn != -1){
                            flag++;//Flag para o erro.
                            flagQL++;//Flag para \n.
                            if(flagQL == 2){
                                flagQL = 1;
                                printf("\n");
                            }
                            exibir_registro(iprimary[recebeIndice].rrn);
                        }
                        navegador = navegador->prox;
                    }
                }
     
            break;
     
            case 4:
     
                scanf("%[^\n]%*c",novo.trajeto); //Leio o trajeto.    
                scanf("%[^\n]%*c",novo.data); //Leio a data.
                
     
                recebeIndice = busca_trajeto(iroute,ntraj,novo.trajeto);
                
                //Mesma lógica do caso 3, porém, precisamos verificar se a data também
                //está de acordo com o registro daquele trajeto.
                if(recebeIndice != -1){
                    navegador = iroute[recebeIndice].lista;
     
                    while(navegador){
                        
                        recebeIndice = busca_chave(navegador->pk,iprimary,nregistros);
     
                        if(recebeIndice != -1 && iprimary[recebeIndice].rrn != -1){
                            recuperado = recuperar_registro(iprimary[recebeIndice].rrn);
                            
                            if(strcmp(novo.data,recuperado.data) == 0){
                                flag++;//Flag para o erro.
                                flagQL++;//Flag para \n.
                                if(flagQL == 2){
                                    flagQL = 1;
                                    printf("\n");
                                }
                                exibir_registro(iprimary[recebeIndice].rrn);
                            }
                        }
                    
                        navegador = navegador->prox;
                    }
                }
                //Recebemos as entradas marca e categoria.
            break;
        }
        //Se a flag for == 0, então não teve nenhuma exibição de registro, ou seja, erro.
        if(flag == 0) printf(REGISTRO_N_ENCONTRADO);
    }
     
    void listar(Ip* iprimary, Is *idriver, Ir *iroute, Isd *idate, Ist *itime, int nregistros, int ntraj){
     
        //índice secundário auxiliar para organizar de acordo com data, hora e pk.
        Isd idateAux[MAX_REGISTROS];
        //Novo para receber as entradas e o recuperado para receber os dados do arquivo.
        Carona novo,recuperado;
        ll *navegador; //Para navegar na lista invertida.
        //Opcao para o switch, recebeIndice para achar a posição no índice primário,
        //flag para saber se ocorreu algum erro, flagQL para controlar os \n e 
        //nT para controlar a quantidade de registros.
        int opcao,recebeIndice, flag = 0, flagQL = 0,nT = 0;
        
        scanf("%d%*c",&opcao);
        //O listar possui basicamente a mesma lógica do buscar com a diferença
        //de que não temos de verificar se determinado registro está de acordo com
        //o dado solicitado pelo usuário.
        switch(opcao) {
            case 1:
                for(int i = 0; i < nregistros; i ++){
                    if(iprimary[i].rrn != -1){
                        flag++;//Flag para o erro.
                        flagQL++;//Flag para \n.
                        if(flagQL == 2){
                            flagQL = 1;
                            printf("\n");
                        }
                        exibir_registro(iprimary[i].rrn);
                    }
                }
            break;
     
            case 2:
                for(int i = 0; i < ntraj; i++){
                    navegador = iroute[i].lista;
                    while(navegador != NULL){
                        recebeIndice = busca_chave(navegador->pk,iprimary,nregistros);
                        if(recebeIndice != -1 && iprimary[recebeIndice].rrn != -1){
                            
                            flag++;//Flag para o erro.
                            flagQL++;//Flag para \n.
                            if(flagQL == 2){
                                flagQL = 1;
                                printf("\n");
                            }
                            exibir_registro(iprimary[recebeIndice].rrn);
                        }
                        navegador = navegador->prox;
                    }
                }
            break;
            
            case 3:
                for(int i = 0; i < nregistros; i++){
                    recebeIndice = busca_chave(idriver[i].pk,iprimary,nregistros);
                    if(recebeIndice != -1 && iprimary[recebeIndice].rrn != -1){
                        flag++;//Flag para o erro.
                        flagQL++;//Flag para \n.
                        if(flagQL == 2){
                            flagQL = 1;
                            printf("\n");
                        }
                        exibir_registro(iprimary[recebeIndice].rrn);
                    }
                }
            break;
     
            case 4:
                //Como o caso 4 possui um listar em caso de empates organizados
                //por data, hora e pk precisamos fazer ajustes.
     
                //Primeiramente, vamos buscando todas as chaves que estão no índice
                //secundário e verificando se existem ou não.
                //Quando existe de fato, mandamos ele ser inserido no índice secundário
                //auxiliar ordenado pelos critérios citados acima.
                //Ou seja, organizamos do modo que foi pedido e não alteramos o índice
                //secundário original.
                for(int i = 0; i < nregistros; i++){
                    recebeIndice = busca_chave(idate[i].pk,iprimary,nregistros);
                    if(recebeIndice != -1 && iprimary[recebeIndice].rrn != -1){
                        inserir_idate_itime(idate[i].pk,idateAux,iprimary,nT,idate[i].data);
                        nT++;
                    }
                }
                
                //Depois, mandamos listar.
                for(int i = 0; i < nT; i++){
                    recebeIndice = busca_chave(idateAux[i].pk,iprimary,nregistros);
                
                    flag++;//Flag para o erro.
                    flagQL++;//Flag para \n.
                    if(flagQL == 2){
                        flagQL = 1;
                        printf("\n");
                    }
                    exibir_registro(iprimary[recebeIndice].rrn);
                }
     
            break;
            
            case 5:
                //De modo semelhante ao caso 4, o caso 5 possui os mesmos critérios,
                //porém, não estamos listando a data, e sim o trajeto.
     
                //Nesse caso, temos de listar trajeto a trajeto e em caso de empates
                //organizar pelos critérios data, hora e pk.
                //Logo, varremos todo o índice secundário de trajeto.            
                for(int i = 0; i < ntraj; i++){
                    navegador = iroute[i].lista;
                    //Para cada trajeto, fazemos uma inserção no idateAux,
                    //mas antes, verificando se o trajeto existe.
                    while(navegador != NULL){
                        recebeIndice = busca_chave(navegador->pk,iprimary,nregistros);
                        if(recebeIndice != -1 && iprimary[recebeIndice].rrn != -1){
                            recuperado = recuperar_registro(iprimary[recebeIndice].rrn);
                            inserir_idate_itime(navegador->pk,idateAux,iprimary,nT,recuperado.data);
                            nT++;
                        }
                        navegador = navegador->prox;
                    }
                    
                    //Após todas as chaves do determinado trajeto estarem inseridas no
                    //idateAux mandamos listar.
                    for(int j = 0; j < nT; j++){
                        recebeIndice = busca_chave(idateAux[j].pk,iprimary,nregistros);
     
                        flag++;//Flag para o erro.
                        flagQL++;//Flag para \n.
                        if(flagQL == 2){
                            flagQL = 1;
                            printf("\n");
                        }
                        exibir_registro(iprimary[recebeIndice].rrn);
                    }
                    //Depois de listado, o nT é zerado para fazer o mesmo processo com o próximo trajeto.
                    nT = 0;
                }
            break;
        }
        if(flag == 0) printf(REGISTRO_N_ENCONTRADO);
    }
     
    void liberar_espaco(Ip* iprimary, Is *idriver, Ir *iroute, Isd *idate, Ist *itime, int *nregistros, int *nregistros_ex, int *ntraj){
        
        int i = 0,j = 0;
        
        //Varremos o arquivo em busca das marcações de removidos no começo
        // de cada registro, ou seja, |*.
        while(ARQUIVO[i] != '\0'){
            //Caso encontramos a marcação de removido, pulamos o registro.
            if(i%TAM_REGISTRO == 0 && ARQUIVO[i] == '*' && ARQUIVO[i+1] == '|'){
                i = i + TAM_REGISTRO;
            }
            //E caso o registro não esteja marcado como removido, simplesmente,
            //gravamos.
            //Observe que o que estamos fazendo é gravar um registro existente
            //em cima de um marcado.
            else {
                ARQUIVO[j] = ARQUIVO[i];
                j++;
                i++;
            }
        }
        
        //Por fim, colocamos o \0 no final do arquivo.
        ARQUIVO[j] = '\0';
        //Ajustamos a quantidade de registros, ou seja, esquecemos aqueles que estão
        //marcados como removidos.
        *nregistros = j/TAM_REGISTRO;
        //Zeramos a quantidade de registros reinseridos.
        *nregistros_ex = 0;
        
        //E mandamos recriar nossos índices.
        criar_iprimary(iprimary, nregistros);
        criar_idriver(idriver, nregistros);
        criar_idate(idate, nregistros);
        criar_itime(itime, nregistros);
        criar_iroute(iroute, nregistros, ntraj);
        
    }
     
    //Desconcatenamos todos os nós da lista invertida.
    //Uso tanto para desalocar ao finalizar quanto para iniciar
    //a lista invertida com NULL.
    void descTrajeto(Ir* iroute, int ntraj){
        
        ll *navegador;
        ll *delete;
     
        for(int i = 0; i < MAX_TRAJETOS; i++){
            navegador = iroute[i].lista;
            while(navegador){
                delete = navegador;
                navegador = navegador->prox;
                free(delete);    
            }
            iroute[i].lista = NULL;
        }
    }
     
    //Desalocamos a memória.
    void finalizar(Ip* iprimary, Is *idriver, Ir *iroute, Isd *idate, Ist *itime, int ntraj){
        free(iprimary);
        free(idriver);
        free(idate);
        free(itime);
     
        descTrajeto(iroute,ntraj);
        free(iroute);
        
    }
     
    //Strcmp para as datas.
    int strcmpData(char *data1, char *data2){
         
        int diaD1, mesD1, anoD1;
        int diaD2, mesD2, anoD2;
        
        sscanf(data1,"%d/%d/%d",&diaD1,&mesD1,&anoD1);
        sscanf(data2,"%d/%d/%d",&diaD2,&mesD2,&anoD2);
        
        if(strcmp(data1,data2) == 0){
            return 0;
        }
        //Caso contrário, fazemos uma série de verificações para ver se data1 > data2.
        //Se for, retornamos 1, que no caso, é > 0.
        if((anoD1 > anoD2) || (anoD1 == anoD2 && mesD1 > mesD2) ||
         (anoD1 == anoD2 && mesD1 == mesD2 && diaD1 > diaD2)){
            return 1;
        }
        //E se não retornar, então data1 < data2.
        return -1;
    }
     
    //Strcmp para hora.
    int strcmpHora(char *tempo1, char *tempo2){
        int horaT1, minT1;
        int horaT2, minT2;
     
        sscanf(tempo1,"%d:%d",&horaT1,&minT1);
        sscanf(tempo2,"%d:%d",&horaT2,&minT2);
        
        if(strcmp(tempo1,tempo2) == 0){
            return 0;
        }
        
        if((horaT1 > horaT2) || (horaT1 == horaT2 && minT1 > minT2)){
            return 1;
        }
        //E se não retornar, então data1 < data2.
        return -1;
    }
     
     
    //Função utilizada para inserir de acordo com os casos 4 e 5 do listar.
    void inserir_idate_itime(char *pk, Isd *idate, Ip *iprimary, int nregistros, char *data){
        
        Carona recuperado_1,recuperado;
        Isd trocar;
        int i = nregistros;
        int recebeIndice,recebeIndice_1;
        
        idate[i].pk[0] = '\0';
        idate[i].data[0] = '\0'; 
        
        strcpy(idate[i].pk, pk);
        strcpy(idate[i].data, data);
        
        while(i){
            if(strcmpData(idate[i-1].data,idate[i].data) > 0){
                        trocar = idate[i];
                        idate[i] = idate[i - 1];
                        idate[i - 1] = trocar;
            }
            else if(strcmpData(idate[i-1].data,idate[i].data) == 0){
                recebeIndice_1 = busca_chave(idate[i-1].pk,iprimary,nregistros);
                recebeIndice = busca_chave(idate[i].pk,iprimary,nregistros);
                
                recuperado_1 = recuperar_registro(iprimary[recebeIndice_1].rrn);
                recuperado = recuperar_registro(iprimary[recebeIndice].rrn);
                
                if(strcmpHora(recuperado_1.hora,recuperado.hora) > 0){
                        trocar = idate[i];
                        idate[i] = idate[i - 1];
                        idate[i - 1] = trocar;
                }else if(strcmpHora(recuperado_1.hora,recuperado.hora) == 0){
                    if(strcmp(idate[i-1].pk,idate[i].pk) > 0){
                        trocar = idate[i];
                        idate[i] = idate[i - 1];
                        idate[i - 1] = trocar;
                    }
                }
            } else break;
            i--;
        }
    }