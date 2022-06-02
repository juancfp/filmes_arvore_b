#define d 2
typedef struct Filme{
    char titulo[80];
    int ano;
    char diretor[50];
    char genero[30];
    int duracao;
} Filme;


typedef struct Pagina{
    long int pt_arquivo;        //aponta para si próprio

    int qnt_chaves; // qtd de chaves
    long int ponteiro_pai; // ponteiro para o no pai (multiplicar com sizeof(Pagina)); long int -> para função fseek pegar rendondo

    Filme* filmes[2*d];      //modificado
    long int ponteiros_filho[2*d+1]; // ponteiro que aponta para um array de ponteiros (multiplicar com sizeof(Pagina))
} Pagina;
typedef struct lista_enc{
    Filme* filme;
    struct lista_enc* prox;
}Lista;

Pagina* le(FILE* meta_arq, FILE* in, long int k);       //lê o arquivo na posição k
void salva(FILE* meta_arq, FILE* out, Pagina* page);    //salva a pagina na posição correta
int imprime_arquivo(FILE* meta_arq, FILE* in);          //imprime o arquivo inteiro
void imprime(Pagina* page, int j);                      //imprime apenas uma pagina (j é o valor da pagina)
Filme* Cria_Filme(int ano ,char* diretor,char* genero,char* titulo,int duracao);    //cria filme com ano, diretor, genero, titulo e duração
Pagina* CriaPagina();                                   //cria pagina em branco
int tamanho_Registro();                                 //tamanho do registro para ser salvo no arquivo
void libera(Pagina* page);                              //libera uma pagina inteira
long int final_arquivo(FILE* out);                      //fim do arquivo (para pt_arquivo de inserção de novo nó)
void imprime_filme(Filme* filme);
void nova_raiz(FILE* meta_arq, Pagina* raiz);
Pagina* le_raiz(FILE* meta_arq, FILE* in);
int compara(Filme* f1, Filme* f2);
int LerEntraIniciais(FILE* arq,char titulo[80],int* ano,char diretor[50],char genero[30],int* duracao);
Filme* altera_infos_filme(char* titulo, int ano, char* genero, char* diretor, int duracao, FILE* meta, FILE* in);
Filme* busca_filme(char* titulo, int ano, FILE* meta, FILE* in);
Pagina* busca(Filme* filme, FILE* meta, FILE *arvore, int *j, int *encontrou);
int organizar(Filme** filmes, long int *pt_filmes, int * pt_count, Pagina* esquerda, Pagina* direita, Pagina* pai, int k);
void redistribuir(FILE* meta, FILE* arvore, Pagina* esquerda, Pagina* direita, Pagina* pai, int k, int dir);
int excluir(FILE* meta, FILE* arvore, Filme* filme);
void ordena(Pagina* page, Filme* filme,int* filhoD);
Filme* organiza_pega_meio(Pagina* page,Filme* filme,int* filhoD);
void dividePage(Pagina* page,Pagina* novaPage);
Pagina* insere(FILE* meta, FILE* arvore, Filme* filme_novo);
Pagina* insere_filme(char* titulo, int ano, char* diretor, char* genero, int duracao, FILE* meta, FILE* in);
void remove_filme(char* titulo, int ano, FILE* meta, FILE* in);

int busca_genero(FILE* meta, FILE* arvore, Pagina* raiz, char* genero, Lista* lista);
int busca_diretor(FILE* meta, FILE* arvore, Pagina* raiz, char* diretor);
int eh_genero(Filme* pg, char* genero, FILE* meta, FILE* arvore, Lista* lista);
Lista* cria_elo(Filme* filme);
Lista* add(Lista*lista, Filme* filme);


Lista* cria_elo(Filme* filme){
    Lista* novo_elo;
    novo_elo = (Lista*) malloc(sizeof(Lista));
    novo_elo->filme = filme;
    novo_elo->prox = NULL;
    return novo_elo;
}

Lista* add(Lista*lista, Filme* filme){
    if(lista == NULL){
        lista = cria_elo(filme);
        return lista;
    }
    Lista* lista_atual = lista;
    while(lista_atual->prox != NULL){
        lista_atual = lista_atual->prox;

    }
    lista_atual->prox = cria_elo(filme);
    return lista_atual;
}

int eh_genero(Filme* pg, char* genero, FILE* meta, FILE* arvore, Lista* lista){
    int i;

    
        if( strcmp(genero, pg->genero) == 0 ){
            add(lista, pg);
    
    }
}

int busca_genero(FILE* meta, FILE* arvore, Pagina* raiz, char* genero, Lista* lista){
    if(raiz->ponteiros_filho[0] == -1){
        int i;
        for(i = 0; i < raiz->qnt_chaves; i++){
            eh_genero(raiz->filmes[i], genero, meta, arvore, lista);
        }

        libera(raiz);
        return 1;
    }
    else{
        int i;
        busca_genero(meta, arvore, le(meta, arvore, raiz->ponteiros_filho[0]), genero, lista);
        for(i = 0; i < raiz->qnt_chaves; i++){
            busca_genero(meta, arvore, le(meta, arvore, raiz->ponteiros_filho[i+1]), genero, lista);
            eh_genero(raiz->filmes[i], genero, meta, arvore, lista);
        }
        libera(raiz);
        return 1;
    }
    return 1;
}

int remove_genero(Pagina* pg, Pagina* raiz, char* genero, FILE* meta, FILE* arvore, Lista* lista){
    int qtd_removidos = 0;

    while(lista->prox != NULL){
        excluir(meta, arvore, lista->filme);
        qtd_removidos++;
    }
    return qtd_removidos;
}

/*Pagina* busca_diretor(Pagina* pg, char* diretor, FILE* meta, FILE* in){

    //verifica se os filmes sao do diretor x
    eh_diretor(pg, diretor);

    // verificando se sou folha
    if(pg->ponteiros_filho[0] == -1) return pg;

    // olhando para os filhos
    Pagina* pagina_atual = pg;
    int i;
    for(i=0; i<pg->qnt_chaves+1; i++){
        pagina_atual = busca_diretor(le(pagina_atual->ponteiros_filho[i], meta, in), diretor, meta, in);
    }

    return pg;
}*/


Pagina* le_raiz(FILE* meta_arq, FILE* in) {
	long int pt_raiz;
	Pagina* page;

	fseek(meta_arq, 0, SEEK_SET);
	fread(&pt_raiz, 1, sizeof(long int), meta_arq);
	fflush(meta_arq);

	page = le(meta_arq, in, pt_raiz);

	return page;
}
void nova_raiz(FILE* meta_arq, Pagina* raiz) {

    fseek(meta_arq, 0, SEEK_SET);
    fwrite(&raiz->pt_arquivo, 1, sizeof(long int), meta_arq);
    fflush(meta_arq);
}
int compara(Filme* f1, Filme* f2) {
	char str1[100], str2[100];
 
	snprintf (str1, 100, "%s%d", f1->titulo, f1->ano);
	snprintf (str2, 100, "%s%d", f2->titulo, f2->ano);

	int comp = strcmp(str1, str2);

	if(comp > 0) return 1;
	else if(comp < 0) return -1;

	return 0;
}

int LerEntraIniciais(FILE* arq,char titulo[80],int* ano,char diretor[50],char genero[30],int* duracao){
    char c = fgetc(arq);
    if(c==EOF){
        printf("EOF");
        return 0;
    }


    int i=0;
    for(i=0;c!='/';i++ ){
        titulo[i]=c;

        c = fgetc(arq);

    }
    titulo[i]='\0';


    c = fgetc(arq);
    char anoo[20];
    for(i=0;c!='/';i++ ){
        anoo[i]=c;
        c = fgetc(arq);
    }
    anoo[i]='\0';
    *ano=atoi(anoo);


    c = fgetc(arq);
    for(i=0;c!='/';i++ ){
        diretor[i]=c;

        c = fgetc(arq);

    }
    diretor[i]='\0';



    c = fgetc(arq);
    for(i=0;c!='/';i++ ){
        genero[i]=c;

        c = fgetc(arq);

    }
    genero[i]='\0';


    c = fgetc(arq);
     for(i=0;c!='\n'&&  c!=EOF;i++ ){

        anoo[i]=c;
        c = fgetc(arq);
    }
    anoo[i]='\0';

    *duracao=atoi(anoo);

    if(c  == EOF){
        printf("EOF");
        return 0;
    }
    return 1;
}

int tamanho_Registro(){
    int tamanho =  sizeof(long int) +sizeof(int) +sizeof(long int)+(2*d+1)*sizeof(long int);
    int tamanho1= (2*d) * (sizeof(char) * (80 + 50 + 30) + 2*sizeof(int));
    return tamanho + tamanho1;
}

long int final_arquivo(FILE* out) {
	fseek(out, 0, SEEK_END);
	return (long int)((float) ftell(out) / tamanho_Registro() );
}

Pagina* le(FILE* meta_arq, FILE* in, long int k) {

	int i=0;
    fseek(in, k * tamanho_Registro(), SEEK_SET);

	Pagina* page = (Pagina*) malloc(sizeof(Pagina));

	//fseek(in, k * ((long) sizeof(Pagina)), SEEK_SET);

	//parâmetros
	fread(&page->pt_arquivo, sizeof(long int), 1, in);
	fread(&page->qnt_chaves, sizeof(int), 1, in);
	fread(&page->ponteiro_pai, sizeof(long int), 1, in);
    fread(page->ponteiros_filho, sizeof(long int), (2*d+1), in);
    //printf("%d \n",page->qnt_chaves);
	//filmes

	for(i = 0; i < 2 * d; i++) {

            page->filmes[i] = (Filme*) malloc(sizeof(Filme));

            //todos são strings (já são ponteiros), exceto o ano
            //fread(page->filmes[i]->chave, sizeof(char), 85, in);

            fread(page->filmes[i]->titulo, sizeof(char), 80, in);
            fread(&page->filmes[i]->ano, sizeof(int), 1, in);
            fread(page->filmes[i]->diretor, sizeof(char), 50, in);
            fread(page->filmes[i]->genero, sizeof(char), 30, in);
            fread(&page->filmes[i]->duracao, sizeof(int), 1, in);

	}

	//ponteiros para filhos (string)
	fflush(in);
	return page;
}
void salva(FILE* meta_arq, FILE* out, Pagina* page) {

    int i=0;
	long int onde = page->pt_arquivo * tamanho_Registro();
    fseek(out, onde, SEEK_SET);
	//parâmetros
	fwrite(&page->pt_arquivo, 1, sizeof(long int), out);
	fwrite(&page->qnt_chaves, 1, sizeof(int), out);
	fwrite(&page->ponteiro_pai, 1, sizeof(long int), out);
	fwrite(page->ponteiros_filho, (2*d+1), sizeof(long int), out);
    Filme* filme;
    char a ='\0';
    filme=Cria_Filme(-1,&a,&a,&a,-1);
	//filmes
	for(i = 0; i < 2*d; i++) {
        if(i < page->qnt_chaves ){


		//todos são strings (já são ponteiros), exceto o ano
            //fwrite(page->filmes[i]->chave, 85, sizeof(char), out);
            fwrite(page->filmes[i]->titulo, 80, sizeof(char), out);
            fwrite(&page->filmes[i]->ano, 1, sizeof(int), out);
            fwrite(page->filmes[i]->diretor, 50, sizeof(char), out);
            fwrite(page->filmes[i]->genero, 30, sizeof(char), out);
            fwrite(&page->filmes[i]->duracao, 1, sizeof(int), out);

        }
        else{
            //fwrite(filme->chave, 85, sizeof(char), out);
            fwrite(filme->titulo, 80, sizeof(char), out);
            fwrite(&filme->ano, 1, sizeof(int), out);
            fwrite(filme->diretor, 50, sizeof(char), out);
            fwrite(filme->genero, 30, sizeof(char), out);
            fwrite(&filme->duracao, 1, sizeof(int), out);


        }
        
	}

	//ponteiros para filhos (string)
	fflush(out);
}

void libera(Pagina* page) {
	int i;

	for(i = 0; i < page->qnt_chaves; i++) {
		free(page->filmes[i]);
	}
	free(page);
}

//imprime tudo do arquivo (para testes)
int imprime_arquivo(FILE* meta_arq, FILE* in) {
	int i=0;
	int a = 0;
	Pagina* raiz;
	Pagina* page;

	raiz = le_raiz(meta_arq, in);

	if(raiz->ponteiros_filho[0] == -1) {
		printf("--------- NIVEL %d ---------------\n\n", a++);
		imprime(raiz, 0);
		return -1;
	}

	int choice;

	printf("--------- NIVEL %d ---------------\n\n", a++);
	imprime(raiz, 0);
	printf("Para qual ponteiro deseja ir? [0, %d]\n", raiz->qnt_chaves);
	scanf("%d", &choice);
	if(choice > raiz->qnt_chaves){
		printf("Numero inválido (ERRO)\n");
		return -1;
	}
	page = le(meta_arq, in, raiz->ponteiros_filho[choice]);
	
	//imprime antes de ver se é folha
	printf("--------- NIVEL %d ---------------\n\n", a++);
	imprime(page, 0);
	do{
		
		
		if(page->ponteiros_filho[0] != -1) {
			printf("Para qual ponteiro deseja ir? [0, %d]\n", page->qnt_chaves);
			scanf("%d", &choice);
			if(choice > page->qnt_chaves){
				printf("Numero inválido\n");
				continue;
			}
			page = le(meta_arq, in, page->ponteiros_filho[choice]);
			printf("--------- NIVEL %d ---------------\n\n", a);
			a++;
			imprime(page, 0);
		} else
			break;
		
		
		
	}while(1);
}
void imprime_apos_exclusao(Pagina* esquerda, Pagina* direita, Pagina* pai) {

	printf("------------- PAI --------------\n");
	imprime(pai, 0);
	printf("------------- ESQUERDA --------------\n");
	imprime(esquerda, 0);
	printf("------------- DIREITA --------------\n");
	imprime(direita, 0);
}
//imprime uma pagina apenas
void imprime(Pagina* page, int j) {
	int i = 0;


	printf("(pt_arq=%ld, qnt_ch=%d, pg=%d ):", page->pt_arquivo, page->qnt_chaves, j);
	printf("{[%ld]", page->ponteiros_filho[0]);
	for(i = 0; i < page->qnt_chaves; i++){
		printf("[%s, %d | %ld]", page->filmes[i]->titulo, page->filmes[i]->ano, page->ponteiros_filho[i+1]);
	}
	printf("}\n");
}

void imprime_filme(Filme* filme){
	printf("[%s, %d, %s, %s]\n", filme->titulo, filme->ano, filme->genero, filme->diretor);
}
//cria um filme a partir das strings que estão nos parâmetros
Filme* Cria_Filme(int ano ,char* diretor,char* genero,char* titulo,int duracao){
    Filme* filme;
    filme=(Filme*) malloc(sizeof(Filme));
    //filme->chave[0]='\0';
    filme->ano = ano;
    //filme->diretor[0]='\0';
    //filme->genero[0]='\0';
    //filme->titulo[0]='\0';

    strcpy(filme->titulo,titulo);
    strcpy(filme->diretor,diretor);
    
    
    strcpy(filme->genero,genero);
    filme->duracao = duracao;

    return filme;
}
//cria uma pagina em branco (sem filmes)
Pagina* CriaPagina(){

    Pagina* pagina;
    pagina= (Pagina*) malloc(sizeof(Pagina));
    pagina->pt_arquivo=0;
    pagina->qnt_chaves=0;
    pagina->ponteiro_pai=-1;
    for(int i =0 ;i<2*d+1;i++){
        pagina->ponteiros_filho[i]=-1;
    }
    return pagina;
}

Filme* altera_infos_filme(char* titulo, int ano, char* genero, char* diretor, int duracao, FILE* meta, FILE* in){
    Filme* filme = busca_filme(titulo, ano, meta, in);
    if(!filme){
        printf("Filme nao encontrado");
        return NULL;
    }

    strcpy(filme->genero,genero);
    strcpy(filme->diretor,diretor);
    filme->duracao = duracao;

    imprime_filme(filme);

    return filme;
}
Filme* busca_filme(char* titulo, int ano, FILE* meta, FILE* in){
    Filme* filme = Cria_Filme(ano, "diretor", "genero", titulo, 10);
    Pagina* pg = busca(filme, meta, in, 0, 0);
    int i;
    for(i=0; i < pg->qnt_chaves; i++){
        if(compara(filme, pg->filmes[i]) == 0){
            imprime_filme(pg->filmes[i]);printf("\n");
            return pg->filmes[i];
        }
    }

    free(filme);
    free(pg);
    return NULL;
}
Pagina* insere_filme(char* titulo, int ano, char* diretor, char* genero, int duracao, FILE* meta, FILE* in){
    Filme* filme_novo = Cria_Filme(ano, diretor, genero, titulo, duracao);
    return insere(meta, in, filme_novo); 
}
void remove_filme(char* titulo, int ano, FILE* meta, FILE* in){
    Filme* filme = Cria_Filme(ano, "diretor", "genero", titulo, 10);
    excluir(meta, in, filme);
}
Pagina* busca(Filme* filme, FILE* meta, FILE *arvore, int *j, int *encontrou){
    Pagina *raiz;

    raiz = le_raiz(meta, arvore);

    int i=0;

    Pagina* pagina = raiz;
    if(raiz->qnt_chaves == 0){
        return raiz;
    }
    while(1)
    {   
        if(compara(pagina->filmes[0], filme) <= 0){ //se n ta no primeiro ponteiro (menor que o primeiro nó)
            i=pagina->qnt_chaves-1;
            while(i > 0 && compara(pagina->filmes[i], filme) > 0){      //qnd filme buscado for maior ou igual ao filme q está sendo percorrido, sai)
                i--;
            }
            if(i == pagina->qnt_chaves){
                i--;
            }
            if(i == -1){
                i++;
            }
        } else  //se está no primeiro nó
        {


            if(pagina->ponteiros_filho[0] == -1){
                *encontrou = 0;
                *j = 0;
                return pagina;
            }
            else
            {
                long int pt = pagina->ponteiros_filho[0];
                libera(pagina);
                pagina = le(meta, arvore, pt);

                continue;
            }
        }
        if(compara(filme, pagina->filmes[i]) == 0)
        {
            *encontrou = 1;
            *j = i;
            return pagina;
        }
        else
        {
            if(pagina->ponteiros_filho[0] == -1){
                *encontrou = 0;
                *j = i;
                return pagina;
            }
            else
            {
                long int pt = pagina->ponteiros_filho[i+1];
                libera(pagina);
                pagina = le(meta, arvore, pt);
            }
        }         
    
    }
        
    

    return NULL;
}
int organizar(Filme** filmes, long int *pt_filmes, int * pt_count, Pagina* esquerda, Pagina* direita, Pagina* pai, int k) {
    int count = 0, i = 0;
    *pt_count = 0;

    pt_filmes[*pt_count] = esquerda->ponteiros_filho[0];
    *pt_count = *pt_count+1;

    for(i = 0; i < esquerda->qnt_chaves; i++) {
        filmes[count] = esquerda->filmes[i];
        pt_filmes[*pt_count] = esquerda->ponteiros_filho[i+1];
        // imprime_filme(filmes[count], pt_filmes[pt_count]);
        count++; *pt_count = *pt_count+1;
    }
    filmes[count] = pai->filmes[k];
    pt_filmes[*pt_count] = direita->ponteiros_filho[0];
    count++;*pt_count = *pt_count+1;

    for(i = 0; i < direita->qnt_chaves; i++) {
        filmes[count++] = direita->filmes[i];
        pt_filmes[*pt_count] = direita->ponteiros_filho[i+1];
        *pt_count = *pt_count+1;
    }

    return count;

}
void redistribuir(FILE* meta, FILE* arvore, Pagina* esquerda, Pagina* direita, Pagina* pai, int k, int dir) {

    int i=0;
    Filme* filmes[3*d+ 1];
    long int pt_filmes[3*d + 2];
    int count = 0;
    int pt_count = 0;

    count = organizar(filmes, pt_filmes, &pt_count, esquerda, direita, pai, k);


    //colocar na ordem - esquerda
    if(dir == 0) {
        esquerda->qnt_chaves = d;
        esquerda->ponteiros_filho[0] = pt_filmes[0];
        for(i = 0; i < d; i++) {
            esquerda->filmes[i] = filmes[i];
            esquerda->ponteiros_filho[i+1] = pt_filmes[i+1];

        }

        //pai - d + 1
        pai->filmes[k] = filmes[d];
        // printf("\n");imprime_filme(filmes[d], 90);printf("\n");
        direita->ponteiros_filho[0] = pt_filmes[d+1];

        //colocar na ordem - direita
        direita->qnt_chaves = count - d - 1;
        for(i = 0; i < direita->qnt_chaves ; i++) {
            direita->filmes[i] = filmes[i + d + 1];
            direita->ponteiros_filho[i+1] = pt_filmes[i + d + 2];
        }   
    } else
    {
        direita->qnt_chaves = d;
        direita->ponteiros_filho[0] = pt_filmes[0];
        for(i = 0; i < d; i++) {
            direita->filmes[i] = filmes[i];
            direita->ponteiros_filho[i+1] = pt_filmes[i+1];

        }

        //pai - d + 1

        pai->filmes[k] = filmes[d];
        // printf("\n");imprime_filme(filmes[d], 90);printf("\n");
        direita->ponteiros_filho[0] = pt_filmes[d+1];

        //colocar na ordem - direita
        esquerda->qnt_chaves = count - d - 1;
        for(i = 0; i < direita->qnt_chaves ; i++) {
            esquerda->filmes[i] = filmes[i + d + 1];
            esquerda->ponteiros_filho[i+1+1] = pt_filmes[i + d + 2];
        }
    }

    
    salva(meta, arvore, direita);
    salva(meta, arvore, esquerda);
    salva(meta, arvore, pai);
}

int excluir(FILE* meta, FILE* arvore, Filme* filme) {

    int indice, encontrou;
    Pagina* page = busca(filme, meta, arvore, &indice, &encontrou), *pai, *descer;


    if(!encontrou) {
        return -1;      //não encontrou
    }
    // imprime(page, indice);
    //encontrou - remove

    if(page->ponteiros_filho[0] == -1){
        descer = page;
        int j = indice;

        while( j < descer->qnt_chaves){

            descer->filmes[j] = descer->filmes[j+1];
            j++;
        }
        descer->qnt_chaves--;
        salva(meta, arvore, descer);

        if(descer->ponteiro_pai == -1){
            return 1;
        }

        pai = le(meta, arvore, descer->ponteiro_pai);
        //achar pai
        indice=pai->qnt_chaves-1;
        while(indice > 0 && compara(pai->filmes[indice], descer->filmes[0]) > 0){      //qnd filme buscado for maior ou igual ao filme q está sendo percorrido, sai)
            indice--;
        }
        
    }else{
        pai = page;
        descer = le(meta, arvore, page->ponteiros_filho[indice]);

        while(descer->ponteiros_filho[0] != -1) {
            libera(pai);
            pai = descer;
            descer = le(meta, arvore, descer->ponteiros_filho[descer->qnt_chaves]);     //qnt_chaves já tem o +1 (de n ser quantidade, e n índice)
        }
        
        page->filmes[indice] = descer->filmes[descer->qnt_chaves-1];
        descer->qnt_chaves--;   //removido (ultimo)
        salva(meta, arvore, descer);
        salva(meta, arvore, page);
        
        if(descer->qnt_chaves >= d){
            return 1;
        }   
        //achar o indice p fazer redistribuição ou concatenação
        indice=pai->qnt_chaves-1;
        while(indice > 0 && compara(pai->filmes[indice], descer->filmes[0]) > 0){      //qnd filme buscado for maior ou igual ao filme q está sendo percorrido, sai)
            indice--;
        }
    }

    while(1) {
        Pagina* direita, *esquerda;
        int qual=1; //0 == redistribuir; 1 == concatenar;
        int dir=0; //direita ou esquerda
        if(indice == 0 && pai->ponteiro_pai != -1){ //extrema esquerda
            printf("(1)\n");
            esquerda = descer;
            direita = le(meta, arvore, pai->ponteiros_filho[1]);
            if(esquerda->qnt_chaves >= d || direita->qnt_chaves >= d){
                if(esquerda->qnt_chaves >= d){
                    dir = 1;
                }else dir = 2;
                qual = 0;
            }
        } else if(indice == pai->qnt_chaves-1 && pai->ponteiro_pai != -1){  //extrema direita
            printf("(2)\n");
            direita = descer;
            esquerda = le(meta, arvore, pai->ponteiros_filho[1]);

            if(esquerda->qnt_chaves >= d || direita->qnt_chaves >= d){
                if(esquerda->qnt_chaves >= d){
                    dir = 1;
                }else dir = 2;
                qual = 0;
            }
        }
        else if(indice == 0 && pai->ponteiro_pai == -1){    //pai com apenas uma chave
            printf("(3)\n");

            if(compara(descer->filmes[descer->qnt_chaves-1], pai->filmes[0]) < 0){
                esquerda = descer;
                direita = le(meta, arvore, pai->ponteiros_filho[1]);
            }
            else{
                direita = descer;
                esquerda = le(meta, arvore, pai->ponteiros_filho[0]);   
            }
            if(esquerda->qnt_chaves >= d || direita->qnt_chaves >= d){
                if(esquerda->qnt_chaves >= d){
                    dir = 1;
                }else dir = 2;
                qual = 0;
            }
        }
        else{       //no meio
            printf("(4)\n");
            //decidir entre redistribuir ou concatenar
            esquerda = le(meta, arvore, pai->ponteiros_filho[indice]);
            direita = le(meta, arvore, pai->ponteiros_filho[indice+1]);

            if(direita->qnt_chaves >= d){
                // libera(esquerda);
                dir=0;
                esquerda = descer;
                qual = 0;
            } else if(esquerda->qnt_chaves >= d){
                // libera(direita);
                dir = 1;
                direita = descer;
                qual = 0;
            }
            
        }

        //com os ponteiros já certos
        // pai ok; direita ok; esquerda ok;

        if(qual == 0) {     //redistribuir

            
            redistribuir(meta, arvore, esquerda, direita, pai, indice, dir);
            
            libera(esquerda);
            libera(direita);
            libera(pai);
            return 1;
        }
        else {
            Filme* filmes[3*d+1];
            long int pt_filmes[3*d+2];
            int pt_count;
            int count = organizar(filmes, pt_filmes,&pt_count, esquerda, direita, pai, indice);     //qnt de nós salvos

            //dar shift no pai
            int j = indice;
            while(j < pai->qnt_chaves){
                pai->filmes[j] = pai->filmes[j+1];
                pai->ponteiros_filho[j+1] = pai->ponteiros_filho[j+2];
                j++;
            }
            pai->qnt_chaves--;

            //coloca tudo no da esquerda
            for(j = 0; j < count; j++) {

                esquerda->filmes[j] = Cria_Filme(filmes[j]->ano, filmes[j]->diretor, filmes[j]->genero, filmes[j]->titulo, filmes[j]->duracao);
                esquerda->ponteiros_filho[j] = pt_filmes[j];
            }
            
            esquerda->ponteiros_filho[j] = pt_filmes[j];
            esquerda->qnt_chaves = count;

            salva(meta, arvore, direita);
            salva(meta, arvore, esquerda);
            salva(meta, arvore, pai);

            if(pai->ponteiro_pai == -1 && pai->qnt_chaves == 0){
                nova_raiz(meta, esquerda);
                libera(esquerda);
                return 1;
            }
            if(pai->qnt_chaves < d) {
                descer = pai;
                pai = le(meta, arvore, descer->ponteiro_pai);

                //achar pai
                indice=pai->qnt_chaves-1;
                while(indice > 0 && compara(pai->filmes[indice], descer->filmes[0]) > 0){      //qnd filme buscado for maior ou igual ao filme q está sendo percorrido, sai)
                    indice--;
                }

            } else {
                libera(pai);
                return 1;
            }
            
        }
    }

}

int busca_diretor(FILE* meta, FILE* arvore, Pagina* raiz, char* diretor){
    if(raiz->ponteiros_filho[0] == -1){
        int i;
        for(i = 0; i < raiz->qnt_chaves; i++){
            if(strcmp(raiz->filmes[i]->diretor, diretor) == 0){
                imprime_filme(raiz->filmes[i]);     //imprime_filme tem 2 parametros, o segundo pode ser um inteiro qqr
            }

        }
        libera(raiz);
        return 1;
    }
    else{
        int i;
        busca_diretor(meta, arvore,le(meta, arvore, raiz->ponteiros_filho[0]), diretor);
        for(i = 0; i < raiz->qnt_chaves; i++){
            busca_diretor(meta, arvore, le(meta, arvore, raiz->ponteiros_filho[i+1]), diretor);
            if(strcmp(raiz->filmes[i]->diretor, diretor) == 0){
                imprime_filme(raiz->filmes[i]);
            }
        }

        libera(raiz);
        return 1;
    }
    return 1;
}   

Pagina* insere(FILE* meta, FILE* arvore, Filme* filme_novo) {
    int posicao;
    static int value = 0;
    Pagina* nova_page;
    value++;printf("qnt_adicionados=%d\n", value);
    int encontrou;
    int filhoD = -1;
    Pagina* no = busca(filme_novo, meta, arvore, &posicao, &encontrou);


    /*printf("Titulo :::::::::%s\n",filme_novo->titulo);
     printf("-------------------------------------------\n");
            for(int i = 0 ; i<no->qnt_chaves;i++){
                printf("%s \n",no->filmes[i]->titulo );
            }
    printf("-------------------------------------------\n");*/
    if(no->qnt_chaves == 0){    //raiz vazia
        // printf("(1)\n");
        no->filmes[0] = filme_novo;
        no->qnt_chaves++;
        // imprime(no, 200);
        salva(meta, arvore, no);
        return no;
    }
    if( encontrou ){
        // printf("(2)\n");
        return no;
    }
    while(1){
        // printf("(3)\n");
        if(no->qnt_chaves< d*2){
            // printf("(4)\n");

            //raiz folha precisa particionar

            ordena(no,filme_novo,&filhoD);

            salva(meta,arvore,no);

            return no;
        }

        else {
            // printf("(5)\n");

            filme_novo = organiza_pega_meio(no,filme_novo,&filhoD);
            nova_page=CriaPagina();
            dividePage(no,nova_page);
            nova_page->ponteiros_filho[0]=filhoD;
            nova_page->pt_arquivo=final_arquivo(arvore);
            nova_page->ponteiro_pai=no->ponteiro_pai;
            salva(meta,arvore,nova_page);
            salva(meta,arvore,no);
            filhoD=nova_page->pt_arquivo;

            if(no->ponteiro_pai==-1){
                // printf("(6)\n");
                Pagina* pai;
                pai=CriaPagina();
                pai->filmes[0]=filme_novo;
                pai->qnt_chaves=pai->qnt_chaves+1;
                pai->ponteiros_filho[0]=no->pt_arquivo;
                pai->ponteiros_filho[1]=filhoD;
                pai->ponteiro_pai=-1;
                pai->pt_arquivo = final_arquivo(arvore);
                salva(meta,arvore,pai);
                nova_raiz(meta, pai);
                no->ponteiro_pai=pai->pt_arquivo;
                nova_page->ponteiro_pai=pai->pt_arquivo;
                salva(meta,arvore,nova_page);
                salva(meta,arvore,no);
                // imprime(pai, 900);
                // imprime(no, 900);
                // imprime(nova_page, 900);
                libera(nova_page);
                libera(no);
                
                return pai;
            }
            libera(no);
            no=le(meta,arvore,nova_page->ponteiro_pai);
            libera(nova_page);
        }
    }
    return no;
}

// funções auxiliares
    void ordena(Pagina* page, Filme* filme,int* filhoD){
        Filme* ajuda;
        int iAjuda;

        for(int i = 0 ; i < page->qnt_chaves;i++){

            if(compara(page->filmes[i],filme)>0){

                ajuda=page->filmes[i];
                page->filmes[i]=filme;
                filme = ajuda;
                iAjuda= page->ponteiros_filho[i+1];
                page->ponteiros_filho[i+1]=*filhoD;
                *filhoD=iAjuda;
            }
        }

        page->filmes[page->qnt_chaves]=filme;
        page->ponteiros_filho[page->qnt_chaves+1]=*filhoD;
        page->qnt_chaves=page->qnt_chaves+1;
    }

    Filme* organiza_pega_meio(Pagina* page,Filme* filme,int* filhoD){
        Filme* ajuda;
        int iAjuda;




         for(int i = 0 ; i < d ; i++){

            if(compara(page->filmes[i],filme)>0){
                ajuda=page->filmes[i];
                page->filmes[i]=filme;
                filme = ajuda;
                iAjuda= page->ponteiros_filho[i+1];
                page->ponteiros_filho[i+1]=*filhoD;
                *filhoD=iAjuda;
            }
        }





        for(int i = (2*d)-1 ; i > d-1;i--){

            if(compara(page->filmes[i],filme)<0){
                ajuda=page->filmes[i];
                page->filmes[i]=filme;
                filme = ajuda;
                iAjuda= page->ponteiros_filho[i+1];
                page->ponteiros_filho[i+1]=*filhoD;
                *filhoD=iAjuda;
            }
        }



        return filme;
    }
    void dividePage(Pagina* page,Pagina* novaPage){
        for(int i =d ;i<2*d;i++){
            novaPage->filmes[i-d]=page->filmes[i];
            page->filmes[i]=NULL;
            novaPage->ponteiros_filho[i-d+1]=page->ponteiros_filho[i+1];
            page->ponteiros_filho[i+1]=-1;
        }
        page->qnt_chaves=d;
        novaPage->qnt_chaves=d;
    }
