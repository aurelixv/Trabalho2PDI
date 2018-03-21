/*============================================================================*/
/* GEOMETRIA                                                                  */
/*----------------------------------------------------------------------------*/
/* Autor: Bogdan T. Nassu - nassu@dainf.ct.utfpr.edu.br                       */
/*============================================================================*/
/** Tipos e funções para representar entidades geométricas. */
/*============================================================================*/

#include "geometria.h"

/*============================================================================*/
/* COORDENADA                                                                 */
/*============================================================================*/
/** Apenas uma função útil para "criar" uma instância do tipo Coordenada.
 *
 * Parâmetros: int x: valor no eixo x.
 *             int y: valor no eixo y.
 *
 * Valor de retorno: a Coordenada criada. */

Coordenada criaCoordenada (int x, int y)
{
    Coordenada c;
    c.x = x;
    c.y = y;
    return (c);
}

/*============================================================================*/
/* RETANGULO                                                                  */
/*============================================================================*/
/** Apenas uma função útil para "criar" uma instância do tipo Retangulo.
 *
 * Parâmetros: int c: posição y do lado superior.
 *             int b: posição y do lado inferior.
 *             int e: posição x do lado esquerdo.
 *             int d: posição x do lado direito.
 *
 * Valor de retorno: o Retangulo criado. */

Retangulo criaRetangulo (int c, int b, int e, int d)
{
    Retangulo r;
    r.c = c;
    r.b = b;
    r.e = e;
    r.d = d;
    return (r);
}

/*============================================================================*/
