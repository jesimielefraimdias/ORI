/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Organização e Recuperação da Informação
 * Prof. Tiago A. de Almeida
 *
 * Trabalho 02 - Árvore B
 *
 * RA: 		726544
 * Aluno: 	Jesimiel Efraim Dias
 * ========================================================================== */

/* Bibliotecas */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

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
#define TAM_STRING_INDICE (30 + 6 + 4 + 1)

#define TAM_REGISTRO 256
#define MAX_REGISTROS 1000
#define MAX_ORDEM 150
#define TAM_ARQUIVO (MAX_REGISTROS * TAM_REGISTRO + 1)

/* Saídas do usuário */
#define OPCAO_INVALIDA "Opcao invalida!\n"
#define MEMORIA_INSUFICIENTE "Memoria insuficiente!\n"
#define REGISTRO_N_ENCONTRADO "Registro(s) nao encontrado!\n"
#define CAMPO_INVALIDO "Campo invalido! Informe novamente.\n"
#define ERRO_PK_REPETIDA "ERRO: Ja existe um registro com a chave primaria: %s.\n"
#define ARQUIVO_VAZIO "Arquivo vazio!"
#define INICIO_BUSCA "********************************BUSCAR********************************\n"
#define INICIO_LISTAGEM "********************************LISTAR********************************\n"
#define INICIO_ALTERACAO "********************************ALTERAR*******************************\n"
#define INICIO_ARQUIVO "********************************ARQUIVO*******************************\n"
#define INICIO_INDICE_PRIMARIO "***************************INDICE PRIMÁRIO****************************\n"
#define INICIO_INDICE_SECUNDARIO "***************************INDICE SECUNDÁRIO**************************\n"
#define SUCESSO "OPERACAO REALIZADA COM SUCESSO!\n"
#define FALHA "FALHA AO REALIZAR OPERACAO!\n"
#define NOS_PERCORRIDOS_IP "Busca por %s. Nos percorridos:\n"
#define NOS_PERCORRIDOS_IS "Busca por %s.\nNos percorridos:\n"

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

/*Estrutura da chave de um nó do Índice Primário*/
typedef struct Chaveip
{
	char pk[TAM_PRIMARY_KEY];
	int rrn;
} Chave_ip;

/*Estrutura da chave de um  do Índice Secundário*/
typedef struct Chaveis
{
	char string[TAM_STRING_INDICE];
	char pk[TAM_PRIMARY_KEY];
} Chave_is;

/* Estrutura das Árvores-B */
typedef struct nodeip
{
	int num_chaves;  /* numero de chaves armazenadas*/
	Chave_ip *chave; /* vetor das chaves e rrns [m-1]*/
	int *desc;		 /* ponteiros para os descendentes, *desc[m]*/
	char folha;		 /* flag folha da arvore*/
} node_Btree_ip;

typedef struct nodeis
{
	int num_chaves;  /* numero de chaves armazenadas*/
	Chave_is *chave; /* vetor das chaves e rrns [m-1]*/
	int *desc;		 /* ponteiros para os descendentes, *desc[m]*/
	char folha;		 /* flag folha da arvore*/
} node_Btree_is;

typedef struct
{
	int raiz;
} Indice;

/* Variáveis globais */
char ARQUIVO[TAM_ARQUIVO];
char ARQUIVO_IP[2000 * sizeof(Chave_ip)];
char ARQUIVO_IS[2000 * sizeof(Chave_is)];
int ordem_ip;
int ordem_is;

int nregistros; //Número de registros do ARQUIVO.

int nregistrosip; /*Número de nós presentes no ARQUIVO_IP*/
int nregistrosis; /*Número de nós presentes no ARQUIVO_IS*/

int tamanho_registro_ip; //Tamanho do nó ip.
int tamanho_registro_is; //Tamanho do nó is.
/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */

/* Recebe do usuário uma string simulando o arquivo completo e retorna o número
  * de registros. */
int carregar_arquivo();

/* (Re)faz o Cria iprimary*/
void criar_iprimary_iride(Indice *iprimary, Indice *iride);

/*Escreve um nó da árvore no arquivo de índice,
* O terceiro parametro serve para informar qual indice se trata */
void write_btree(void *salvar, int rrn, char ip);
void write_btreeIp(node_Btree_ip *salvar, int rrn);
void write_btreeIs(node_Btree_is *salvar, int rrn);

/*Lê um nó do arquivo de índice e retorna na estrutura*/
void read_btree(int rrn, char ip, void *retornoNo);

/* Aloca dinamicamente os vetores de chaves e descendentes */
void* criar_no(char ip);

/* Percorre a arvore e retorna o RRN da chave informada.  Retorna -1, caso não
 * encontrada. */
int buscar_chave_ip(int noderrn, char *pk, char exibir_caminho);

/* Percorre a arvore e retorna a pk da string destino/data-hora informada. Retorna -1, caso não
 * encontrada. */
int buscar_chave_is(int noderrn, char *titulo, char* pk, char exibir_caminho);

/* Realiza percurso em-ordem imprimindo todas as caronas na ordem lexicografica do destino e data/hora e o
 * nível de cada nó (raiz = nível 1) */
int imprimir_arvore_is(int noderrn, int nivel);

/*Gera Chave da struct Carona*/
void gerarChave(Carona *novo);

/* Função auxiliar que ordena as chaves em esq + a chave a ser inserida e divide
 * entre os nós esq e dir. Retorna o novo nó à direita, a chave promovida e seu
 * descendente direito, que pode ser nulo, caso a nó seja folha. */
int divide_no_ip(int rrnesq, Chave_ip *chave, int desc_dir_rrn);
int divide_no_is(int rrnesq, Chave_is *chave, int desc_dir_rrn);

/* Lista todos os registros não marcados para remoção */
void liberar(Indice iprimary, Indice iride);

/* Realiza percurso pré-ordem imprimindo as chaves primárias dos registros e o
 * nível de cada nó (raiz = nível 1) */
int imprimir_arvore_ip(int noderrn, int nivel);

/* Recupera do arquivo o registro com o rrn informado e retorna os dados na
 * struct Carona */
Carona recuperar_registro(int rrn);

/********************FUNÇÕES DO MENU*********************/
void cadastrar(Indice *iprimary, Indice *iride);

int alterar(Indice iprimary);

void buscar(Indice iprimary, Indice iride);

void listar(Indice iprimary, Indice iride);

/*******************************************************/
void libera_no(void *node, char ip);

/*Realiza os scanfs na struct Carona*/
void ler_entrada(char *registro, Carona *novo);

/* Atualiza os dois índices com o novo registro inserido */
void inserir_registro_indices(Indice *iprimary, Indice *iride, Carona j);

/* Insere um novo registro na Árvore B */
void insere_chave_ip(Indice *iprimary, Chave_ip *chave);
void insere_chave_is(Indice *iride, Chave_is *chave);

/* Função auxiliar para ser chamada recursivamente, inserir as novas chaves nas
 * folhas e tratar overflow no retorno da recursão. */
int insere_aux_ip(int noderrn, Chave_ip *chave);
int insere_aux_is(int noderrn, Chave_is *chave);

/* VOCÊS NÃO NECESSARIAMENTE PRECISAM USAR TODAS ESSAS FUNÇÕES, É MAIS PARA TEREM UMA BASE MESMO, 
 * PODEM CRIAR SUAS PRÓPRIAS FUNÇÕES SE PREFERIREM */
void inicializaNoIs(node_Btree_is *no);
void inicializaNoIp(node_Btree_ip *no);

void complementar_noIp(node_Btree_ip *salvar, char *retorno);
void complementar_noIs(node_Btree_is *salvar, char *retorno);
void complementar_registro(char* registro);

void read_btreeIp(int rrn, node_Btree_ip* auxIp);
void read_btreeIs(int rrn, node_Btree_is* auxIs);

void gerarChaveIs(char *destino, char *data, char *hora, char *string);
void ler_noIs(char *stringOriginal, char *destino, char *data, char *hora);



//Funções do primeiro trabalho
//------------------------------------------------------------
//Grava na string arquivo.
void gravarArquivo(char *registro, int nregistros);

//Srcmp para comparar as datas e horas.
int strcmpData(char *data1, char *data2);
int strcmpHora(char *tempo1, char *tempo2);
//Funções do primeiro trabalho que precisam ser readptadas.
//-------------------------------------------------------------

int main()
{
	char *p; /* # */
			 /* Arquivo */
	int carregarArquivo = 0;
	nregistros = 0, nregistrosip = 0, nregistrosis = 0;
	scanf("%d%*c", &carregarArquivo); /* 1 (sim) | 0 (nao) */
	if (carregarArquivo)
		nregistros = carregar_arquivo();

	scanf("%d %d%*c", &ordem_ip, &ordem_is);

	tamanho_registro_ip = ordem_ip * 3 + 4 + (-1 + ordem_ip) * 14;
	tamanho_registro_is = ordem_is * 3 + 4 + (-1 + ordem_is) * (TAM_STRING_INDICE + 10);

	/* Índice primário */
	Indice iprimary;
	//criar_iprimary(&iprimary);

	/* Índice secundário de nomes dos Caronas */
	Indice iride;
	//criar_iride(&iride);
	criar_iprimary_iride(&iprimary, &iride);

	/* Execução do programa */
	int opcao = 0;
	while (1)
	{
		scanf("%d%*c", &opcao);
		switch (opcao)
		{
		case 1: /* Cadastrar uma nova Carona */
			cadastrar(&iprimary, &iride);
			break;
		case 2: /* Alterar a qtd de vagas de uma Carona */
			printf(INICIO_ALTERACAO);
			if (alterar(iprimary))
				printf(SUCESSO);
			else
				printf(FALHA);
			break;
		case 3: /* Buscar uma Carona */
			printf(INICIO_BUSCA);
			buscar(iprimary, iride);
			break;
		case 4: /* Listar todos as Caronas */
			printf(INICIO_LISTAGEM);
			listar(iprimary, iride);
			break;
		case 5: /* Imprimir o arquivo de dados */
			printf(INICIO_ARQUIVO);
			printf("%s\n", (*ARQUIVO != '\0') ? ARQUIVO : ARQUIVO_VAZIO);
			break;
		case 6: /* Imprime o Arquivo de Índice Primário*/
			printf(INICIO_INDICE_PRIMARIO);
			if (!*ARQUIVO_IP)
				puts(ARQUIVO_VAZIO);
			else
				for (p = ARQUIVO_IP; *p != '\0'; p += tamanho_registro_ip)
				{
					fwrite(p, 1, tamanho_registro_ip, stdout);
					puts("");
				}
			break;
		case 7: /* Imprime o Arquivo de Índice Secundário*/
			printf(INICIO_INDICE_SECUNDARIO);
			if (!*ARQUIVO_IS)
				puts(ARQUIVO_VAZIO);
			else
				for (p = ARQUIVO_IS; *p != '\0'; p += tamanho_registro_is)
				{
					fwrite(p, 1, tamanho_registro_is, stdout);
					puts("");
				}
			//printf("%s\n", ARQUIVO_IS);
			break;
		case 8: /*Libera toda memória alocada dinâmicamente (se ainda houver) e encerra*/
			return 0;
		default: /* exibe mensagem de erro */
			printf(OPCAO_INVALIDA);
			break;
		}
	}
	return -1;
}

/* ==========================================================================
 * ================================= FUNÇÕES ================================
 * ========================================================================== */

/* Recebe do usuário uma string simulando o arquivo completo e retorna o número
 * de registros. */
int carregar_arquivo()
{
	scanf("%[^\n]%*c", ARQUIVO);
	return strlen(ARQUIVO) / TAM_REGISTRO;
}

/* Exibe a Carona */
int exibir_registro(int rrn)
{
	
	if (rrn < 0) return 0;

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

/* Recupera do arquivo o registro com o rrn informado e retorna os dados na
 * struct Carona */
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

//O iprimary define o tamanho dos nregistros para não ter redundância em todos os indices.
void criar_iprimary_iride(Indice *iprimary, Indice *iride){
			
	Carona car;

	ARQUIVO_IP[0] = '\0';
	ARQUIVO_IS[0] = '\0';	
	iprimary->raiz = -1;
	iride->raiz = -1;
	int n = nregistros; 
	nregistros = 0;
	//A cada registro recuperado mandamos inserir no indice_p.
	for(int i = 0; i < n; i++, nregistros++){
		car = recuperar_registro(i);
		inserir_registro_indices(iprimary, iride, car);		
	}
			
}


void inicializaNoIp(node_Btree_ip *no){
	for(int i = 0; i < (ordem_ip-1); i++){
		no->chave[i].pk[0] = '\0';
		no->chave[i].rrn = -1;
	}
	for(int i = 0; i < ordem_ip; i++){
		no->desc[i] = -1;
	}

	no->folha = 'T';
	no->num_chaves = 0;
}

void inicializaNoIs(node_Btree_is *no){
	for(int i = 0; i < ordem_is-1; i++){
		no->chave[i].pk[0] = '\0';
		no->chave[i].string[0] = '\0';
	}
	for(int i = 0; i < ordem_is; i++){
		no->desc[i] = -1;
	}

	no->folha = 'T';
	no->num_chaves = 0;
}

void read_btreeIp(int rrn, node_Btree_ip* retornoNo){

	char temp[tamanho_registro_ip+1]; //Variável temp para receber o nó do ARQUIVO_IP.
	
	char auxNumChaves[4]; //Variável auxNumChaves para receber o número de chaves de temp.
	char auxRrn[5];	//Variável auxRrn para receber o rrn das chaves de temp.
	char filhoRrn[4]; // //Variável filhoRrn para receber o rrn do nó filho de temp.

	//Pegando o nó do ARQUIVO_IP e passando para temp.
	strncpy(temp, ARQUIVO_IP + (rrn*tamanho_registro_ip), tamanho_registro_ip);
	temp[tamanho_registro_ip] = '\0';

	//Pegando a quantidade de chaves no nó e convertendo para inteiro.
	strncpy(auxNumChaves,temp,3);
	auxNumChaves[3] = '\0';
	sscanf(auxNumChaves,"%d",&retornoNo->num_chaves);
	
	//Pegando o byte que indica se é folha ou não.
	retornoNo->folha = temp[3+((TAM_PRIMARY_KEY-1)+4)*(ordem_ip-1)];

	for(int i = 0, j = 0; i < retornoNo->num_chaves; i++,j+=(4+(TAM_PRIMARY_KEY-1)) ){
		auxRrn[0] = '\0';
		//Estamos pegando a chave pk. 
		strncpy(retornoNo->chave[i].pk, temp+3+j, (TAM_PRIMARY_KEY-1));
		retornoNo->chave[i].pk[TAM_PRIMARY_KEY-1] = '\0';
		//Estamos pegando o rrn.
		strncpy(auxRrn, temp+3+j+(TAM_PRIMARY_KEY-1), 4);
		auxRrn[4] = '\0';	
		//Convertendo o rrn colocado no auxRrrn e colocando na estrutura.
		sscanf(auxRrn,"%d",&(retornoNo->chave[i].rrn));
	}

	//Se não for folha.
	if(retornoNo->folha != 'T'){
		//Pegamos todos os rrns dos nós filhos.
		for(int i = 0, j = 0; i < (retornoNo->num_chaves+1); i++, j+=3){
			filhoRrn[0] = '\0';
			//Estamos pegando os rrns dos nós filhos.
			strncpy(filhoRrn, temp+3+((TAM_PRIMARY_KEY-1)+4)*(ordem_ip-1)+1+j, 3);	
			filhoRrn[3] = '\0';
			//Convertendo o rrn colocado no auxRrrn e colocando na estrutura.
			sscanf(filhoRrn,"%d",&(retornoNo->desc[i]));
		}
	}
}

void read_btreeIs(int rrn, node_Btree_is *retornoNo){
	int k;
	char temp[tamanho_registro_is+1]; //Variável temp para receber o nó do ARQUIVO_IS.
	
	char auxNumChaves[4]; //Variável auxNumChaves para receber o número de chaves de temp.
	char stringAux[TAM_STRING_INDICE+1];	//Variável stringAux para receber a substring de temp.
	char filhoRrn[4]; // //Variável filhoRrn para receber o rrn do nó filho de temp.

	//Pegando o nó do ARQUIVO_IS e passando para temp.
	strncpy(temp, ARQUIVO_IS + (rrn*tamanho_registro_is), tamanho_registro_is);
	temp[tamanho_registro_is] = '\0';

	//Pegando a quantidade de chaves no nó e convertendo para inteiro.
	strncpy(auxNumChaves,temp,3);
	auxNumChaves[3] = '\0';
	sscanf(auxNumChaves,"%d",&retornoNo->num_chaves);
	
	//Pegando o byte que indica se é folha ou não.
	retornoNo->folha = temp[3+(TAM_PRIMARY_KEY-1+TAM_STRING_INDICE)*(ordem_is-1)];
	
	for(int i = 0, j = 0; i < retornoNo->num_chaves; i++,j+=(TAM_PRIMARY_KEY-1+TAM_STRING_INDICE)){
		
		stringAux[0] = '\0';

		//Estamos pegando a chave pk. 
		strncpy(retornoNo->chave[i].pk, temp+3+j, TAM_PRIMARY_KEY-1);
		retornoNo->chave[i].pk[TAM_PRIMARY_KEY-1] = '\0';
		//Estamos pegando a string.
		strncpy(stringAux, temp+3+j+(TAM_PRIMARY_KEY-1), (TAM_STRING_INDICE-1));	
		
		for(k = 0; k < TAM_STRING_INDICE && stringAux[k] != '#'; k++){ 
			retornoNo->chave[i].string[k] = stringAux[k]; }
		retornoNo->chave[i].string[k] = '\0';
	}

	if(retornoNo->folha != 'T'){
		for(int i = 0, j = 0; i < retornoNo->num_chaves+1; i++, j+=3){
			filhoRrn[0] = '\0';

			strncpy(filhoRrn, temp+3+(TAM_PRIMARY_KEY-1+TAM_STRING_INDICE)*(ordem_is-1)+1+j, 3);	
			filhoRrn[3] = '\0';
			//Convertendo o rrn colocado no auxRrrn e colocando na estrutura.
			sscanf(filhoRrn,"%d",&(retornoNo->desc[i]));
		}
	}
}


void read_btree(int rrn, char ip, void *retornoNo){
	if(ip == 'T'){
		read_btreeIp(rrn, (node_Btree_ip*)retornoNo);
	}
	else{
		read_btreeIs(rrn, (node_Btree_is*)retornoNo);
	}
}



void complementar_noIp(node_Btree_ip *salvar, char *retorno){
	
	char subReg[4+TAM_PRIMARY_KEY];
	char auxRrnFilho[4];
	char auxFolha[1];

	auxFolha[0] = salvar->folha;
	auxFolha[1] = '\0';

	retorno[0] = '\0';

	//Colocando a quantidade de chaves no nó na variável no.
	sprintf(retorno,"%03d",salvar->num_chaves);

	for(int i = 0; i < (ordem_ip-1); i++){
			
		//Verificando se o rrn é nulo, se for, colocamos ############## que representa a pk + rrn.
		if(salvar->chave[i].rrn == -1){
			strcat(retorno,"##############");
		} else {
			//Se não, colocamos a chave e o rrn.
			subReg[0] = '\0';
			sprintf(subReg,"%s%04d",salvar->chave[i].pk, salvar->chave[i].rrn);
			strcat(retorno,subReg);
		}	
	}

	//Colocando a folha.
	strcat(retorno,auxFolha);

	for(int i = 0; i < ordem_ip; i++){
		//Veificando se o rrn do filho é nulo, se for, colocamos ***.
		if(salvar->desc[i] == -1){
			strcat(retorno,"***");
		} else {
			//Se não, colocamos o rrn do filho.
			auxRrnFilho[0] = '\0';

			sprintf(auxRrnFilho,"%03d", salvar->desc[i]);
			strcat(retorno,auxRrnFilho);
		}
	}
	retorno[tamanho_registro_ip] = '\0';
}

void complementar_noIs(node_Btree_is *salvar, char *retorno){
	//51, pois é 41 string + 11 pk = 52.
	char auxString[TAM_STRING_INDICE+TAM_PRIMARY_KEY];
	char auxRrnFilho[4];
	char auxFolha[1];
	
	auxFolha[0] = salvar->folha;
	auxFolha[1] = '\0';

	retorno[0] = '\0';

	//Colocando a quantidade de chaves do nó na variável no.
	sprintf(retorno,"%03d", salvar->num_chaves);

	for(int i = 0; i < (ordem_is-1); i++){
		
		if(salvar->chave[i].string[0] == '\0'){
						  //MRDD160120OSASCO$1901162048########################
			strcat(retorno,"###################################################");
		} else {

			//Se não, colocamos o a chave e a string.
			auxString[0] = '\0';
			sprintf(auxString,"%s%s",salvar->chave[i].pk, salvar->chave[i].string);

			//Concatenando os ## para preencher o resto da chave não utilizado.
			for(int j = strlen(auxString); j < (TAM_PRIMARY_KEY-1+TAM_STRING_INDICE); j++){
				strcat(auxString,"#");
			}

			strcat(retorno, auxString);
		}	
	}
	//Colocando a folha.
	strcat(retorno,auxFolha);

	for(int i = 0; i < ordem_is; i++){
		//Veificando se o rrn do filho é nulo, se for, colocamos ***.
		if(salvar->desc[i] == -1){
			strcat(retorno,"***");
		} else {
			//Se não, colocamos o rrn do filho.
			auxRrnFilho[0] = '\0';

			sprintf(auxRrnFilho,"%03d", salvar->desc[i]);
			strcat(retorno, auxRrnFilho);
		}
	}
	retorno[tamanho_registro_is] = '\0';
}

void write_btreeIp(node_Btree_ip *salvar, int rrn){
	char retorno[tamanho_registro_ip+1];
	complementar_noIp(salvar, retorno);

	for(int i = rrn*tamanho_registro_ip, j = 0; i < (rrn*tamanho_registro_ip+tamanho_registro_ip); i++, j++){
		ARQUIVO_IP[i] = retorno[j];
	}

	if((rrn*tamanho_registro_ip + tamanho_registro_ip) == (nregistrosip * tamanho_registro_ip + tamanho_registro_ip)){
		ARQUIVO_IP[nregistrosip * tamanho_registro_ip + tamanho_registro_ip] = '\0';
	}
}

void write_btreeIs(node_Btree_is *salvar, int rrn){
	char retorno[tamanho_registro_is+1];
	complementar_noIs(salvar, retorno);

	for(int i = rrn*tamanho_registro_is, j = 0; i < (rrn*tamanho_registro_is+tamanho_registro_is); i++, j++){
		ARQUIVO_IS[i] = retorno[j];
	}

	if((rrn*tamanho_registro_is + tamanho_registro_is) == (nregistrosis * tamanho_registro_is + tamanho_registro_is)){
		ARQUIVO_IS[nregistrosis * tamanho_registro_is + tamanho_registro_is] = '\0';
	}
}

void write_btree(void *salvar, int rrn, char ip){
	if(ip == 'T') write_btreeIp((node_Btree_ip*) salvar, rrn);
	else write_btreeIs((node_Btree_is*) salvar, rrn);
}

/* Percorre a arvore e retorna o RRN da chave informada.  Retorna -1, caso não
 * encontrada. */
int buscar_chave_ip(int noderrn, char *pk, char exibir_caminho){
	
	if(noderrn == -1) return -1;

	int i = 0, retorno;

	node_Btree_ip* noIp = (node_Btree_ip*) criar_no('T');
	read_btree(noderrn, 'T', noIp);

	/*
	Se controle for:
		* = -1: Se controle se manter = -1 ocorreu um erro.
		* = 0: Se controle for = 0 achamos o intervalo em que a chave se encontra.
		* = 1: Se controle for = 1 achamos a chave. 
	*/

	int controle = -1;
	if(exibir_caminho == 'T'){
	
		for(i = 0; i < noIp->num_chaves; i++){
			printf("%s",noIp->chave[i].pk);
			if(i != (noIp->num_chaves -1)) printf(", ");		
		}
	
		printf("\n");
	}

	for(i = 0; i < noIp->num_chaves; i++){
		
		//Se for a última.
		if(i == (noIp->num_chaves-1) && strcmp(pk,noIp->chave[i].pk) > 0){ 
			i+=1;
			controle = 0;
			break;
		}
		//Se for a comparação com a primeira pk.
		else if(i == 0) {
			if(strcmp(pk,noIp->chave[i].pk) < 0){ 
				controle = 0;
				break;
			}	
			else if(strcmp(pk,noIp->chave[i].pk) == 0){
				controle = 1;
				break;
			}
		//Se for diferente da primeira comparação.
		} else if(i != 0){
			//Se estivermos nos intervalos!.
			if(strcmp(pk,noIp->chave[i-1].pk) > 0 && strcmp(pk,noIp->chave[i].pk) < 0){
				controle = 0;
				break;
			}
			else if(strcmp(pk,noIp->chave[i].pk) == 0){
				controle = 1;
				break;
			}	
		} 	

	}

	if(controle == -1){
		libera_no(noIp,'T');
		return -1;
	}
	else if(controle == 0){	
		retorno = buscar_chave_ip(noIp->desc[i],pk,exibir_caminho);
		libera_no(noIp,'T');
		return retorno;
	}
	else if(controle == 1){
		retorno = noIp->chave[i].rrn;
		libera_no(noIp,'T');
		return retorno;
	}
}

/* Percorre a arvore e retorna a pk da string destino/data-hora informada. Retorna -1, caso não
 * encontrada. */
int buscar_chave_is(int noderrn, char *titulo, char *pk, char exibir_caminho){

		
	if(noderrn == -1) return -1;

	int i = 0, retorno;

	node_Btree_is* noIs = (node_Btree_is*) criar_no('F');
	read_btree(noderrn, 'F', noIs);

	/*
	Se controle for:
		* = -1: Se controle se manter = -1 ocorreu um erro.
		* = 0: Se controle for = 0 achamos o intervalo em que a chave se encontra.
		* = 1: Se controle for = 1 achamos a chave. 
	*/

	int controle = -1;
	if(exibir_caminho == 'T'){
	
		for(i = 0; i < noIs->num_chaves; i++){
			printf("%s",noIs->chave[i].string);
			if(i != (noIs->num_chaves -1)) printf(", ");		
		}
	
		printf("\n");
	}

	for(i = 0; i < noIs->num_chaves; i++){
		//Se for a última.
		if(i == (noIs->num_chaves-1) && strcmp(titulo,noIs->chave[i].string) > 0){ 
			i+=1;
			controle = 0;
			break;
		}
		//Se for a comparação com a primeira pk.
		else if(i == 0) {
			if(strcmp(titulo,noIs->chave[i].string) < 0){ 
				controle = 0;
				break;
			}	
			else if(strcmp(titulo,noIs->chave[i].string) == 0){
				controle = 1;
				break;
			}
		//Se for diferente da primeira comparação.
		} else if(i != 0){
			//Se estivermos nos intervalos!.
			if(strcmp(titulo,noIs->chave[i-1].string) > 0 && 
			strcmp(titulo,noIs->chave[i].string) < 0){
				controle = 0;
				break;
			}
			else if(strcmp(titulo,noIs->chave[i].string) == 0){
				controle = 1;
				break;
			}	
		} 	

	}

	if(controle == -1){
		libera_no(noIs,'F');
		return -1;
	}
	else if(controle == 0){	
		retorno = buscar_chave_is(noIs->desc[i], titulo, pk, exibir_caminho);
		libera_no(noIs,'F');
		return retorno;
	}
	else if(controle == 1){
		strcpy(pk, noIs->chave[i].pk);
		libera_no(noIs,'F');
		return retorno;
	}
}

void gerarChaveIs(char *destino, char *data, char *hora, char *string){
	string[0] = '\0';
	char aux[12];

	//Fazendo parte da chave com a data e a hora.	
	aux[0] = '$';
	aux[1] = data[6];
	aux[2] = data[7];
	aux[3] = data[3];
	aux[4] = data[4];
	aux[5] = data[0];
	aux[6] = data[1];

	aux[7] = hora[0];
	aux[8] = hora[1];
	aux[9] = hora[3];
	aux[10] = hora[4];
	aux[11] = '\0';
	//Passando o destino para a string a ser retornada e concatenando o $+data+hora.
	strcpy(string,destino);
	strcat(string,aux);
}

void buscar(Indice iprimary, Indice iride){
	int opcao, retorno = -1;

	char destino[TAM_STRING_INDICE], data[TAM_DATA], hora[TAM_HORA], pk[TAM_PRIMARY_KEY];
	char string[TAM_STRING_INDICE]; 

	scanf("%d%*c", &opcao);
	
	switch (opcao)
	{
		case 1:
			scanf("%[^\n]%*c",pk); //Leio a chave.
			printf(NOS_PERCORRIDOS_IP,pk);
			retorno = buscar_chave_ip(iprimary.raiz,pk,'T');

			if(retorno != -1){
				printf("\n");
				exibir_registro(retorno);
			}
		break;

		case 2:
			scanf("%[^\n]%*c",destino); //Leio a chave.
			scanf("%[^\n]%*c",data); //Leio a chave.
			scanf("%[^\n]%*c",hora); //Leio a chave.
			
			gerarChaveIs(destino,data,hora,string);
			printf(NOS_PERCORRIDOS_IS,string);
			retorno = buscar_chave_is(iride.raiz, string, pk, 'T');
			
			if(retorno != -1){
				retorno = buscar_chave_ip(iprimary.raiz,pk,'F');
				if(retorno != -1){
					printf("\n");
					exibir_registro(retorno);
				}
			}
		break;
	}

	if(retorno == -1){
		printf("\n");
		printf(REGISTRO_N_ENCONTRADO);
	}
}

int alterar(Indice iprimary){
	
	int rrn, flag = 0;
	char pk[TAM_PRIMARY_KEY];
	
	scanf("%[^\n]%*c", pk);

	rrn = buscar_chave_ip(iprimary.raiz, pk, 'F');
	
	//Se o índice for -1 ou seu rrn -1 então o registro não existe.
	if(rrn == -1){
		printf(REGISTRO_N_ENCONTRADO);
		return 0;
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

	}while(vagaInt < 1 || vagaInt >= 9);
	
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

void libera_no(void *node, char ip){

	if(ip == 'T'){
		node_Btree_ip *nodeIp = (node_Btree_ip*) node;
		free(nodeIp->chave);
		free(nodeIp->desc);
		free(nodeIp);
	} else {
		node_Btree_is *nodeIs = (node_Btree_is*) node;
		free(nodeIs->chave);
		free(nodeIs->desc);
		free(nodeIs);
	}
}

void* criar_no(char ip){
	
	if(ip == 'T'){
		node_Btree_ip *nodeIp = (node_Btree_ip*) malloc(sizeof(node_Btree_ip));
		nodeIp->chave = (Chave_ip*) malloc(sizeof(Chave_ip)*(ordem_ip-1));
		nodeIp->desc = (int*) malloc(sizeof(int)*(ordem_ip)); 
		inicializaNoIp(nodeIp);

		return (void*) nodeIp;
	} else {
		node_Btree_is *nodeIs = (node_Btree_is*) malloc(sizeof(node_Btree_is));
		nodeIs->chave = (Chave_is*) malloc(sizeof(Chave_is)*(ordem_is-1));
		nodeIs->desc = (int*) malloc(sizeof(int)*(ordem_is));		
		inicializaNoIs(nodeIs);

		return (void*) nodeIs;	
	}
}

void listarIp_preOrdem(int rrn, int nivel, int nregistrosIp){
	//Se o rrn for -1 então não temos mais nós para percorrer.	
	if(rrn == -1) return;
	//Lemos o nó do rrn passado como parâmetro.
	node_Btree_ip *nodeIp = (node_Btree_ip*) criar_no('T');
	read_btree(rrn,'T', nodeIp);
	
	//Printamos o nível.
	printf("%d - ",nivel);

	//Percorremos todos os registros do nó e vamos imprimindo.
	for(int i = 0; i < nodeIp->num_chaves; i++){
		printf("%s",nodeIp->chave[i].pk);
		if(i != (nodeIp->num_chaves -1)) printf(", ");		
	}
	printf("\n");

	//Fazemos uma recursiva para os seus descendentes.
	for(int i = 0; i < nodeIp->num_chaves+1; i++){
		listarIp_preOrdem(nodeIp->desc[i], nivel+1, nregistrosIp-1);		
	}
	//Após a recursão liberamos o nó.
	libera_no(nodeIp, 'T');
}

void imprimirAux_EmOrdem(char *entrada){
	
	char saida[49];
	char string[11];
	char data[TAM_DATA];
	char hora[TAM_HORA];
	//char destino[TAM_STRING_INDICE - 6 -4];

	int i,j;
	for(i = 0; entrada[i] != '$'; i++);
	
	strncpy(saida, entrada, i);
	saida[i] = '\0';
	for(int k = strlen(saida); k < 30; k++) strcat(saida,"-");

	strncpy(string, entrada + i+1, 10);
	string[10] = '\0';

	data[0] = string[4];
	data[1] = string[5];
	data[2] = '/';
	data[3] = string[2];
	data[4] = string[3];
	data[5] = '/';
	data[6] = string[0];
	data[7] = string[1];
	data[8] = '\0';

	hora[0] = string[6];
	hora[1] = string[7];
	hora[2] = ':';
	hora[3] = string[8];
	hora[4] = string[9];
	hora[5] = '\0';

	strcat(saida," ");
	strcat(saida,data);
	strcat(saida," - ");

	strcat(saida,hora);
	strcat(saida,"- ");
	printf("%s\n",saida);
}

void listarIs_emOrdem(int rrn){
	if(rrn == -1) return;
	int i;
	char saida[49];

	node_Btree_is *nodeIs = (node_Btree_is*) criar_no('F');
	read_btree(rrn,'F', nodeIs);

	for(i = 0; i < nodeIs->num_chaves; i++){
		listarIs_emOrdem(nodeIs->desc[i]);
		imprimirAux_EmOrdem(nodeIs->chave[i].string);	
	}
	//Para o último descendente a direita.
	listarIs_emOrdem(nodeIs->desc[nodeIs->num_chaves]);
	
	//imprimirAux_EmOrdem(nodeIs->chave[i].string, saida);
	libera_no(nodeIs,'F');
}

void listar(Indice iprimary, Indice iride){
	int opcao, retorno = -1;
	char destino[30], data[TAM_DATA], hora[TAM_HORA];	
	char string[TAM_STRING_INDICE], pk[TAM_PRIMARY_KEY];

	scanf("%d%*c", &opcao);
	
	switch (opcao)
	{
		case 1:
			listarIp_preOrdem(iprimary.raiz,1,nregistrosip);
			if(iprimary.raiz == -1) printf(REGISTRO_N_ENCONTRADO);
		break;

		case 2:
			listarIs_emOrdem(iride.raiz);
			if(iride.raiz == -1) printf(REGISTRO_N_ENCONTRADO);
		break;
	}
}

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

void complementar_registro(char* registro){
	for(int i = strlen(registro); i < TAM_REGISTRO; i++){
		strcat(registro,"#");
	}
}

void gravarArquivo(char *registro, int nregistros){
	complementar_registro(registro);	
	strcat(ARQUIVO,registro);
}

int divide_no_is(int rrnesq, Chave_is *chave, int desc_dir_rrn){
	//Lendo o nó que ocorreu o split.
	node_Btree_is *noEsquerda = (node_Btree_is*) criar_no('F');
	read_btree(rrnesq, 'F', noEsquerda);
	
	//Que irá receber o indice da chave promida no vetorChave.
	int indiceChavePromovida;
	int i, j, retorno;

	Chave_is trocar;
	Chave_is vetorChaves[ordem_is];
	
	int desc[ordem_is+1];
	
	/*Passando todos as chaves para o vetorChaves e
	todos os desc para o vetor de desc.*/
	for(i = 0; i < noEsquerda->num_chaves; i++){
		strcpy(vetorChaves[i].string, noEsquerda->chave[i].string);
		strcpy(vetorChaves[i].pk, noEsquerda->chave[i].pk);
		
		desc[i] = noEsquerda->desc[i];
	}
	//Para colocar o último descendente.
	desc[i] = noEsquerda->desc[i];
	
	//Setamos o i com 
	i = ordem_is-1;
	
	//Colocamos os valores no final do vetor de chaves.
	strcpy(vetorChaves[ordem_is-1].string, chave->string);
	strcpy(vetorChaves[ordem_is-1].pk, chave->pk);

	//Para fazermos os desc irem um índice para frente.
	
	while(i){
		//Fazemos as trocas toda vez que o anterior for maior que o sucessor.
		if(strcmp(vetorChaves[i-1].string, vetorChaves[i].string) > 0){
			//Passamos o sucessor para o trocar.
			strcpy(trocar.string, vetorChaves[i].string);
			strcpy(trocar.pk, vetorChaves[i].pk);
			//O sucessor para o antecessor.
			strcpy(vetorChaves[i].string, vetorChaves[i-1].string);
			strcpy(vetorChaves[i].pk, vetorChaves[i-1].pk);
			//E o sucessor para o antecessor.
			strcpy(vetorChaves[i-1].string, trocar.string);
			strcpy(vetorChaves[i-1].pk, trocar.pk);
			
			//Movimentamos o descendente para frente toda vez que a chave se move.
			desc[i+1] = desc[i];
		} else {
			//Quando não podemos mover o descendente implica que ele é a posição do descendente.
			break;
		}		
		i--;
	}		
	//if(i == 0) desc[k] = desc_dir_rrn;
	desc[i+1] = desc_dir_rrn;
	//Criando um novo nó que irá ser retornado.
	node_Btree_is *noDireito = (node_Btree_is*) criar_no('F');

	//Recebemos o indice da chave promovida que é exatamente a metade ou valor inteiro.
	indiceChavePromovida = (int)(ordem_is/2);
	
	//Para deixar todos os dados setados como nulo antes de inserir novamente.
	inicializaNoIs(noEsquerda);

	//Passamos as chaves e os descendentes para o nó a esquerda.
	for(i = 0; i < indiceChavePromovida; i ++){
		//Colocando os dado das Chave_is no vetor.
		strcpy(noEsquerda->chave[i].string, vetorChaves[i].string);
		strcpy(noEsquerda->chave[i].pk, vetorChaves[i].pk);
		//Passando os descendentes.
		noEsquerda->desc[i] = desc[i];
	}
	//Para colocar o último descendente.
	noEsquerda->desc[i] = desc[i];
	/* Como a quantidade de chaves é sempre uma a mais do que inserimos,
	passamos sempre o i para ter esse controle. */
	noEsquerda->num_chaves = i;
	

	for(i = 0, j = indiceChavePromovida+1; j < ordem_is; i++, j++){
		//Passamos as demais pro outro vetor.
		strcpy(noDireito->chave[i].string, vetorChaves[j].string);
		strcpy(noDireito->chave[i].pk, vetorChaves[j].pk);
		
		noDireito->desc[i] = desc[j];
	}
	//Para colocar o último descendente.
	noDireito->desc[i] = desc[j];
	//Idem do nó a esquerda.
	noDireito->num_chaves = i;
	

	//Se for != -1, então nosso nó criado não é a folha.
	if(desc_dir_rrn != -1){
		noDireito->folha = 'F';
		noEsquerda->folha = 'F';
	} 

	//Escrevemos os nós no arquivo IP.
	write_btree(noEsquerda, rrnesq, 'F');
	write_btree(noDireito, nregistrosis, 'F');
	retorno = nregistrosis;//Salvamos o retorno.
	nregistrosis+=1; //Incrementamos a quantidade de nós.
	
	//Copiamos a chave promovida para ser retornada acima e retornamos.
	strcpy(chave->string, vetorChaves[indiceChavePromovida].string);
	strcpy(chave->pk, vetorChaves[indiceChavePromovida].pk);
	//Liberamos os nós.
	libera_no(noEsquerda, 'F');
	libera_no(noDireito, 'F');
	
	return retorno;
}

int insere_aux_is(int noderrn, Chave_is *chave){
	
	int filhoDireito = -1, i;

	//Estamos lendo o primeiro nó para fazer as comparações.
	node_Btree_is *noIs = (node_Btree_is*) criar_no('F');

	read_btree(noderrn,'F', noIs);
	
	//Se o nó for folha.
	if(noIs->folha == 'T'){
		//Se a chave servir no nó.
		if(noIs->num_chaves < (ordem_is-1)){
			//Inserção ordenada no vetor. (meu próprio código :D )
			Chave_is trocar;
			i = noIs->num_chaves;

			//Colocamos os valores em uma struct que será utilizada para trocar.
			strcpy(noIs->chave[i].string, chave->string);
			strcpy(noIs->chave[i].pk, chave->pk);
			
			while(i){
				if(strcmp(noIs->chave[i-1].string,noIs->chave[i].string) > 0){
					strcpy(trocar.string, noIs->chave[i].string);
					strcpy(trocar.pk, noIs->chave[i].pk);

					strcpy(noIs->chave[i].string, noIs->chave[i-1].string);
					strcpy(noIs->chave[i].pk, noIs->chave[i-1].pk);

					strcpy(noIs->chave[i-1].string, trocar.string);
					strcpy(noIs->chave[i-1].pk, trocar.pk);
				} else {
					break;
				}		
				i--;
			}
			noIs->num_chaves+=1;
			//Gravamos no arquivo IS.
			write_btree(noIs, noderrn, 'F');

			//Liberamos.
			libera_no(noIs,'F');
			//Deixamos chave == NULL para não ocorrer split em cima.
			chave->string[0] = '\0';
			chave->pk[0] = '\0';

			return -1;
		//Porém, se o nó for folha mas estiver cheio, precisamos fazer o split.	
		} else {
			
			libera_no(noIs, 'F');
			//O nó a direita é nulo porque o noderrrn é uma folha.
			return divide_no_is(noderrn, chave, -1);
		}
	} else {
		//Para achar o rrn do próximo nó percorremos o vetor de chaves fazendo as comparações.
		for(i = 0; i < noIs->num_chaves; i++){
			//Se for maior que o último.
			if(i == (noIs->num_chaves-1) && strcmp(chave->string,noIs->chave[i].string) > 0){ 
				i+=1;
				break;
			}
			//Se for a comparação com a primeira string.
			else if(i == 0) {
				if(strcmp(chave->string,noIs->chave[i].string) < 0) break;
				//Se for diferente da primeira comparação.
			} else if(i != 0){
				//Se estivermos nos intervalos!.
				if(strcmp(chave->string,noIs->chave[i-1].string) > 0 && 
				strcmp(chave->string,noIs->chave[i].string) < 0){
					break;
				}
				//Se for a última.
			}	
		} 

		filhoDireito = insere_aux_is(noIs->desc[i], chave);
		
		//Porém, se ela for != nulo temos um split para tratar.	
		if(filhoDireito != -1){
			//Verificamos se o nó não está cheio.
			if(noIs->num_chaves < (ordem_is-1)){
				//Se o nó não estiver cheio, vamos inserir a chave promovida nele.
				//Primeiramente, temos de passar as chaves para frente, pois
				//precisamos abrir espaço para a chave que irá ser inserida.
				int j = noIs->num_chaves; //FOi alterado.
				//Observe o seguinte, quando ocorre overflow em um desce i, o lugar que a chave
				//promovida será inserida será no indice i no vetor de chaves.
				//Então, passamos todas as chaves para frente incluindo o i para 
				//abrir espaço para a chave promovida.
				
				//O erro pode estar aqui.
				while(i <= j){
					//Passando as chaves para frente.
					strcpy(noIs->chave[j].string, noIs->chave[j-1].string);
					strcpy(noIs->chave[j].pk, noIs->chave[j-1].pk);
					
					
					//Passando os descendentes para frente, observe que
					//precisamos levar em consideração que os descentes tem sempre um dado
					//no vetor a mais que as chaves.
					noIs->desc[j+1] = noIs->desc[j];
					j--;
				}
				//E por fim, colocando a chave promovida.
				strcpy(noIs->chave[i].string, chave->string);
				strcpy(noIs->chave[i].pk, chave->pk);
				//E tbm, o novo descendente.
				noIs->desc[i+1] = filhoDireito;
				//Incrementamos a chave.
				noIs->num_chaves+=1;
				//Gravamos no arquivo IP.
				write_btree(noIs, noderrn, 'F');
				//Liberamos.
				libera_no(noIs,'F');
				//Deixamos chave == NULL para não ocorrer split em cima.
				chave->string[0] = '\0';
				chave->pk[0] = '\0';
				return -1;
			}
			//porém, se não temos espaço, precisaremos fazer um novo split.
			else {
				libera_no(noIs, 'F');
				//O nó a direita é nulo porque o noderrrn é uma folha.
				return divide_no_is(noderrn, chave, filhoDireito);	
			}
		//Porém, se a chave for igual a null.
		} 
		libera_no(noIs, 'F');
		chave->string[0] = '\0';
		chave->pk[0] = '\0';
		return -1;
	}
}

void insere_chave_is(Indice *iride, Chave_is *chave){
	int filhoDireita;

	//Se for o primeiro nó a ser inserido.	
	if(iride->raiz == -1){
		//Criamos um nó.
		node_Btree_is *raiz =  (node_Btree_is *)criar_no('F');
		//Passamos a chave para o primeoiro nó.
		strcpy(raiz->chave[0].string, chave->string);
		strcpy(raiz->chave[0].pk, chave->pk);
		
		raiz->num_chaves+=1; //Incrementamos a quantidade de chaves.
		//indicamos quem é o nó raiz.
		iride->raiz = nregistrosis;
		
		//Gravamos o nó no arquivo IS.
		write_btree(raiz, nregistrosis, 'F');
		
		//Somamos +1 no arquivo da árvore IS e liberamos o nó.
		nregistrosis+=1;
		libera_no(raiz,'F');
	} else {

		filhoDireita = insere_aux_is(iride->raiz, chave);
		
		if(filhoDireita != -1){
			//Criamos a nova raiz.
			node_Btree_is *novaRaiz = (node_Btree_is*) criar_no('F');
			//Como não é mais a primeiro nó, então seu nó não é folha.
			novaRaiz->folha = 'F';
			
			//Passamos a chave para o primeoiro nó.
			strcpy(novaRaiz->chave[0].pk, chave->pk);
			strcpy(novaRaiz->chave[0].string, chave->string);
			//Incrementamos a quantidade de chaves.	
			novaRaiz->num_chaves+=1;

			//O que ocorre é que o retorno do split será sempre o filho a direita.
			novaRaiz->desc[0] = iride->raiz;
			novaRaiz->desc[1] = filhoDireita;

			//indicamos quem é o novo nó raiz.
			iride->raiz = nregistrosis;

			//Gravamos o nó no arquivo IS.
			write_btree(novaRaiz, nregistrosis, 'F');

			//Somamos +1 no arquivo da árvore IS e liberamos o nó.
			nregistrosis+=1;
			libera_no(novaRaiz,'F');
		}
	}
}

int divide_no_ip(int rrnesq, Chave_ip *chave, int desc_dir_rrn){
	//Lendo o nó que ocorreu o split.
	node_Btree_ip *noEsquerda = (node_Btree_ip*) criar_no('T');
	read_btree(rrnesq, 'T', noEsquerda);
	
	//Que irá receber o indice da chave promida no vetorChave.
	int indiceChavePromovida;
	int i, j, retorno;

	Chave_ip trocar;
	Chave_ip vetorChaves[ordem_ip];
	
	int desc[ordem_ip+1];
	
	/*Passando todos as chaves para o vetorChaves e
	todos os desc para o vetor de desc.*/
	for(i = 0; i < noEsquerda->num_chaves; i++){
		strcpy(vetorChaves[i].pk, noEsquerda->chave[i].pk);
		vetorChaves[i].rrn = noEsquerda->chave[i].rrn;
		desc[i] = noEsquerda->desc[i];
	}
	//Para colocar o último descendente.
	desc[i] = noEsquerda->desc[i];
	
	//Setamos o i com 
	i = ordem_ip-1;
	
	//Colocamos os valores no final do vetor de chaves.
	strcpy(vetorChaves[ordem_ip-1].pk, chave->pk);
	vetorChaves[ordem_ip-1].rrn = chave->rrn;

	while(i){
		//Fazemos as trocas toda vez que o anterior for maior que o sucessor.
		if(strcmp(vetorChaves[i-1].pk, vetorChaves[i].pk) > 0){
			//Passamos o sucessor para o trocar.
			strcpy(trocar.pk, vetorChaves[i].pk);
			trocar.rrn = vetorChaves[i].rrn;
			//O sucessor para o antecessor.
			strcpy(vetorChaves[i].pk, vetorChaves[i-1].pk);
			vetorChaves[i].rrn = vetorChaves[i-1].rrn;
			//E o sucessor para o antecessor.
			strcpy(vetorChaves[i-1].pk, trocar.pk);
			vetorChaves[i-1].rrn = trocar.rrn;

			//Movimentamos o descendente para frente toda vez que a chave se move.
			desc[i+1] = desc[i];
		} else {
			break;
		}		
		i--;
	}		
	desc[i+1] = desc_dir_rrn;

	//Criando um novo nó que irá ser retornado.
	node_Btree_ip *noDireito = (node_Btree_ip*) criar_no('T');

	//Recebemos o indice da chave promovida que é exatamente a metade ou valor inteiro.
	indiceChavePromovida = (int)(ordem_ip/2);

	//Setamos os próximos como nulos.
	inicializaNoIp(noEsquerda);
		
	//Passamos as chaves e os descendentes para o nó a esquerda.
	for(i = 0; i < indiceChavePromovida; i ++){
		//Colocando os dado das Chave_ip no vetor.
		strcpy(noEsquerda->chave[i].pk, vetorChaves[i].pk);
		noEsquerda->chave[i].rrn = vetorChaves[i].rrn;
		//Passando os descendentes.
		noEsquerda->desc[i] = desc[i];
	}
	//Para colocar o último descendente.
	noEsquerda->desc[i] = desc[i];
	/* Como a quantidade de chaves é sempre uma a mais do que inserimos,
	passamos sempre o i para ter esse controle. */
	noEsquerda->num_chaves = i;
	

	for(i = 0, j = indiceChavePromovida+1; j < ordem_ip; i++, j++){
		//Passamos as demais pro outro vetor.
		strcpy(noDireito->chave[i].pk, vetorChaves[j].pk);
		noDireito->chave[i].rrn = vetorChaves[j].rrn;
		
		noDireito->desc[i] = desc[j];
	}
	//Para colocar o últo descendente.
	noDireito->desc[i] = desc[j];
	//Idem do nó a esquerda.
	noDireito->num_chaves = i;

	//Se for != -1, então nosso nó criado não é a folha.
	if(desc_dir_rrn != -1){
		noDireito->folha = 'F';
		noEsquerda->folha = 'F';
	}


	//Escrevemos os nós no arquivo IP.
	write_btree(noEsquerda, rrnesq, 'T');
	write_btree(noDireito, nregistrosip, 'T');
	retorno = nregistrosip;//Salvamos o retorno.
	nregistrosip+=1; //Incrementamos a quantidade de nós.
	
	//Copiamos a chave promovida para ser retornada acima e retornamos.
	strcpy(chave->pk, vetorChaves[indiceChavePromovida].pk);
	chave->rrn = vetorChaves[indiceChavePromovida].rrn;
	//Liberamos os nós.
	libera_no(noEsquerda, 'T');
	libera_no(noDireito, 'T');
	
	return retorno;
}

int insere_aux_ip(int noderrn, Chave_ip *chave){
	
	int filhoDireito = -1, i;

	//Estamos lendo o primeiro nó para fazer as comparações.
	node_Btree_ip *noIp = (node_Btree_ip*) criar_no('T');

	read_btree(noderrn,'T', noIp);
	
	//Se o nó for folha.
	if(noIp->folha == 'T'){
		//Se a chave servir no nó.
		if(noIp->num_chaves < (ordem_ip-1)){
			//Inserção ordenada no vetor. (meu próprio código :D )
			Chave_ip trocar;
			i = noIp->num_chaves;

			//Colocamos os valores em uma struct que será utilizada para trocar.
			strcpy(noIp->chave[i].pk, chave->pk);
			noIp->chave[i].rrn = chave->rrn;
			
			while(i){
				if(strcmp(noIp->chave[i-1].pk,noIp->chave[i].pk) > 0){
					strcpy(trocar.pk, noIp->chave[i].pk);
					trocar.rrn = noIp->chave[i].rrn;

					strcpy(noIp->chave[i].pk, noIp->chave[i-1].pk);
					noIp->chave[i].rrn = noIp->chave[i-1].rrn;

					strcpy(noIp->chave[i-1].pk, trocar.pk);
					noIp->chave[i-1].rrn = trocar.rrn;
				} else {
					break;
				}		
				i--;
			}
			noIp->num_chaves+=1;
			//Gravamos no arquivo IP.
			write_btree(noIp, noderrn, 'T');

			//Liberamos.
			libera_no(noIp,'T');
			//Deixamos chave == NULL para não ocorrer split em cima.
			chave->pk[0] = '\0';

			return -1;
		//Porém, se o nó for folha mas estiver cheio, precisamos fazer o split.	
		} else {
			
			libera_no(noIp, 'T');
			//O nó a direita é nulo porque o noderrrn é uma folha.
			return divide_no_ip(noderrn, chave, -1);
		}
	} else {
		//Para achar o rrn do próximo nó percorremos o vetor de chaves fazendo as comparações.
		for(i = 0; i < noIp->num_chaves; i++){
			//Se for maior que o último.
			if(i == (noIp->num_chaves-1) && strcmp(chave->pk,noIp->chave[i].pk) > 0){ 
				i+=1;
				break;
			}
			//Se for a comparação com a primeira pk.
			else if(i == 0) {
				if(strcmp(chave->pk,noIp->chave[i].pk) < 0) break;
				//Se for diferente da primeira comparação.
			} else if(i != 0){
				//Se estivermos nos intervalos!.
				if(strcmp(chave->pk,noIp->chave[i-1].pk) > 0 && 
				strcmp(chave->pk,noIp->chave[i].pk) < 0){
					break;
				}
				//Se for a última.
			}	
		} 

		filhoDireito = insere_aux_ip(noIp->desc[i], chave);
	
		//Porém, se ela for != nulo temos um split para tratar.	
		if(filhoDireito != -1){
			//Verificamos se o nó não está cheio.
			if(noIp->num_chaves < (ordem_ip-1)){
				//Se o nó não estiver cheio, vamos inserir a chave promovida nele.
				//Primeiramente, temos de passar as chaves para frente, pois
				//precisamos abrir espaço para a chave que irá ser inserida.
				int j = noIp->num_chaves; //FOi alterado.
				//Observe o seguinte, quando ocorre overflow em um desce i, o lugar que a chave
				//promovida será inserida será no indice i no vetor de chaves.
				//Então, passamos todas as chaves para frente incluindo o i para 
				//abrir espaço para a chave promovida.
				
				//O erro pode estar aqui.
				while(i <= j){
					//Passando as chaves para frente.
					strcpy(noIp->chave[j].pk, noIp->chave[j-1].pk);
					noIp->chave[j].rrn = noIp->chave[j-1].rrn;
					
					
					//Passando os descendentes para frente, observe que
					//precisamos levar em consideração que os descentes tem sempre um dado
					//no vetor a mais que as chaves.
					noIp->desc[j+1] = noIp->desc[j];
					j--;
				}
				//E por fim, colocando a chave promovida.
				strcpy(noIp->chave[i].pk, chave->pk);
				noIp->chave[i].rrn = chave->rrn;
				//E tbm, o novo descendente.
				noIp->desc[i+1] = filhoDireito;
				//Incrementamos a chave.
				noIp->num_chaves+=1;
				//Gravamos no arquivo IP.
				write_btree(noIp, noderrn, 'T');
				//Liberamos.
				libera_no(noIp,'T');
				//Deixamos chave == NULL para não ocorrer split em cima.
				chave->pk[0] = '\0';
				return -1;
			}
			//porém, se não temos espaço, precisaremos fazer um novo split.
			else {
				libera_no(noIp, 'T');
				//O nó a direita é nulo porque o noderrrn é uma folha.
				return divide_no_ip(noderrn, chave, filhoDireito);	
			}
		//Porém, se a chave for igual a null.
		} 
		libera_no(noIp, 'T');
		return -1;
	}
}

void insere_chave_ip(Indice *iprimary, Chave_ip *chave){
	int filhoDireita;

	//Se for o primeiro nó a ser inserido.	
	if(iprimary->raiz == -1){
		//Criamos um nó.
		node_Btree_ip *raiz =  (node_Btree_ip *)criar_no('T');
		//Passamos a chave para o primeoiro nó.
		strcpy(raiz->chave[0].pk, chave->pk);
		raiz->chave[0].rrn = chave->rrn;

		raiz->num_chaves+=1; //Incrementamos a quantidade de chaves.
		//indicamos quem é o nó raiz.
		iprimary->raiz = nregistrosip;
		
		//Gravamos o nó no arquivo IP.
		write_btree(raiz, nregistrosip, 'T');
		
		//Somamos +1 no arquivo da árvore IP e liberamos o nó.
		nregistrosip+=1;
		libera_no(raiz,'T');
	} else {

		filhoDireita = insere_aux_ip(iprimary->raiz, chave);
		
		if(filhoDireita != -1){
			//Criamos a nova raiz.
			node_Btree_ip *novaRaiz = (node_Btree_ip*) criar_no('T');
			//Como não é mais a primeiro nó, então seu nó não é folha.
			novaRaiz->folha = 'F';
			
			//Passamos a chave para o primeoiro nó.
			strcpy(novaRaiz->chave[0].pk, chave->pk);
			novaRaiz->chave[0].rrn = chave->rrn;
			//Incrementamos a quantidade de chaves.	
			novaRaiz->num_chaves+=1;

			//O que ocorre é que o retorno do split será sempre o filho a direita.
			novaRaiz->desc[0] = iprimary->raiz;
			novaRaiz->desc[1] = filhoDireita;

			//indicamos quem é o novo nó raiz.
			iprimary->raiz = nregistrosip;

			//Gravamos o nó no arquivo IP.
			write_btree(novaRaiz, nregistrosip, 'T');

			//Somamos +1 no arquivo da árvore IP e liberamos o nó.
			nregistrosip+=1;
			libera_no(novaRaiz,'T');
		}
	}
}


void inserir_registro_indices(Indice *iprimary, Indice *iride, Carona j){
	
/*Passos para inserir na árvore ip. */
	//Chave ip que irá ser inserida em algum nó.
	Chave_ip chaveip;
	//Colocando a chave e rrn do registro que se encontra no ARQUIVO.	
	strcpy(chaveip.pk, j.pk);
	chaveip.rrn = nregistros;
	//Chamando a função para inserir na árvore ip.
	insere_chave_ip(iprimary, &chaveip);

/* --------------------------------- */
//Passos para inserir na árvore is. 
	//Chave is que irá ser inserida em algum nó.
	Chave_is chaveis;
	char destino[TAM_STRING_INDICE-6-4];
	char string[TAM_STRING_INDICE];
	int i, ac;
	//Chave pk para ser passada para a função busca do is.
	for(ac = 0,i = strlen(j.trajeto)-1; j.trajeto[i] != '|' && i >= 0; i-=1, ac+=1);

	strncpy(destino,j.trajeto+strlen(j.trajeto) - ac, ac);
	destino[ac] = '\0';

	//Gerando a chave composta com destino+data+hora.
	gerarChaveIs(destino,j.data, j.hora, string);
	//Inserindo a chave pk e a composta no chaveis.
	strcpy(chaveis.pk, j.pk);
	strcpy(chaveis.string, string);
	
	//Inserindo a chave composta.
	insere_chave_is(iride, &chaveis);	
}

void cadastrar(Indice *iprimary, Indice *iride){

	Carona novo;
	char registro[TAM_REGISTRO+1];
	ler_entrada(registro, &novo);
	gerarChave(&novo);

	if(buscar_chave_ip(iprimary->raiz, novo.pk, 'F') == -1){
		gravarArquivo(registro, nregistros);
		//Precisamos retirar o novo.
		inserir_registro_indices(iprimary, iride, novo);
		nregistros+=1;
	} else {
		printf(ERRO_PK_REPETIDA,novo.pk);
	}
}

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
