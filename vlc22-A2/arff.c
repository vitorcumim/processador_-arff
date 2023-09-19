#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arff.h"

//imprime os atributos do vetor infos de forma ordenada
void exibe_atributos(atributo *infos, int tamanho){
  if (infos == NULL){
        printf("O arquivo ARFF fornecido é inválido!\n");
        exit(0);
    }

    printf("==== ATRIBUTOS DO ARQUIVO ====\n");
    for(int i = 0; i < tamanho; i++){
        printf("-> Atributo #%d\n", i+1);
        printf("Rótulo: %s\n", infos[i].rotulo);
        printf("Tipo: %s\n", infos[i].tipo);
        if (infos[i].categorias) 
            for (int j = 0; j < infos[i].cat_tam; j++) {
                if(infos[i].categorias[j]!=NULL)
                  printf("%d)%s\n",j,infos[i].categorias[j]);
            }
        if (i < tamanho-1) printf("------------------------------\n");
    }
    printf("===============================\n");
}

int conta_atributos(FILE *arff){
  int i=0;
  char linha[1024],*attribute;

  while(!feof(arff)){
    fscanf(arff,"%s",linha);
    attribute=strtok(linha," ");
    if((strcmp(attribute,"@attribute"))==0){
      i++;
    }
  }
  rewind(arff);
  return i;
}

//conta as virgulas de uma categoria para saber seu tamanho
int conta_virgula(char *str){
  int virgulas=0,i=0;
  while(str[i]!='\0'){
    if(str[i]==',')
      virgulas++;
    i++;
  }
  return virgulas;
}

//processa a string categorias e a distribui dentro do vetor v
void processa_categorias(char **v,char *categorias,int cat_tam){
  char *token;
  int i=0;

  token=strtok(categorias,",");
  for(int k=0;k<strlen(token);k++)
    token[k]=token[k+1];
  v[i]=strdup(token);
  while((token=strtok(NULL,","))!=0){
    i++;
    if(i==cat_tam-1){
      token[strlen(token)-1]='\0';
    }
    v[i]=strdup(token);
  }
}

//processa o header de um arquivo arff e retorna os seus atributos em um vetor
atributo* processa_atributos(FILE *arff){
  atributo *v;
    char linha[1025],aux[1025],*attribute[3]={NULL,NULL,NULL};
    int i=0,virgulas=0;
    int num_atributos=conta_atributos(arff);
    
    v=malloc(sizeof(atributo)*num_atributos);
    if(v==NULL){
      printf("erro na alocação");
      return NULL;
    }
    fscanf(arff,"%1025[^\n]",linha);
    fgetc(arff);
    while(!feof(arff)){
      strcpy(aux,linha);
      attribute[0]=strtok(aux," ");
         
      if((strcmp(attribute[0],"@data"))==0){
        return v;
      }
 
      if((strcmp(attribute[0],"@attribute"))!=0){
        printf("atributo %d iniciado incorretamente\n",i);
        return NULL;
      }

      for(int j=0;j<2;j++){
        attribute[j+1]=strtok(NULL," ");
      }

      if(attribute[1]!=NULL){
        v[i].rotulo=strdup(attribute[1]);
      }
      else{
        printf("atributos com parametros incorretos\n");
        return NULL;
      }

      if(attribute[2]!=NULL){
        if((strcmp(attribute[2],"numeric")!=0) && ((strcmp(attribute[2],"string")!=0))){
          virgulas=conta_virgula(attribute[2]);
          v[i].categorias=malloc((virgulas+1) * sizeof(char*));
          v[i].cat_tam = virgulas+1;
          processa_categorias(v[i].categorias,attribute[2],v[i].cat_tam);
          v[i].tipo="categoric";
        }
        else{  
          v[i].categorias=NULL;
          v[i].cat_tam=0;
          v[i].tipo=strdup(attribute[2]);
        }
      }
      else{
        printf("atributos com parametros incorretos\n");
        return NULL;
      }

      i++;
      while(fscanf(arff,"%1025[^\n]",linha)== 0){
        fgetc(arff);
      }
    }
    if(strcmp(linha,"@data")==0)
      return v;


    return NULL;
}

void libera_atributos(atributo *v, int num_atributos) {
    for (int i = 0; i < num_atributos; i++) {
        free(v[i].rotulo);
        if(strcmp(v[i].tipo,"categoric")!=0)
          free(v[i].tipo);
        else
          for(int j=0;j<v[i].cat_tam;j++)
            if(v[i].categorias[j]!=NULL)
              free(v[i].categorias[j]);
        free(v[i].categorias);
    }
    free(v);
}

//valida se o tipo de um atributo é condizente com um conteudo
//retorna 1 se sim e 0 se não
int valida_tipo(atributo atributo,char *conteudo){
  int tipo_flag=0;

  if(strcmp(atributo.tipo,"string")==0){
    tipo_flag=1;
  }
  else if(strcmp(atributo.tipo,"numeric")==0) {;
    for(int j=0;j<strlen(conteudo);j++){
      if(conteudo[j]>=48 && conteudo[j]<=57)
        tipo_flag=1;
      else if(conteudo[j]==46)
        tipo_flag=1;
      else{
        tipo_flag=0;
        return tipo_flag;
      }
    }
  }
  else if(atributo.categorias!=NULL) {
    for(int i=0;i<atributo.cat_tam;i++){
      if(strcmp(atributo.categorias[i],conteudo)==0){
        tipo_flag=1;
      }
    }
  }
  return tipo_flag;
}

//Recebe um arquivo arff com ponteiro de leitura antes do "@data"; passa por todas as linhas de dados e valida cada elementos de cada coluna em
//relação ao vetor de atributos também fornecido como argumento.
void valida_arff(FILE *arff, atributo *atributos, int quantidade){
      char linha[2049],*dados[quantidade];
  int n_linha=0;

  //chega no @data
  fscanf(arff,"%1025[^\n]",linha);
  while(strcmp(linha,"@data")!=0){
    fscanf(arff,"%1025[^\n]",linha);
  }

  while(!feof(arff)){
    fscanf(arff,"%2049[^\n]",linha);
    
    //verificação de quantidade    
    int virgulas=conta_virgula(linha);
    if(virgulas+1!=quantidade){
      printf("quantidade de dados da linha %d não condiz com os atributos do arquivo\n",n_linha);
      return;
    }

    dados[0]=strtok(linha,",");
    for(int i=1;i<quantidade;i++){
      dados[i]=strtok(NULL,",");
    }

    for(int j=0;j<quantidade;j++){
      if(valida_tipo( atributos[j] , dados[j] )){
        printf("tipo do dado %d da linha %d não condiz com o atributo\n",j,n_linha);
        return;
      }
    }
    n_linha++;
  }
  printf("o arquivo é valido");
}
}
