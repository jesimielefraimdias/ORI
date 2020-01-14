/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Organizacao e Recuperacao da Informacao
 * Prof. Tiago A. Almeida
 *
 * Trabalho 03B - Hashing com encadeamento
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
#define REGISTRO_INSERIDO "Registro %s inserido com sucesso.\n\n"

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
 * Contém a chave primária, o RRN do registro atual e o ponteiro para o próximo
 * registro. */
typedef struct chave
{
	char pk[TAM_PRIMARY_KEY];
	int rrn;
	struct chave *prox;
} Chave;

/* Estrutura da Tabela Hash */
typedef struct
{
	int tam;
	Chave **v;
} Hashtable;

/* Variáveis globais */
char ARQUIVO[TAM_ARQUIVO];
int nregistros;

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

/*Função de Hash*/
short hash(const char *chave, int tam);

/*Auxiliar para a função de hash*/
short f(char x);

/*Funcoes do menu*/
void ler_entrada(char *registro, Carona *novo);
void cadastrar(Hashtable *tabela);
int alterar(Hashtable *tabela);
void buscar(Hashtable *tabela);
int remover(Hashtable *tabela);
void imprimir_tabela(Hashtable *tabela);
void liberar_tabela(Hashtable *tabela);

/*Funcoes auxiliares*/
void criar_tabela(Hashtable *tabela, int tam);
void carregar_tabela(Hashtable *tabela);

/* <<< DECLARE AQUI OS PROTOTIPOS >>> */
void complementar_registro(char* registro);
//Retorna 1 quando inseriu em um determinado endereço ou 0 quando não inseriu.
int inserir_tabela(Hashtable *tabela, char *chave, int rrn);
int buscar_tabela(Hashtable *tabela, char *chave, int* rrn);
void gravarArquivo(char *registro, int nregistros);

/* ==========================================================================
 * ============================ FUNÇÃO PRINCIPAL ============================
 * =============================== NÃO ALTERAR ============================== */
int main()
{

	/* Arquivo */
	int carregarArquivo = nregistros = 0;
	scanf("%d%*c", &carregarArquivo); // 1 (sim) | 0 (nao)
	if (carregarArquivo)
		carregar_arquivo();

	/* Tabela Hash */
	int tam;
	scanf("%d%*c", &tam);
	tam = prox_primo(tam);

	Hashtable tabela;
	criar_tabela(&tabela, tam);
	if (carregarArquivo)
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
				printf("%s", (alterar(&tabela)) ? SUCESSO : FALHA);
				break;
			case 3:
				printf(INICIO_BUSCA);
				buscar(&tabela);
				break;
			case 4:
				printf(INICIO_EXCLUSAO);
				printf("%s", (remover(&tabela)) ? SUCESSO : FALHA);
				break;
			case 5:
				printf(INICIO_LISTAGEM);
				imprimir_tabela(&tabela);
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

short f(char x)
{
	return (x < 59) ? x - 48 : x - 54;
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

/* Libera a Tabela Hash */
void liberar_tabela(Hashtable *tabela)
{
	Chave *aux1, *aux2, *aux3;
	short i;
	for (i = 0, aux1 = tabela->v[i]; i < tabela->tam; aux1 = tabela->v[++i])
		for (aux2 = aux1; aux2; aux3 = aux2, aux2 = aux2->prox, free(aux3), aux3 = NULL)
			;
	free(tabela->v);
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

void complementar_registro(char* registro){
    int i;
    for(i = strlen(registro); i < TAM_REGISTRO; i++){
        strcat(registro,"#");
    }
    registro[TAM_REGISTRO] = '\0';
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

void criar_tabela(Hashtable *tabela, int tam){
    tabela->tam = tam;
    tabela->v = (Chave**) malloc(sizeof(Chave*) * tam);
    
    if(!nregistros) ARQUIVO[0] = '\0';
    for(int i = 0; i < tabela->tam; i++) tabela->v[i] = NULL;
}

int alterar(Hashtable *tabela){
   char chave[TAM_PRIMARY_KEY];
   int flag = 0;

   scanf("%[^\n]%*c", chave);
   
   int rrn;
   int endereco = buscar_tabela(tabela, chave, &rrn);

   if(rrn == -1){
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
   rrn = rrn * TAM_REGISTRO;
   
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

int buscar_tabela(Hashtable *tabela, char *chave, int *rrn){
    int endereco = hash(chave, tabela->tam);
    *rrn = -1;
    Chave * encontrou = tabela->v[endereco];

    while(encontrou != NULL && strcmp(encontrou->pk, chave)){
        encontrou = encontrou->prox;
    }
    if(encontrou != NULL){
		*rrn = encontrou->rrn;
	}
    return endereco;
}

void buscar(Hashtable *tabela){
    char chave[TAM_PRIMARY_KEY];
    int rrn;

    scanf("%[^\n]%*c", chave);
 
    int endereco = buscar_tabela(tabela, chave, &rrn);
 
    if(rrn == -1){
        printf(REGISTRO_N_ENCONTRADO);
        return;
    }
 
    exibir_registro(rrn);
}

void imprimir_tabela(Hashtable *tabela){
     Chave *encontrou;
     for(int i = 0; i < tabela->tam; i++){
         
         printf("[%d]",i);
         encontrou = tabela->v[i];
         
         while(encontrou != NULL){
             printf(" %s",encontrou->pk);
			 encontrou = encontrou->prox;
         }
		 printf("\n");
     }
 }

void cadastrar(Hashtable *tabela){
    Carona novo;
    char registro[TAM_REGISTRO+1];
    int rrn;
	
    ler_entrada(registro,&novo);
 
    gerarChave(&novo);
	buscar_tabela(tabela,novo.pk, &rrn);
    
	if(rrn != -1){
        printf(ERRO_PK_REPETIDA,novo.pk);
        return;
    }
 
    if(inserir_tabela(tabela,novo.pk, nregistros) == 1){
        printf(REGISTRO_INSERIDO, novo.pk);
    } else {
        printf(MEMORIA_INSUFICIENTE);
        return;
    }
 
    gravarArquivo(registro,nregistros);
    nregistros+=1;
}

 int inserir_tabela(Hashtable *tabela, char *chave, int rrn){
    int endereco = hash(chave,tabela->tam);
    
	Chave *novo = (Chave*)malloc(sizeof(Chave));

	if(novo){//Verifica se alocou memória.
		strcpy(novo->pk,chave);//recebe a chave
        novo->rrn = rrn;
		novo->prox = NULL;

		Chave *ant = tabela->v[endereco];

		//Se for o primeiro da lista ligada da categoria.
		if(tabela->v[endereco] == NULL) {
			tabela->v[endereco] = novo;
		//Se for menor que o primeiro.
		} else if(strcmp(novo->pk, ant->pk) < 0){
			novo->prox = ant;
			tabela->v[endereco] = novo; 
		//Nos demais casos achamos a posição correta para inserir.
		} else {
			
			//Aponta para o próximo.
			Chave *prox = ant->prox;

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
	return 0;
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

int remover(Hashtable *tabela){
    char chave[TAM_PRIMARY_KEY];
    int rrn = -1;
    scanf("%[^\n]%*c", chave);
     

    int endereco = hash(chave, tabela->tam);

    Chave *anterior = tabela->v[endereco];
    Chave *delete;

    if(anterior == NULL){
		printf(REGISTRO_N_ENCONTRADO);
        return 0;
    } else if(anterior != NULL && strcmp(anterior->pk, chave) == 0){
        delete = anterior;
        tabela->v[endereco] = anterior->prox;
        rrn = delete->rrn;
        free(delete);
        
    } else {

        while(anterior->prox != NULL && strcmp(anterior->prox->pk, chave) != 0){
            anterior = anterior->prox;
        }

        if(anterior->prox != NULL && strcmp(anterior->prox->pk, chave) == 0){
            delete = anterior->prox;
            anterior->prox = anterior->prox->prox;    
            rrn = delete->rrn;
            free(delete);
        }
    }

	if(rrn == -1){
		printf(REGISTRO_N_ENCONTRADO);
		return 0;
	}
	    
 	//Alteramos o arquivo para evidenciar que o registro foi removido.
    ARQUIVO[rrn*TAM_REGISTRO] = '*';
	ARQUIVO[rrn*TAM_REGISTRO+1] = '|';
	return 1;    
}