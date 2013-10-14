/* HTAB = 4 */
/****************************************************************************
 * bstdfile.h -- This module implements a buffered interface for the		*
 * fread(2)	standard function that can signal an eof of file condition		*
 * synchronously with the transmission of the last bytes of a file.			*
 *--------------------------------------------------------------------------*
 * (c) 2004 Bertrand Petit													*
 *																			*
 * Redistribution and use in source and binary forms, with or without		*
 * modification, are permitted provided that the following conditions		*
 * are met:																	*
 *																			*
 * 1. Redistributions of source code must retain the above copyright		*
 *    notice, this list of conditions and the following disclaimer.			*
 *																			*
 * 2. Redistributions in binary form must reproduce the above				*
 *    copyright notice, this list of conditions and the following			*
 *    disclaimer in the documentation and/or other materials provided		*
 *    with the distribution.												*
 * 																			*
 * 3. Neither the name of the author nor the names of its contributors		*
 *    may be used to endorse or promote products derived from this			*
 *    software without specific prior written permission.					*
 * 																			*
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS''		*
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED		*
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A			*
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR		*
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,				*
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT			*
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF			*
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND		*
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,		*
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT		*
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF		*
 * SUCH DAMAGE.																*
 *																			*
 ****************************************************************************/

/*
 * $Name: v1_1p1 $
 * $Date: 2004/03/19 08:08:25 $
 * $Revision: 1.2 $
 */

#ifndef BSTDFILE_H
#define BSTDFILE_H

/****************************************************************************
 * Datatypes definitions													*
 ****************************************************************************/
typedef struct bstdfile bstdfile_t;

/****************************************************************************
 * Prototypes																*
 ****************************************************************************/
extern bstdfile_t	*NewBstdFile(FILE *fp);
extern int			BstdFileDestroy(bstdfile_t *BstdFile);
extern int			BstdFileEofP(const bstdfile_t *BstdFile);
extern int			BstdFileErrorP(const bstdfile_t *BstdFile);
extern size_t		BstdRead(void *UserBuffer, size_t ElementSize, size_t ElementsCount, bstdfile_t *BstdFile);

#endif /* BSTDFILE_H */

/*  LocalWords:  HTAB bstdfile fread Datatypes
 */
/*
 * Local Variables:
 * tab-width: 4
 * End:
 */

/****************************************************************************
 * End of file bstdfile.h													*
 ****************************************************************************/
