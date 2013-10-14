/* HTAB = 4 */
/****************************************************************************
 * bstdfile.c -- This module implements a buffered interface for the		*
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
 * $Revision: 1.3 $
 */

/****************************************************************************
 * Includes.																*
 ****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <errno.h>
#include <sys/types.h>
#include "bstdfile.h"

/****************************************************************************
 * Preprocessor definitions													*
 ****************************************************************************/
#define BFILE_BUFSIZE	(8192U)

/* We use some errno symbols that may be undefined. We supply such
 * definitions, all with the 1 value, only to achieve compilation: the
 * semantics of the signaled errors may thus be broken. It is the
 * responsibility of this module user to map the following symbols to
 * the symbols provided by the target system.
 */
#if (!defined(unix) && !defined (__unix__) && !defined(__unix))
/* Memory exhausted */
# ifndef ENOMEM
#  define ENOMEM	(1)
# endif

/* Bad file descriptor */
# ifndef EBADF
#  define EBADF		(1)
# endif

/* Invalid address. */
# ifndef EFAULT
#  define EFAULT	(1)
# endif

/* Invalid argument. */
# ifndef EINVAL
#  define EINVAL	(1)
# endif
#endif

/****************************************************************************
 * Datatypes definitions													*
 ****************************************************************************/
struct bstdfile
{
	/* buffer is an internal buffer used by BstdRead(). live points
	 * inside that buffer to the data that was not yet transmitted to
	 * the user. live_size is the number of bytes available for direct
	 * consumption to feed the user buffer from the live pointer.
	 */
	char	buffer[BFILE_BUFSIZE],
			*live;
	size_t	live_size;

	/* fp is the file opened for reading associated with that bfile. */
	FILE	*fp;

	/* Error management: eof is non-zero when an end of file condition
	 * was detected. error is zero when no error was detected (error
	 * is zero when eof is 1), it stores the errno of the detected
	 * error.
	 */
	int		eof,
			error;
};

/****************************************************************************
 * Creates a new bstdfile from an already opened file.						*
 ****************************************************************************/
bstdfile_t *NewBstdFile(FILE *fp)
{
	bstdfile_t	*BstdFile;

	/* Allocate the bstdfile structure. */
	BstdFile=(bstdfile_t *)malloc(sizeof(bstdfile_t));
	if(BstdFile==NULL)
	{
		errno=ENOMEM;
		return(NULL);
	}

	/* Initialize the structure to safe defaults. */
	BstdFile->live=BstdFile->buffer;
	BstdFile->live_size=0;
	BstdFile->eof=0;
	BstdFile->error=0;
	BstdFile->fp=fp;

	/* Return the new bfile. */
	return(BstdFile);
}

/****************************************************************************
 * Destroys a previously allocated BstdFile.								*
 ****************************************************************************/
int BstdFileDestroy(bstdfile_t *BstdFile)
{
	if(BstdFile==NULL)
	{
		errno=EBADF;
		return(1);
	}
	free(BstdFile);
	return(0);
}

/****************************************************************************
 * This predicate returns a non nul value when there is an end of file		*
 * condition on the BstdFile argument.										*
 ****************************************************************************/
int BstdFileEofP(const bstdfile_t *BstdFile)
{
	return(BstdFile->eof);
}

/****************************************************************************
 * This predicate returns a non nul value when there is an error condition	*
 * on the BstdFile argument.												*
 ****************************************************************************/
int BstdFileErrorP(const bstdfile_t *BstdFile)
{
	return(BstdFile->error);
}

/****************************************************************************
 * This works as read(2) but operates on a bfile instead of a file			*
 * descriptor.																*
 ****************************************************************************/
size_t BstdRead(void *UserBuffer, size_t ElementSize, size_t ElementsCount, bstdfile_t *BstdFile)
{
	size_t	RequestSize=ElementSize*ElementsCount,
			FeededSize=0,
			ReadSize,
			ObtainedSize;
	int		OldErrno=errno;

	/* Check the validity of the arguments. */
	if(BstdFile==NULL)
	{
		errno=EBADF;
		return((size_t)0);
	}
	if(UserBuffer==NULL)
	{
		errno=EFAULT;
		return((size_t)0);
	}
	if(RequestSize<1)
	{
		errno=EINVAL;
		return((size_t)0);
	}

	/* Return immediately if an exceptional situation exists. */
	if(BstdFile->eof)
		return((size_t)0);
	if(BstdFile->error)
	{
		errno=BstdFile->error;
		return((size_t)0);
	}

	/* The easy case. */
	if(RequestSize==0U)
		return((size_t)0);

	/* First feed the target buffer from the BstdFile buffer if it has
	 * some meat to be feeded on.
	 */
	if(BstdFile->live_size>0)
	{
		/* If there is more data in the buffer than requested by the
		 * user then we feed him directly from our buffer without a
		 * read operation.
		 */
		if(BstdFile->live_size>RequestSize)
		{
			memcpy(UserBuffer,BstdFile->live,RequestSize);
			BstdFile->live+=RequestSize;
			BstdFile->live_size-=RequestSize;
			return(RequestSize);
		}
		/* Else we drain our buffer. */
		else
		{
			memcpy(UserBuffer,BstdFile->live,BstdFile->live_size);
			UserBuffer=(char *)UserBuffer+BstdFile->live_size;
			FeededSize=BstdFile->live_size;
			BstdFile->live=BstdFile->buffer;
			BstdFile->live_size=0;
		}
	}

	/* If the user request was not yet fulfilled we then read from the
     * file the remaining data requested by the user.
	 */
	if(FeededSize<RequestSize)
	{
		ReadSize=RequestSize-FeededSize;
		ObtainedSize=fread(UserBuffer,1,ReadSize,BstdFile->fp);
		FeededSize+=ObtainedSize;

		/* If an error occurs we return the amount of data that was
		 * feeded from the buffer and store the error condition for a
		 * later call. If our buffer was empty and we thus have
		 * transferred no data to the user buffer then we directly
		 * return the error.
		 */
		if(ObtainedSize==0U)
		{
			if(feof(BstdFile->fp))
				BstdFile->eof=1;
			else
			{
				BstdFile->error=errno;
				errno=OldErrno;
			}
			if(FeededSize!=0)
				return(FeededSize);
			else
				return(0U);
		}
	}

	/* Fill again our buffer. In case of error, or end of file, that
	 * error is recorded but we still report the amount of data that
	 * was feeded to the user buffer.
	 */
	ObtainedSize=fread(BstdFile->buffer,1,BFILE_BUFSIZE,BstdFile->fp);
	if(ObtainedSize==0)
	{
		if(feof(BstdFile->fp))
			BstdFile->eof=1;
		else
		{
			BstdFile->error=errno;
			errno=OldErrno;
		}
	}
	else
	{
		BstdFile->live=BstdFile->buffer;
		BstdFile->live_size=ObtainedSize;
	}

	/* Eventually return the number ob bytes feeded to the user
     * buffer.
	 */
	return(FeededSize);
}

/*  LocalWords:  HTAB bstdfile fread Datatypes BstdRead fp bfile BstdFile
 */
/*
 * Local Variables:
 * tab-width: 4
 * End:
 */

/****************************************************************************
 * End of file bstdfile.c														*
 ****************************************************************************/
