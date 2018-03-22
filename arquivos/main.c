#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pdi.h"

#define IMAGEM "Exemplos/a01 - Original.bmp"

int main() {
    
    Imagem *imagem;
    
    //Abre a imagem a ser borrada
    printf("A carregar a imagem [ %s ]... ", IMAGEM);
    imagem = abreImagem(IMAGEM, 3);
    if(!imagem) {
            printf("\nERRO: A imagem não pode ser aberta.\n");
            exit(1);
    }
    printf("Sucesso!\n");
    
    //Funções que borram a imagem
    //TODO: Algoritmo ingênuo
    
    //TODO: Filtro separável
    
    //TODO: Algoritmo com imagens integrais
    
    
    //Libera a memória alocada antes de terminar
    printf("Liberando memória...\n");
    destroiImagem(imagem);
    
    printf("Encerrando...\n");
    
    return 0;
}
