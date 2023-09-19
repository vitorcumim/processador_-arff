#ifndef __ARFF__
#define __ARFF__

typedef struct {
  char *rotulo;
  char *tipo;
  char **categorias;
  int cat_tam;
} atributo;

void exibe_atributos(atributo *infos, int quantidade);
int conta_atributos(FILE *arff);
int conta_virgula(char *str);
void processa_categorias(char **v,char *categorias,int cat_tam);
atributo* processa_atributos(FILE *arff);
void libera_atributos(atributo *v, int num_atributos);
int valida_tipo(atributo atributo,char *conteudo);
void valida_arff(FILE *arff, atributo *atributos, int quantidade);

#endif