/*============================================================================*/
/* MANIPULAÇÃO DE ARQUIVOS BMP                                                */
/*----------------------------------------------------------------------------*/
/* Autor: Bogdan T. Nassu - nassu@dainf.ct.utfpr.edu.br                       */
/*============================================================================*/
/** Este arquivo traz declarações de um tipo e rotinas para manipulação de
 * arquivos bmp. Como temos um propósito puramente didático, apenas um sub-
 * conjunto mínimo do formato foi implementado. Usamos aqui matrizes alocadas
 * dinamicamente e números de ponto flutuante. Esta não é a forma mais eficiente
 * de se trabalhar com imagens, mas procuramos priorizar a clareza e a
 * facilidade de uso. */
/*============================================================================*/

#ifndef __IMAGEM_H
#define __IMAGEM_H

/*============================================================================*/

typedef struct
{
	int largura;
	int altura;
	int n_canais;
	float*** dados; /* Uma matriz de dados por canal. Acessar com 3 índices: [canal][y][x]. */
} Imagem;

/*----------------------------------------------------------------------------*/
/* Por simplicidade e compatibilidade, nós sempre consideramos a leitura e
 * escrita de imagens com 3 canais, 24bpp. Todas as conversões para escala de
 * cinza e float são feitas internamente. */

Imagem* criaImagem (int largura, int altura, int n_canais);
void destroiImagem (Imagem* img);
Imagem* abreImagem (char* arquivo, int n_canais);
int salvaImagem (Imagem* img, char* arquivo);
Imagem* clonaImagem (Imagem* img);
void copiaConteudo (Imagem* in, Imagem* out);

/*============================================================================*/
#endif /* __IMAGEM_H */
