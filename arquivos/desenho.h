/*============================================================================*/
/* FUNÇÕES PARA DESENHAR COISAS                                               */
/*----------------------------------------------------------------------------*/
/* Autor: Bogdan T. Nassu - nassu@dainf.ct.utfpr.edu.br                       */
/*============================================================================*/
/** Funções para desenhar elementos em imagens. */
/*============================================================================*/

#ifndef __DESENHO_H
#define __DESENHO_H

/*============================================================================*/

#include "base.h"
#include "geometria.h"
#include "cores.h"
#include "imagem.h"

/*============================================================================*/

void desenhaLinha (Coordenada p1, Coordenada p2, Cor cor, Imagem* img);
void desenhaRetangulo (Retangulo r, Cor cor, Imagem* img);

/*============================================================================*/
#endif /* __DESENHO_H */
