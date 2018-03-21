/*============================================================================*/
/* SEGMENTAÇÃO                                                                */
/*----------------------------------------------------------------------------*/
/* Autor: Bogdan T. Nassu - nassu@dainf.ct.utfpr.edu.br                       */
/*============================================================================*/
/** Tipos e funções para segmentação. */
/*============================================================================*/

#ifndef __SEGMENTA_H
#define __SEGMENTA_H

/*============================================================================*/

#include "imagem.h"
#include "geometria.h"

/*============================================================================*/

typedef struct
{
    float label;
    Retangulo roi;
    int n_pixels;

} ComponenteConexo;

/*----------------------------------------------------------------------------*/

void binariza (Imagem* in, Imagem* out, float threshold);
int rotulaFloodFill (Imagem* img, ComponenteConexo** componentes, int largura_min, int altura_min, int n_pixels_min);
void floodFill (Imagem* img, Coordenada* pilha, ComponenteConexo* componente);
int rotulaUnionFind (Imagem* img, ComponenteConexo** componentes, int largura_min, int altura_min, int n_pixels_min);

/*============================================================================*/
#endif /* __IMAGEM_H */
