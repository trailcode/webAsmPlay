/*
// ----------------------------------------------------------------------------
//
//
// OpenSteer -- Steering Behaviors for Autonomous Characters
//
// Copyright (c) 2002-2005, Sony Computer Entertainment America
// Original author: Craig Reynolds <craig_reynolds@playstation.sony.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
// ----------------------------------------------------------------------------
*/
/* ------------------------------------------------------------------ */
/*                                                                    */
/* Locality Query facility                                            */
/*                                                                    */
/* (by Craig Reynolds, see lq.h file for documentation)               */
/*                                                                    */
/*  5-17-99: created                                                  */
/*  5-20-99: found elusive "allocate 0 bins" bug                      */
/*  5-28-99: lqMapOverAllObjectsInLocality: clipped, incremental      */
/*  6- 7-99: clean up, split off annotation stuff into debuglq.c      */
/*  6- 8-99: tried screening by sum of coords ("first mean"?) but     */
/*           it was slightly slower, moved unused code to debuglq     */
/* 10-19-99: Change lqClientObject, lqObject from: "struct x {};" to  */
/*           "typedef struct x {} x;" for EE compiler.                */
/* 12- 2-00: Make lqObject "private" using lqInternalDB_2D.              */
/* 12- 5-00: Rename lqObject to lqDB_2D, lqClientObject to lqClientProxy2D */
/* 12- 6-00: Change lqCallBackFunction from arglist of (void*) to:    */
/*           (void* clientObject, float distanceSquared, void*        */
/*           clientQueryState).  Add void* clientQueryState arg to    */
/*           lqMapOverAllObjectsInLocality and its helper functions   */
/*           lqMapOverAllObjectsInLocalityClipped and                 */
/*           lqMapOverAllOutsideObjects. Change macro                 */
/*           lqTraverseBinClientObjectList to invoke callback         */
/*           function with three arguments, add "state" to its        */
/*           arglist.  Remove extern lqDistanceSquared.               */
/* 12- 7-00: Rename lqInitClientObject to lqInitClientProxy, make     */
/*           "func" be an argument to lqTraverseBinClientObjectList,  */
/*           add comments.                                            */
/* 12- 8-00: Add lqFindNearestNeighborWithinRadius and related        */
/*           definitions: lqFindNearestHelper lqFindNearestState      */
/*           Add lqMapOverAllObjects and lqRemoveAllObjects (plus:    */
/*           lqMapOverAllObjectsInBin and lqRemoveAllObjectsInBin)    */
/*                                                                    */
/* 12-05-2018 Modified by Matthew Tang (trailcode@gmail.com)          */
/*            Made 2D                                                 */
/* ------------------------------------------------------------------ */


#include <stdlib.h>
#include <float.h>
#include <limits.h> /* for INT_MAX */
#include <webAsmPlay/Debug.h>
#include "OpenSteer/lq2D.h"

/* for debugging and graphical annotation (normally unused) */
#ifdef BOIDS_LQ_DEBUG
#include "OpenSteer/debuglq.c"
#endif

#ifndef WIN32
#define USUSED_PARAM __attribute__ ((unused))
#else
#define USUSED_PARAM
#endif


/* ------------------------------------------------------------------ */
/* This structure represents the spatial database.  Typically one of
   these would be created, by a call to lqCreateDatabase2D, for a given
   application.  */


typedef struct lqInternalDB_2D
{

    /* the origin is the super-brick corner minimum coordinates */
    float originx, originy;

    /* length of the edges of the super-brick */
    float sizex, sizey;

    /* number of sub-brick divisions in each direction */
    int divx, divy;

    /* pointer to an array of pointers, one for each bin */
    lqClientProxy2D** bins;

    /* extra bin for "everything else" (points outside super-brick) */
    lqClientProxy2D* other;

} lqInternalDB_2D;


/* ------------------------------------------------------------------ */
/* Allocate and initialize an LQ database, return a pointer to it.
   The application needs to call this before using the LQ facility.
   The nine parameters define the properties of the "super-brick":
      (1) origin: coordinates of one corner of the super-brick, its
          minimum x, y and z extent.
      (2) size: the width, height and depth of the super-brick.
      (3) the number of subdivisions (sub-bricks) along each axis.
   This routine also allocates the bin array, and initialize its
   contents. */


lqInternalDB_2D* lqCreateDatabase2D (   float originx, float originy,
                                        float sizex, float sizey,
                                        int divx, int divy)
{
    lqInternalDB_2D* lq = ((lqInternalDB_2D*) malloc (sizeof (lqInternalDB_2D)));

    lqInitDatabase2D (lq,
		    originx, originy,
		    sizex, sizey,
		    divx, divy);
    return lq;
}


/* ------------------------------------------------------------------ */
/* Deallocate the memory used by the LQ database */


void lqDeleteDatabase2D(lqDB_2D* lq)
{
    free (lq->bins);
    free (lq);
}


/* ------------------------------------------------------------------ */
/* Given an LQ database object and the nine basic parameters: fill in
   the object's slots, allocate the bin array, and initialize its
   contents. */


void lqInitDatabase2D ( lqInternalDB_2D* lq,
                        float originx, float originy,
                        float sizex, float sizey,
                        int divx, int divy)
{
    lq->originx = originx;
    lq->originy = originy;
    lq->sizex = sizex;
    lq->sizey = sizey;
    lq->divx = divx;
    lq->divy = divy;
    {
	int i;
	int bincount = divx * divy;
	int arraysize = sizeof (lqClientProxy2D*) * bincount;
	lq->bins = (lqClientProxy2D**) malloc (arraysize);
	for (i=0; i<bincount; i++) lq->bins[i] = NULL;
    }
    lq->other = NULL;
}


/* ------------------------------------------------------------------ */
/* Determine index into linear bin array given 2D bin indices */


#define lqBinCoordsToBinIndex(lq, ix, iy) \
    ((ix * lq->divy) + iy)


/* ------------------------------------------------------------------ */
/* Find the bin ID for a location in space.  The location is given in
   terms of its XYZ coordinates.  The bin ID is a pointer to a pointer
   to the bin contents list.  */


lqClientProxy2D** lqBinForLocation2D (lqInternalDB_2D* lq, float x, float y)
{
    int i, ix, iy;

    /* if point outside super-brick, return the "other" bin */
    if (x < lq->originx)              return &(lq->other);
    if (y < lq->originy)              return &(lq->other);
    
    if (x >= lq->originx + lq->sizex) return &(lq->other);
    if (y >= lq->originy + lq->sizey) return &(lq->other);

    /* if point inside super-brick, compute the bin coordinates */
    ix = (int) (((x - lq->originx) / lq->sizex) * lq->divx);
    iy = (int) (((y - lq->originy) / lq->sizey) * lq->divy);

    /* convert to linear bin number */
    i = lqBinCoordsToBinIndex (lq, ix, iy);

    /* return pointer to that bin */
    return &(lq->bins[i]);
}


/* ------------------------------------------------------------------ */
/* The application needs to call this once on each lqClientProxy2D at
   setup time to initialize its list pointers and associate the proxy
   with its client object. */ 


void lqInitClientProxy2D (lqClientProxy2D* proxy, void* clientObject)
{
    proxy->prev   = NULL;
    proxy->next   = NULL;
    proxy->bin    = NULL;
    proxy->object = clientObject;
}


/* ------------------------------------------------------------------ */
/* Adds a given client object to a given bin, linking it into the bin
   contents list. */


void lqAddToBin2D (lqClientProxy2D* object, lqClientProxy2D** bin)
{
    /* if bin is currently empty */    
    if (*bin == NULL)
    {
        object->prev = NULL;
        object->next = NULL;
        *bin = object;
    }
    else
    {
        object->prev = NULL;
        object->next = *bin;
        (*bin)->prev = object;
        *bin = object;
    }

    /* record bin ID in proxy object */
    object->bin = bin;
}


/* ------------------------------------------------------------------ */
/* Removes a given client object from its current bin, unlinking it
   from the bin contents list. */


void lqRemoveFromBin2D (lqClientProxy2D* object)
{
    /* adjust pointers if object is currently in a bin */
    if (object->bin != NULL)
    {
        /* If this object is at the head of the list, move the bin
        pointer to the next item in the list (might be NULL). */
        if (*(object->bin) == object) *(object->bin) = object->next;

        /* If there is a prev object, link its "next" pointer to the
        object after this one. */
        if (object->prev != NULL) object->prev->next = object->next;

        /* If there is a next object, link its "prev" pointer to the
        object before this one. */
        if (object->next != NULL) object->next->prev = object->prev;
    }

    /* Null out prev, next and bin pointers of this object. */
    object->prev = NULL;
    object->next = NULL;
    object->bin = NULL;
}


/* ------------------------------------------------------------------ */
/* Call for each client object every time its location changes.  For
   example, in an animation application, this would be called each
   frame for every moving object.  */


void lqUpdateForNewLocation2D  (lqInternalDB_2D* lq, 
                                lqClientProxy2D* object, 
                                float x, float y)
{
    /* find bin for new location */
    lqClientProxy2D** newBin = lqBinForLocation2D (lq, x, y);

    /* store location in client object, for future reference */
    object->x = x;
    object->y = y;

    /* has object moved into a new bin? */
    if (newBin != object->bin)
    {
        lqRemoveFromBin2D (object);
        lqAddToBin2D (object, newBin);
    }
}


/* ------------------------------------------------------------------ */
/* Given a bin's list of client proxies, traverse the list and invoke
   the given lqCallBackFunction on each object that falls within the
   search radius.  */


#define lqTraverseBinClientObjectList2D(co, radiusSquared, func, state) \
    while (co != NULL)                                                \
    {                                                                 \
        /* compute distance (squared) from this client   */           \
        /* object to given locality sphere's centerpoint */           \
        float dx = x - co->x;                                         \
        float dy = y - co->y;                                         \
        float distanceSquared = (dx * dx) + (dy * dy);                  \
                                                                        \
        /* apply function if client object within sphere */           \
        if (distanceSquared < radiusSquared)                          \
            (*func) (co->object, distanceSquared, state);             \
                                                                        \
        /* consider next client object in bin list */                 \
        co = co->next;                                                \
    }


/* ------------------------------------------------------------------ */
/* This subroutine of lqMapOverAllObjectsInLocality efficiently
   traverses of subset of bins specified by max and min bin
   coordinates. */

void lqMapOverAllObjectsInLocalityClipped2D (lqInternalDB_2D* lq, 
                                            float x, float y,
                                            float radius,
                                            lqCallBackFunction2D func,
                                            void* clientQueryState,
                                            int minBinX,
                                            int minBinY, 
                                            int maxBinX,
                                            int maxBinY)
{
    int i, j;
    int iindex, jindex;
    int slab = lq->divy;
    int istart = minBinX * slab;
    int jstart = minBinY;
    lqClientProxy2D* co;
    lqClientProxy2D** bin;
    float radiusSquared = radius * radius;

#ifdef BOIDS_LQ_DEBUG
    if (lqAnnoteEnable) drawBallGL (x, y, z, radius);
#endif

    /* loop for x bins across diameter of sphere */
    iindex = istart;
    for (i = minBinX; i <= maxBinX; i++)
    {
        /* loop for y bins across diameter of sphere */
        jindex = jstart;
        for (j = minBinY; j <= maxBinY; j++)
        {
            /* get current bin's client object list */
            bin = &lq->bins[iindex + jindex];
            co = *bin;

    #ifdef BOIDS_LQ_DEBUG
            if (lqAnnoteEnable) drawBin (lq, bin);
    #endif
            /* traverse current bin's client object list */
            lqTraverseBinClientObjectList2D (co,
                            radiusSquared,
                            func,
                            clientQueryState);
            
            jindex += 1;
        }
        iindex += slab;
    }
}

/* ------------------------------------------------------------------ */
/* If the query region (sphere) extends outside of the "super-brick"
   we need to check for objects in the catch-all "other" bin which
   holds any object which are not inside the regular sub-bricks  */

void lqMapOverAllOutsideObjects2D (lqInternalDB_2D* lq, 
				 float x, float y,
				 float radius,
				 lqCallBackFunction2D func,
				 void* clientQueryState)
{
    lqClientProxy2D* co = lq->other;
    float radiusSquared = radius * radius;

    /* traverse the "other" bin's client object list */
    lqTraverseBinClientObjectList2D (co,
				   radiusSquared,
				   func,
				   clientQueryState);
}


/* ------------------------------------------------------------------ */
/* Apply an application-specific function to all objects in a certain
   locality.  The locality is specified as a sphere with a given
   center and radius.  All objects whose location (key-point) is
   within this sphere are identified and the function is applied to
   them.  The application-supplied function takes three arguments:

     (1) a void* pointer to an lqClientProxy2D's "object".
     (2) the square of the distance from the center of the search
         locality sphere (x,y,z) to object's key-point.
     (3) a void* pointer to the caller-supplied "client query state"
         object -- typically NULL, but can be used to store state
         between calls to the lqCallBackFunction.

   This routine uses the LQ database to quickly reject any objects in
   bins which do not overlap with the sphere of interest.  Incremental
   calculation of index values is used to efficiently traverse the
   bins of interest. */


void lqMapOverAllObjectsInLocality2D (lqInternalDB_2D* lq, 
				    float x, float y,
				    float radius,
				    lqCallBackFunction2D func,
				    void* clientQueryState)
{
    int partlyOut = 0;
    int completelyOutside = 
	(((x + radius) < lq->originx) ||
	 ((y + radius) < lq->originy) ||
	 ((x - radius) >= lq->originx + lq->sizex) ||
	 ((y - radius) >= lq->originy + lq->sizey));
    int minBinX, minBinY, maxBinX, maxBinY;

    /* is the sphere completely outside the "super brick"? */
    if (completelyOutside)
    {
        lqMapOverAllOutsideObjects2D (lq, x, y, radius, func, clientQueryState);
        return;
    }

    /* compute min and max bin coordinates for each dimension */
    minBinX = (int) ((((x - radius) - lq->originx) / lq->sizex) * lq->divx);
    minBinY = (int) ((((y - radius) - lq->originy) / lq->sizey) * lq->divy);
    maxBinX = (int) ((((x + radius) - lq->originx) / lq->sizex) * lq->divx);
    maxBinY = (int) ((((y + radius) - lq->originy) / lq->sizey) * lq->divy);

    /* clip bin coordinates */
    if (minBinX < 0)         {partlyOut = 1; minBinX = 0;}
    if (minBinY < 0)         {partlyOut = 1; minBinY = 0;}
    if (maxBinX >= lq->divx) {partlyOut = 1; maxBinX = lq->divx - 1;}
    if (maxBinY >= lq->divy) {partlyOut = 1; maxBinY = lq->divy - 1;}

    /* map function over outside objects if necessary (if clipped) */
    if (partlyOut) 
	lqMapOverAllOutsideObjects2D (lq, x, y, radius, func, clientQueryState);
    
    /* map function over objects in bins */
    lqMapOverAllObjectsInLocalityClipped2D (lq,
					  x, y,
					  radius,
					  func,
					  clientQueryState,
					  minBinX, minBinY,
					  maxBinX, maxBinY);
}


/* ------------------------------------------------------------------ */
/* internal helper function */

namespace
{
    typedef struct lqFindNearestState
    {
        void* ignoreObject;
        void* nearestObject;
        float minDistanceSquared;

    } lqFindNearestState;
}

void lqFindNearestHelper2D (void* clientObject,
                            float distanceSquared,
                            void* clientQueryState)
{
    lqFindNearestState* fns = (lqFindNearestState*) clientQueryState;

    /* do nothing if this is the "ignoreObject" */
    if (fns->ignoreObject != clientObject)
    {
	/* record this object if it is the nearest one so far */
	if (fns->minDistanceSquared > distanceSquared)
	{
	    fns->nearestObject = clientObject;
	    fns->minDistanceSquared = distanceSquared;
	}
    }
}


/* ------------------------------------------------------------------ */
/* Search the database to find the object whose key-point is nearest
   to a given location yet within a given radius.  That is, it finds
   the object (if any) within a given search sphere which is nearest
   to the sphere's center.  The ignoreObject argument can be used to
   exclude an object from consideration (or it can be NULL).  This is
   useful when looking for the nearest neighbor of an object in the
   database, since otherwise it would be its own nearest neighbor.
   The function returns a void* pointer to the nearest object, or
   NULL if none is found.  */


void* lqFindNearestNeighborWithinRadius2D (lqInternalDB_2D* lq, 
					 float x, float y,
					 float radius,
					 void* ignoreObject)
{
    /* initialize search state */
    lqFindNearestState lqFNS;
    lqFNS.nearestObject = NULL;
    lqFNS.ignoreObject = ignoreObject;
    lqFNS.minDistanceSquared = FLT_MAX;

    /* map search helper function over all objects within radius */
    lqMapOverAllObjectsInLocality2D (lq, 
				   x, y,
				   radius,
				   lqFindNearestHelper2D,
				   &lqFNS);

    /* return nearest object found, if any */
    return lqFNS.nearestObject;
}


/* ------------------------------------------------------------------ */
/* internal helper function */

void lqMapOverAllObjectsInBin2D (lqClientProxy2D* binProxyList, 
			       lqCallBackFunction2D func,
			       void* clientQueryState)
{
    /* walk down proxy list, applying call-back function to each one */
    while (binProxyList != NULL)
    {
        (*func) (binProxyList->object, 0, clientQueryState);
        binProxyList = binProxyList->next;
    }
}


/* ------------------------------------------------------------------ */
/* Apply a user-supplied function to all objects in the database,
   regardless of locality (cf lqMapOverAllObjectsInLocality) */

void lqMapOverAllObjects2D (lqInternalDB_2D* lq, 
			  lqCallBackFunction2D func,
			  void* clientQueryState)
{
    int i;
    int bincount = lq->divx * lq->divy;
    for (i=0; i<bincount; i++)
    {
	    lqMapOverAllObjectsInBin2D (lq->bins[i], func, clientQueryState);
    }
    lqMapOverAllObjectsInBin2D (lq->other, func, clientQueryState);
}

/* ------------------------------------------------------------------ */
/* looks at all bins (except "other") finding the min and max bin
   populations and the average of NON-EMPTY bin populations.  (The
   average over all bins is a constant (population/bincount))  */

#ifndef NO_LQ_BIN_STATS

void lqgbpsCounter (void* clientObject    USUSED_PARAM,
                    float distanceSquared USUSED_PARAM,
                    void* clientQueryState);

void lqgbpsCounter (void* clientObject    USUSED_PARAM,
                    float distanceSquared USUSED_PARAM,
                    void* clientQueryState)
{
    (*(int*)clientQueryState)++;
}

void lqGetBinPopulationStats2D (lqInternalDB_2D* lq,
                              int* min,
                              int* max,
                              float* average)
{
    int minPop = INT_MAX;
    int maxPop = 0;
    int totalCount = 0;
    int nonEmptyBinCount = 0;
    int bincount = lq->divx * lq->divy;
    int i;

    for (i=0; i<bincount; i++)
    {
        /* clear the counter */
        int objectCount = 0;

        /* apply counting function to each object in bin[i] */
	    lqMapOverAllObjectsInBin2D (lq->bins[i], lqgbpsCounter, &objectCount);

        /* collect data: max and min population, count objects and non-empty bins */
        if (objectCount > 0)
        {
            nonEmptyBinCount++;
            if (maxPop < objectCount) maxPop = objectCount;
            if (minPop > objectCount) minPop = objectCount;
            totalCount += objectCount;
        }
    }

    /* set return values */
    *min = minPop;
    *max = maxPop;
    *average = ((float) totalCount) / ((float) nonEmptyBinCount);
}

#endif /* NO_LQ_BIN_STATS */


/* ------------------------------------------------------------------ */
/* internal helper function */


#define lqRemoveAllObjectsInBin2D(bin) \
    while ((bin) != NULL) lqRemoveFromBin2D ((bin));


/* ------------------------------------------------------------------ */
/* Removes (all proxies for) all objects from all bins */


void lqRemoveAllObjects2D (lqInternalDB_2D* lq)
{
    int i;
    int bincount = lq->divx * lq->divy;
    for (i=0; i<bincount; i++)
    {
	    lqRemoveAllObjectsInBin2D (lq->bins[i]);
    }
    lqRemoveAllObjectsInBin2D (lq->other);
}


/* ------------------------------------------------------------------ */
