/******************************************************************************/
/* Important CSCI 402 usage information:                                      */
/*                                                                            */
/* This fils is part of CSCI 402 programming assignments at USC.              */
/*         53616c7465645f5fd1e93dbf35cbffa3aef28f8c01d8cf2ffc51ef62b26a       */
/*         f9bda5a68e5ed8c972b17bab0f42e24b19daa7bd408305b1f7bd6c7208c1       */
/*         0e36230e913039b3046dd5fd0ba706a624d33dbaa4d6aab02c82fe09f561       */
/*         01b0fd977b0051f0b0ce0c69f7db857b1b5e007be2db6d42894bf93de848       */
/*         806d9152bd5715e9                                                   */
/* Please understand that you are NOT permitted to distribute or publically   */
/*         display a copy of this file (or ANY PART of it) for any reason.    */
/* If anyone (including your prospective employer) asks you to post the code, */
/*         you must inform them that you do NOT have permissions to do so.    */
/* You are also NOT permitted to remove or alter this comment block.          */
/* If this comment block is removed or altered in a submitted file, 20 points */
/*         will be deducted.                                                  */
/******************************************************************************/

/*
 * Author:      William Chia-Wei Cheng (bill.cheng@acm.org)
 *
 * @(#)$Id: my402list.c,v 1.1 2019/05/19 23:34:12 cvsps Exp $
 */
#include "cs402.h"

#include "my402list.h"

#include <stdlib.h>

int My402ListLength(My402List* listobject)
{
	return listobject -> num_members;
}

int  My402ListEmpty(My402List* listobject)
{
	return listobject -> num_members <= 0;
}

int  My402ListAppend(My402List* listobject, void* dataobject)
{
	My402ListElem *elementobject = (My402ListElem *) malloc(sizeof(My402ListElem));
	if(elementobject)
	{
		listobject -> num_members++;
		elementobject -> obj = dataobject;
		if(My402ListEmpty(listobject))
		{
			(listobject -> anchor).next = elementobject;
			elementobject -> next = &(listobject -> anchor);
			(listobject -> anchor).prev = elementobject;
			elementobject -> prev = &(listobject -> anchor);
		}
		else
		{
			My402ListElem *lastpointer = (listobject -> anchor).prev;
			lastpointer -> next = elementobject;
			(listobject -> anchor).prev = elementobject;
			elementobject -> prev = lastpointer;
			elementobject -> next = &(listobject -> anchor);
		}
		return 1;
	}
	else
	{
		return 0;	
	}
}

int  My402ListPrepend(My402List* listobject, void* dataobject)
{
	My402ListElem *elementobject = (My402ListElem *) malloc(sizeof(My402ListElem));
	if(elementobject)
	{
		listobject -> num_members++;
		elementobject -> obj = dataobject;
		if(My402ListEmpty(listobject))
		{
			(listobject -> anchor).next = elementobject;
			elementobject -> prev = &(listobject -> anchor);
			(listobject -> anchor).prev = elementobject;
			elementobject -> next = &(listobject -> anchor);
		}
		else
		{
			My402ListElem *firstpointer = (listobject -> anchor).next;
			(listobject -> anchor).next = elementobject;
			firstpointer -> prev = elementobject;
			elementobject -> prev = &(listobject -> anchor);
			elementobject -> next = firstpointer;
		}
		return 1;
	}
	else
	{
		return 0;		
	}
}

void My402ListUnlink(My402List* listobject, My402ListElem* elementobject)
{
	if(My402ListEmpty(listobject))
	{
		return;
	}
	listobject -> num_members--;
	My402ListElem *previouspointer = elementobject -> prev, *nextpointer = elementobject -> next;
	free(elementobject);
	previouspointer -> next = nextpointer;
	nextpointer -> prev = previouspointer;
}

void My402ListUnlinkAll(My402List* listobject)
{
	if(My402ListEmpty(listobject))
	{
		return;
	}
	My402ListElem *currentpointer = (listobject -> anchor).next;
	for(currentpointer = (listobject -> anchor).next; currentpointer != &(listobject -> anchor); currentpointer = currentpointer -> next)
	{
		My402ListElem *temppointer = currentpointer;
		My402ListUnlink(listobject, temppointer);
	}
}

int  My402ListInsertAfter(My402List* listobject, void* dataobject, My402ListElem* elementpointer)
{
	if(elementpointer)
	{
		My402ListElem *newelementobject = (My402ListElem *) malloc(sizeof(My402ListElem));
		if(newelementobject)
		{
			listobject -> num_members++;
			newelementobject -> obj = dataobject;
			My402ListElem *nextelementpointer = elementpointer -> next;
			newelementobject -> next = nextelementpointer;
			elementpointer -> next = newelementobject;
			nextelementpointer -> prev = newelementobject;
			newelementobject -> prev = elementpointer;
			return 1;
		}
		else
			return 0;
	}
	else
	{
		return My402ListAppend(listobject, dataobject);
	}
}

int  My402ListInsertBefore(My402List* listobject, void* dataobject, My402ListElem* elementpointer)
{
	if(elementpointer)
	{
		My402ListElem *newelementobject = (My402ListElem *) malloc(sizeof(My402ListElem));
		if(newelementobject)
		{
			listobject -> num_members++;
			newelementobject -> obj = dataobject;
			My402ListElem *previouselementpointer = elementpointer -> prev;
			newelementobject -> prev = previouselementpointer;
			elementpointer -> prev = newelementobject;
			previouselementpointer -> next = newelementobject;
			newelementobject -> next = elementpointer;
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return My402ListPrepend(listobject, dataobject);
	}
}

My402ListElem *My402ListFirst(My402List* listobject)
{
	if(My402ListEmpty(listobject))
	{
		return NULL;
	}
	else
	{
		return (listobject -> anchor).next;
	}
}

My402ListElem *My402ListLast(My402List* listobject)
{
	if(My402ListEmpty(listobject))
	{
		return NULL;
	}
	else
	{
		return (listobject -> anchor).prev;
	}
}

My402ListElem *My402ListNext(My402List* listobject, My402ListElem* elementpointer)
{
	if(elementpointer)
	{
		if(elementpointer == My402ListLast(listobject))
		{
			return NULL;
		}
		else
		{
			return elementpointer -> next;
		}
	}
	else
	{
		return My402ListFirst(listobject);
	}
}

My402ListElem *My402ListPrev(My402List* listobject, My402ListElem* elementpointer)
{
	if(elementpointer)
	{
		if(elementpointer == My402ListFirst(listobject))
		{
			return NULL;
		}
		else
		{
			return elementpointer -> prev;
		}
	}
	else
	{
		return My402ListLast(listobject);
	}
}

My402ListElem *My402ListFind(My402List* listobject, void* dataobject)
{
	My402ListElem *currentpointer = NULL;
	for(currentpointer = My402ListFirst(listobject); currentpointer != NULL; currentpointer = My402ListNext(listobject, currentpointer))
	{
		if(currentpointer -> obj == dataobject)
		{
			return currentpointer;
		}
	}
	return NULL;
}

int My402ListInit(My402List* listobject)
{
	listobject -> num_members = 0;
	(listobject -> anchor).next = &(listobject -> anchor);
	(listobject -> anchor).prev = &(listobject -> anchor);
	(listobject -> anchor).obj = NULL;
	return 1;
}
