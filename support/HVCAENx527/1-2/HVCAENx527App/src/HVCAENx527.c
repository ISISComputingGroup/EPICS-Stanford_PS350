/* $Header: HVCAENx527/HVCAENx527App/src/HVCAENx527.c 1.20 2007/06/01 13:32:57CST Ru Igarashi (igarasr) Exp Ru Igarashi (igarasr)(2007/06/01 13:32:57CST) $
 *
 * Copyright Canadian Light Source, Inc.  All rights reserved.
 *    - see licence.txt and licence_CAEN.txt for limitations on use.
 */
/*
 * HVCAENx527.c:
 * "private" routines for network interface to CAEN x527 HV power
 * supply crates.  These are essentially the interface routines for
 * EPICS driver and device support routines to the CAEN-supplied
 * library.
 */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <epicsThread.h>
#include <dbDefs.h>
#include <dbAccess.h>
#include <dbScan.h>
#include <recSup.h>
#include <recGbl.h>
#include <devSup.h>
#include <menuScan.h>
#include <errlog.h>
#ifdef _WIN32
#include <winsock2.h>
#include <time.h>
#include <sys/timeb.h>
#define snprintf _snprintf
#else
#include <sys/time.h>
#include <netdb.h>
#endif


#include <CAENHVWrapper.h>

#include <callback.h> /* needed later to replace definition of CALLBACK */

#include <epicsExport.h>

#include "HVCAENx527.h"

epicsShareExtern short DEBUG = 0;

short Busy[MAX_CRATES];
epicsThreadId scanThread;

HVCRATE Crate[MAX_CRATES];

static char *ParUnitStr[] = {
	"",
	"A",
	"V",
	"W",
	"C",
	"Hz",
	"Bar",
	"V/s",
	"s",
	"rpm",
	"counts"
};

static struct timeb Timer1, Timer2, Timer3, TimerLong;
float Period1, Period2, Period3, PeriodLong;

/* Device args in the db records have format:
   "<devaddr> <par>"
   where <par> is the name of any HV channel parameter that has an 
   associated record.
   Device support address for the CAEN x527 HV crates are formatted:
   R.SS.CCC
   where R = crate number, SS = slot number, CCC = channel number,
   left padded with zeros.
   Return value type depends on whether operation is a crate, slot, 
   or channel operation.
*/
/* Why on Earth didn't I use the same kind of syntax as the other
   drivers, i.e. "Ca Sb Nc @parm"???  Consider changing this before
   the software propagates.
   Too late?
*/
void *
CAENx527ParseDevArgs( char *saddr)
{
	int i;
	short cr, sl, ch;
	char pnm[32];
	short narg;
	HVCHAN *hvch;
	void *retp;

	if( saddr == NULL || strlen( saddr) < 8)
		return( NULL);

	narg = sscanf( saddr, "%hd.%hd.%hd %s", &cr, &sl, &ch, pnm);
PDEBUG(4) printf( "DEBUG: parsed dev args: %s -> %hd %hd %hd %s\n", saddr, cr, sl, ch, pnm);
	if( narg != 4)
	{
		printf( "ParseDevArgs: Not enough args.\n");
		return( NULL);
	}

	/* search the crate list for crate cr */
	for( i = 0; i < MAX_CRATES && Crate[i].hvchan != NULL && Crate[i].crate != cr; i++);
	if( i >= MAX_CRATES || Crate[i].hvchan == NULL)
	{
		printf( "ParseDevArgs: No crate found\n");
		return( NULL);
	}
	if( Crate[i].hvchmap == NULL)
	{
		printf( "ParseDevArgs: Crate empty.\n");
		return( NULL);
	}
	if( Crate[i].hvchmap[sl].hvchan[ch] == NULL)
	{
		printf( "ParseDevArgs: Crate %d, Slot %hd, empty.\n", i, sl);
		return( NULL);
	}

	hvch = Crate[i].hvchmap[sl].hvchan[ch];
	if( hvch == NULL)
		return( NULL);

	if( strcmp( pnm, "ChName") == 0)
	{
		retp = hvch;
	}
	else
	{
		/* Note: if parameter names need to be remapped, this loop will
		   be needed, so don't delete it, ru. */
		for( i = 0; i < hvch->npar && strcmp(hvch->pplist[i].pname, pnm) != 0; i++);
		if( i >= hvch->npar)
			return( NULL);
		retp = &(hvch->pplist[i]);
	}


	return( retp);
}

static void
ReadChParProp( char *name, unsigned short slot, unsigned short chan, char *pname, PARPROP *pp)
{
	CAENHVRESULT retval;

	/* get type parameter's type */
	retval = CAENHVGetChParamProp(name, slot, chan, pname, "Type", &(pp->Type));
	if( retval != CAENHV_OK)
	{
		printf( "CAENHVGetCHParamProp(): %s (%d)\n", CAENHVGetError( name), retval);
	}

	retval = CAENHVGetChParamProp(name, slot, chan, pname, "Mode", &(pp->Mode));
	if( retval != CAENHV_OK)
	{
		printf( "CAENHVGetCHParamProp(): %s (%d)\n", CAENHVGetError( name), retval);
	}

	if( pp->Type == PARAM_TYPE_NUMERIC)
	{
		retval = CAENHVGetChParamProp(name, slot, chan, pname, "Minval", &(pp->Minval));
		if( retval != CAENHV_OK)
		{
			printf( "CAENHVGetCHParamProp(): %s (%d)\n", CAENHVGetError( name), retval);
		}

		retval = CAENHVGetChParamProp(name, slot, chan, pname, "Maxval", &(pp->Maxval));
		if( retval != CAENHV_OK)
		{
			printf( "CAENHVGetCHParamProp(): %s (%d)\n", CAENHVGetError( name), retval);
		}

		retval = CAENHVGetChParamProp(name, slot, chan, pname, "Unit", &(pp->Unit));
		if( retval != CAENHV_OK)
		{
			printf( "CAENHVGetCHParamProp(): %s (%d)\n", CAENHVGetError( name), retval);
		}

		retval = CAENHVGetChParamProp(name, slot, chan, pname, "Exp", &(pp->Exp));
		if( retval != CAENHV_OK)
		{
			printf( "CAENHVGetCHParamProp(): %s (%d)\n", CAENHVGetError( name), retval);
		}

	}
	else if( pp->Type == PARAM_TYPE_ONOFF)
	{
		retval = CAENHVGetChParamProp(name, slot, chan, pname, "Onstate", pp->Onstate);
		if( retval != CAENHV_OK)
		{
			printf( "CAENHVGetCHParamProp(): %s (%d)\n", CAENHVGetError( name), retval);
		}

		retval = CAENHVGetChParamProp(name, slot, chan, pname, "Offstate", pp->Onstate);
		if( retval != CAENHV_OK)
		{
			printf( "CAENHVGetCHParamProp(): %s (%d)\n", CAENHVGetError( name), retval);
		}
	}
	else if( pp->Type == PARAM_TYPE_CHSTATUS)
	{
	}
	else if( pp->Type == PARAM_TYPE_BDSTATUS)
	{
	}
}

/* There is support for scanning all channels in a crate sequentially,
   on a slot by slot basis, and by parameter name. 
   cr->hvchan is the sequential list,
   cr->hvchmap is a slot X channel matrix (with variable channel numbers),
   cr->csl is the  list by parameter name.
 */
static void
InitCrate( HVCRATE *cr)
{
	int i, j, k, l;
	unsigned short nsl, *nch;
	char *model, *desc;
	unsigned short *sn;
	unsigned char *fmwrmin, *fmwrmax;
	char *str;
	char *par, (*parnamelist)[MAX_PARAM_NAME];
	int npar;
	HVCHAN *hvch;
	CAENHVRESULT retval;

	retval = CAENHVGetCrateMap( cr->name, &nsl, &nch, &model, &desc, &sn, &fmwrmin, &fmwrmax );
	if( retval != CAENHV_OK)
	{
		printf( "CAENHVGetCrateMap(): %s (%d)\n", CAENHVGetError( cr->name), retval);
		return;
	}
	else
	{
		cr->connected = 1;
		/* Set up cratemap */
		cr->hvchmap = (HVSLOT *)calloc( sizeof( HVSLOT), nsl + 1);
		if( cr->hvchmap)
		{
			cr->nsl = nsl;
			cr->nchan = 0;
			str = model;
			for( i = 0; i < nsl; i++)
			{
				if( str[0] != '\0')
				{
					snprintf( cr->hvchmap[i].slname, MAX_BOARD_NAME, "%s", str);
PDEBUG(1) printf( "DEBUG: model %s in slot %d\n", str, i);
					cr->hvchmap[i].nchan = nch[i];
					cr->hvchmap[i].hvchan = (HVCHAN **)calloc( sizeof( HVCHAN *), nch[i] + 1);
					if( cr->hvchmap[i].hvchan == NULL)
					{
						printf( "DEBUG: failed to allocate mem for channel list for slot %hd\n", i);
						return;
					}
					for( j = 0; j <= nch[i]; j++)
					{
						cr->hvchmap[i].hvchan[j] = NULL;
					}
					cr->nchan += nch[i];
				}
				else
				{
					cr->hvchmap[i].hvchan = NULL;
				}
				str += strlen( str) + 1;
			}
		}
		else
		{
			printf( "DEBUG: failed to allocate mem for cratemap for crate %hd\n", cr->crate);
			return;
		}
PDEBUG(1) printf( "DEBUG: InitCrate(): found %d slots, with total of %d channels\n", cr->nsl, cr->nchan);
		/* Build sequential channel list and fill cratemap */
		cr->hvchan = (HVCHAN *)calloc( sizeof(HVCHAN), cr->nchan);
		if( cr->hvchan == NULL)
		{
			printf( "InitCrate: Failed to calloc channel list\n");
			return;
		}

		k = 0;
		str = model;
		for( i = 0; i < nsl; i++)
		{
			if( str[0] != '\0')
			{
				/* get parameters for each channel
				   and put into respective PV fields */
				retval = CAENHVGetChParamInfo(cr->name, i, 0, &par);
				parnamelist = (char (*)[MAX_PARAM_NAME])par;
				if( retval != CAENHV_OK)
				{
					return;
				}

				for( j = 0; parnamelist[j][0]; j++);
				npar = j;
PDEBUG(3) printf( "DEBUG: number of parameters for each channel: %d\n", j);

				for( j = 0; j < nch[i]; j++)
				{
					hvch = &(cr->hvchan[k]);
					hvch->crate = &(cr->crate);
					hvch->slot = i;
					hvch->chan = j;
					hvch->epicsenabled = 0;
					hvch->npar = npar;
					snprintf( hvch->chaddr, 10, "%1d%02d%03d", cr->crate, hvch->slot, hvch->chan);

					hvch->pplist = (PARPROP *)calloc( sizeof(PARPROP), hvch->npar);
					if( hvch->pplist == NULL)
					{
						printf( "InitCrate: Failed to calloc parameter property list.\n");
						return;
					}
					cr->hvchmap[i].hvchan[j] = hvch;
					hvch->hvcrate = cr;

					for( l = 0; l < hvch->npar; l++)
					{
						strcpy( hvch->pplist[l].pname, parnamelist[l]);
PDEBUG(4) printf( "DEBUG: paramname %s\n", parnamelist[l]);
						ReadChParProp( cr->name, hvch->slot, hvch->chan, hvch->pplist[l].pname, &(hvch->pplist[l]));
						hvch->pplist[l].hvchan = hvch;
					}
					k++;
				}
/*				free( parnamelist); */
			}
			str += strlen( str) + 1;
		}
	}
/*	free( nch);
	free( model); 
	free( desc);
	free( sn);
	free( fmwrmin);
	free( fmwrmax); */
}

epicsShareFunc int
ConnectCrate( char *name, char *linkaddr)
{
	int i;
	CAENHVRESULT retval;

	if( name[0] == '\0' || linkaddr[0] == '\0')
	{
		printf( "ConnectCrate(): lacking crate name or address\n");
		return -1;
	}

	/* TCP/IP connection to crates */
	retval = CAENHVInitSystem( name, LINKTYPE_TCPIP, (void *)linkaddr, "admin", "admin");
	printf( "Connected to crate %s@%s\n", name, linkaddr);
	if( retval == CAENHV_OK)
	{
		i = 0;
		while( i < MAX_CRATES && Crate[i].hvchan != NULL) i++;
		if( i < MAX_CRATES)
		{
			Crate[i].crate = i;
			snprintf( Crate[i].name, 63, "%s", name);
			snprintf( Crate[i].IPaddr, 63, "%s", linkaddr);
			/* get list of boards */
			InitCrate( &(Crate[i]));
			return( 0);
		}
	}
	else
	{
		printf( "CAENHVInitSystem(): %s (%d)\n", CAENHVGetError( name), retval);
	}
	return( -1);
}

epicsShareFunc void
ParseCrateAddr( char (*straddr)[255], short naddr)
{
	int i, j;
	int crnlen;
	char *str;
	char crname[32], craddr[32], crip[32];
	struct hostent *hostip;

	if( straddr)
	{
		for( i = 0; i < naddr; i++)
		{
			str = straddr[i];
			crnlen = strlen( str);
			for( j = 0; j < crnlen && str[j] != '@'; j++);
			snprintf( crname, ++j, "%s", str);
			if( j < crnlen)
			{
				str += j;
				crnlen -= j;
				for( j = 0; j < crnlen && straddr[i][j] != ':'; j++);
				snprintf( craddr, ++j, "%s", str);
				hostip = gethostbyname( craddr);
				if( hostip->h_addrtype == AF_INET)
				{
			/* RU! There has to be a more elegant way of
			converting the IP to string... inet_*()? */
					snprintf( crip, 32, "%hd.%hd.%hd.%hd", (unsigned char)(hostip->h_addr[0]), (unsigned char)(hostip->h_addr[1]), (unsigned char)(hostip->h_addr[2]), (unsigned char)(hostip->h_addr[3]));
				}
				else
				{
					printf( "IPv6 addresses are not supported");
					exit( -1);
				}
PDEBUG(1) printf( "DEBUG: resolve %s -> %s (%s,%d,%d)\n", craddr, crip, hostip->h_name, (int)(hostip->h_addr), hostip->h_length);
			}
			if( j < crnlen)
			{
				str += j;
				crnlen -= j;
					printf( "slot option currently not supported: %s\n", str);
			}
			if( ConnectCrate( crname, crip) == 0)
			{
				printf( "Successfully connected to %s @ %s\n", crname, craddr);
			}
		}
	}
}

short
CAENx527GetConnectionStatus( short cr)
{
	short retval;

	if( ( cr < MAX_CRATES) && ( Crate[cr].hvchan))
		retval = Crate[cr].connected;
	else
		retval = 0;

	return( retval);
}

/* This function should be used sparingly, if at all. */
void *
CAENx527GetChParVal( PARPROP *pp)
{
	union
	{
		float f;
		long l;
	} value;
	HVCHAN *hvch;
	CAENHVRESULT retval;

	if( pp == NULL || pp->hvchan->epicsenabled == 0)
		return( NULL);

	hvch = pp->hvchan;

	while( Busy[*(hvch->crate)])
		epicsThreadSleep(0.01);

	/* get value of one parameter */
	Busy[*(hvch->crate)] = 1;
	if( pp->Type == PARAM_TYPE_NUMERIC)
	{
		retval = CAENHVGetChParam( hvch->hvcrate->name, hvch->slot, pp->pname, 1, (unsigned short *)&(hvch->chan), &(value.f));
		if( retval != CAENHV_OK)
			return NULL;
		pp->pval.f = value.f;
	}
	else
	{
		retval = CAENHVGetChParam( hvch->hvcrate->name, hvch->slot, pp->pname, 1, (unsigned short *)&(hvch->chan), &(value.l));
		if( retval != CAENHV_OK)
			return NULL;
		pp->pval.l = value.l;
	}
	Busy[*(hvch->crate)] = 0;

	/* We also pass the value to the calling routine because the final 
	   type and value often depends on the EPICS PV definition */
	return( &(pp->pval));
}

int
CAENx527SetChParVal( PARPROP *pp)
{
	HVCHAN *hvch;
	CAENHVRESULT retval;

	if( pp == NULL || pp->hvchan->epicsenabled == 0)
		return( 3);

	hvch = pp->hvchan;

	while( Busy[*(hvch->crate)])
		epicsThreadSleep( 0.01);

	/* set value of one parameter */
	Busy[*(hvch->crate)] = 1;
	if( pp->Type == PARAM_TYPE_NUMERIC)
	{
		retval = CAENHVSetChParam( hvch->hvcrate->name, hvch->slot, pp->pname, 1, (unsigned short *)&(hvch->chan), &(pp->pvalset.f));
	}
	else
	{
		retval = CAENHVSetChParam( hvch->hvcrate->name, hvch->slot, pp->pname, 1, (unsigned short *)&(hvch->chan), &(pp->pvalset.l));
	}
	Busy[*(hvch->crate)] = 0;

	if( retval != CAENHV_OK)
		return( 3);
	
	/* get feedback about setting in crate, so user can verify
	   setpoint was successfully set.
	   Note: this is prone to occasional lag at the crate. */
	if( CAENx527GetChParVal( pp) == NULL)
		return( 4);

	return( 0);
}

/* in the given crate, for all channels that have the given parameter, 
   and which have a PV, get their values in one call */
int
CAENx527GetAllChParVal( HVCRATE *cr, char *pname)
{
	int i, j;
	int nset;
	int pnum;
	short *chlist;
	union pval
	{
		float f;
		long l;
	} *pval;
	HVCHAN *hvch;
	CAENHVRESULT retval;
	int rval;

	if( cr == NULL || cr->hvchan == NULL || cr->connected == 0)
		return( 3);

	rval = 0;
	/* We have to do this one slot at a time since modules may vary */
	for( i = 0; i < cr->nsl; i++)
	{
		while( Busy[cr->crate])
			epicsThreadSleep( 0.01);

		Busy[cr->crate] = 1;
/* Note: allocating and building these lists repeatedly is not efficient.
   This should really be done once at init_record. */
		chlist = (short *)calloc( sizeof(short), cr->nchan);
		if( chlist == NULL)
		{
			printf( "GetAllChParVal: Failed to calloc channel list.\n");
			return( 3);
		}
		pval = (union pval *)calloc( sizeof(union pval), cr->nchan);
		if( pval == NULL)
		{
			printf( "GetAllChParVal: Failed to calloc value list.\n");
			return( 3);
		}
		/* Build the list of channels to change in this slot */
		nset = 0;
		pnum = 9999;
/* RU!  This degree of nested blocks is ridiculous! */
		if( cr->hvchmap[i].nchan)
		{
			hvch = cr->hvchmap[i].hvchan[0];
			for( j = 0; j < hvch->npar && strcmp( hvch->pplist[j].pname, pname) != 0; j++);
			pnum = j;
			for( j = 0; j < cr->hvchmap[i].nchan; j++)
			{
				hvch = cr->hvchmap[i].hvchan[0];
				if( hvch->epicsenabled)
				{
					if( pnum < hvch->npar)
					{
						chlist[nset] = j;
						nset++;
					}
				}
				else
				{
					pnum = hvch->npar;
				}
			}
			if( nset)
			{
				retval = CAENHVGetChParam( cr->name, i, pname, nset, chlist, pval);
				/* Now parse the return using the channel list for indeces */
				if( retval == CAENHV_OK)
				{
					for( j = 0; j < nset; j++)
					{
						hvch = cr->hvchmap[i].hvchan[chlist[j]];
						if( pnum < hvch->npar)
						{
							if( hvch->pplist[pnum].Type == PARAM_TYPE_NUMERIC)
								hvch->pplist[pnum].pval.f = pval[chlist[j]].f;
							else
								hvch->pplist[pnum].pval.l = pval[chlist[j]].l;
						/* RU!  Do we push the value to the PV or
						   let it scan for it? */
#if 0
							/* push value to PV VAL field */
							if( hvch->pplist[pnum].PVaddr.precord == NULL)
								dbNameToAddr( hvch->pplist[pnum].PVname, &(hvch->pplist[pnum].PVaddr));
							if( hvch->pplist[pnum].PVaddr.precord != NULL)
							{
								union
								{
									float f;
									long l;
									double d;
									short s;
								} pval;
								if( hvch->pplist[pnum].Type == PARAM_TYPE_NUMERIC)
								{
									pval.d = hvch->pplist[pnum].pval.f;
									dbPutField( &(hvch->pplist[pnum].PVaddr), DBR_DOUBLE, &(pval.d), 1);
								}
								else
								{
								}
							}
#endif
						}
					}
				}
				else if( retval == CAENHV_TIMEERR)
				{
					cr->connected = 0;
					printf( "Lost connection to %s@%s\n", Crate[i].name, Crate[i].IPaddr);
					CAENHVDeinitSystem( cr->name);
					rval = 4;
				}
				else
				{
					rval = 3;
				}
			}
		}
		else
		{
			rval = 3;
		}
		Busy[cr->crate] = 0;
		free( chlist);
		free( pval);
	}
	return( rval);
}

/* in the given crate, for all channels that have the given parameter, 
   and which have a PV, set them in one call, with the given value */
int
CAENx527SetAllChParVal( HVCRATE *cr, char *pname, void *val)
{
	int i, j, k;
	int nset;
	short *chlist;
	union pval
	{
		float f;
		long l;
		double d;
	} *pval;
	HVCHAN *hvch;
	CAENHVRESULT retval;

	if( cr == NULL || cr->hvchan == NULL || cr->connected == 0)
		return( 3);

	/* We have to do this one slot at a time since modules may vary */
	for( i = 0; i < cr->nsl; i++)
	{
		while( Busy[cr->crate])
			epicsThreadSleep( 0.01);

		Busy[cr->crate] = 1;

/* Note: allocating and building these lists repeatedly is not efficient.
   This should really be done once at init_record. */
		chlist = (short *)calloc( sizeof(short), cr->nchan);
		if( chlist == NULL)
		{
			printf( "SetAllChParVal: Failed to calloc channel list.\n");
			return( 3);
		}
		pval = (union pval *)calloc( sizeof(union pval), cr->nchan);
		if( pval == NULL)
		{
			printf( "SetAllChParVal: Failed to calloc value list.\n");
			return( 3);
		}
		/* Build the list of channels to change in this slot,
		   and store the value to send */
		nset = 0;
		for( j = 0; j < cr->hvchmap[i].nchan; j++)
		{
			hvch = cr->hvchmap[i].hvchan[j];
			if( hvch->epicsenabled)
			{
				for( k = 0; k < hvch->npar && strcmp( hvch->pplist[k].pname, pname) != 0; k++);
				if( k < hvch->npar)
				{
					chlist[nset] = j;
					if( hvch->pplist[k].Type == PARAM_TYPE_NUMERIC)
						pval[nset].f = hvch->pplist[k].pval.f;
					else
						pval[nset].l = hvch->pplist[k].pval.l;
					nset++;
				}
			}
		}
		retval = CAENHVSetChParam( cr->name, i, pname, nset, chlist, pval);
		Busy[cr->crate] = 0;
		free( chlist);
		free( pval);
		if( retval != CAENHV_OK)
		{
			cr->connected = 0;
			return( 3);
		}
	}
	return( 0);
}

/* in the given crate, for all channels which have a PV, get their 
   channel name in one call */
/* RU!  Why didn't you make channel name another channel parameter? 
   Complications from chname list? */
int
CAENx527GetAllChName( HVCRATE *cr)
{
	int i, j;
	int nset;
	short *chlist;
	char (*chname)[MAX_CH_NAME];
	HVCHAN *hvch;
	CAENHVRESULT retval;

	if( cr == NULL || cr->hvchan == NULL)
		return( 3);

	/* We have to do this one slot at a time since modules may vary */
	for( i = 0; i < cr->nsl; i++)
	{
		while( Busy[cr->crate])
			epicsThreadSleep( 0.01);

		Busy[cr->crate] = 1;

/* Note: allocating and building these lists repeatedly is not efficient.
   This should really be done once at init_record. */
		chlist = (short *)calloc( sizeof(short), cr->nchan);
		if( chlist == NULL)
		{
			printf( "GetAllChName: Failed to calloc channel list.\n");
			return( 3);
		}
		chname = calloc( sizeof(char)*MAX_CH_NAME, cr->nchan);
		if( chname == NULL)
		{
			printf( "GetAllChName: Failed to calloc channel name.\n");
			return( 3);
		}
		/* Build the list of channels to change in this slot */
		nset = 0;
		for( j = 0; j < cr->hvchmap[i].nchan; j++)
		{
			hvch = cr->hvchmap[i].hvchan[j];
			if( hvch->epicsenabled)
			{
				chlist[nset] = j;
				nset++;
			}
		}
		retval = CAENHVGetChName( cr->name, i, nset, chlist, chname);
		if( retval == CAENHV_OK)
		{
			for( j = 0; j < nset; j++)
			{
				hvch = cr->hvchmap[i].hvchan[chlist[j]];
				snprintf( hvch->chname, MAX_CH_NAME, "%s", chname[chlist[j]]);
				/* RU!  Do we push the value to the PV or
				   let it scan for it? */
#if 0
				/* push value to PV VAL field */
				if( hvch->PVaddr.precord == NULL)
					dbNameToAddr( hvch->pplist[pnum].PVname, &(hvch->PVaddr));
				if( hvch->PVaddr.precord != NULL)
				{
					dbPutField( &(hvch->PVaddr), DBR_DOUBLE, hvch->chname, 1);
				}
#endif
			}
		}
		Busy[cr->crate] = 0;
		free( chlist);
		free( chname);
		if( retval != CAENHV_OK)
			return( 3);
	}
	return( 0);
}

/* Convert a multibit binary word to a state value.  Selects the
   highest order bit if it is a recognized type, otherwise just
   passes the raw value.  This does not extract the bit pattern.
 */
short
CAENx527mbbi2state( PARPROP *pp)
{
	int i;
	short oval;

	if( pp->Type == PARAM_TYPE_CHSTATUS || pp->Type == PARAM_TYPE_BDSTATUS)
	{
		oval = 0;
		for( i = 0; i < 16; i++)
		{
			if( ( pp->pval.l >> i) & 0x1)
				oval = i + 1;
		}
	}
	else
	{
		oval = (short)(pp->pval.l);
	}

	return( oval);
}

/* Convert a multibit binary word to an array of single bit variables,
   each array element holding a 0 or 1 for each bit of the word.
 */
void
CAENx527mbbi2bits( PARPROP *pp, char *bits, short nbits)
{
	int i;

	if( pp->Type == PARAM_TYPE_CHSTATUS || pp->Type == PARAM_TYPE_BDSTATUS)
	{
		for( i = 0; i < nbits; i++)
		{
			bits[i] = ( pp->pval.l >> i) & 0x1;
		}
	}
	else
	{
		for( i = 0; i < nbits; i++)
			bits[i] = 0;
	}

	return;
}

char *
CAENx527GetChName( HVCHAN *hvch)
{
	char chname[MAX_CH_NAME];
	CAENHVRESULT retval;

	if( hvch == NULL || hvch->hvcrate == NULL || hvch->epicsenabled == 0)
		return( NULL);

	while( Busy[*(hvch->crate)])
		epicsThreadSleep( 0.01);

	Busy[*(hvch->crate)] = 1;
	retval = CAENHVGetChName( hvch->hvcrate->name, hvch->slot, 1, &(hvch->chan), &chname);
	Busy[*(hvch->crate)] = 0;

	if( retval != CAENHV_OK)
		return( NULL);

	snprintf( hvch->chname, MAX_CH_NAME, "%s", chname);

	return( hvch->chname);
}

int
CAENx527SetChName( HVCHAN *hvch, char *chname)
{
	CAENHVRESULT retval;

	if( hvch == NULL || hvch->hvcrate == NULL || hvch->epicsenabled == 0)
		return( 3);

	while( Busy[*(hvch->crate)])
		epicsThreadSleep( 0.01);

	Busy[*(hvch->crate)] = 1;
	snprintf( hvch->chname, MAX_CH_NAME, "%s", chname);
	retval = CAENHVSetChName( hvch->hvcrate->name, hvch->slot, 1, &(hvch->chan), hvch->chname);
	Busy[*(hvch->crate)] = 0;

	if( retval != CAENHV_OK)
		return( 3);

	return( 0);
}

/* Get the engineering units the crate is using for this parameter
   and fill a user-defined char string with it */
char *
CAENx527GetParUnit( PARPROP *pp, char *fieldval)
{
	char exp[8];

	strcpy( exp, "");
	if( pp->Exp)
	{
		if( pp->Exp == 6)
			strcpy( exp, "M\0");
		else if( pp->Exp == 3)
			strcpy( exp, "k\0");
		else if( pp->Exp == -3)
			strcpy( exp, "m\0");
		else if( pp->Exp == -6)
			strcpy( exp, "u\0");
PDEBUG(4) printf( "DEBUG: EGU scale %d -> %s\n", pp->Exp, exp);
	}
			
	snprintf( fieldval, 64, "%s%s", exp, ParUnitStr[pp->Unit]);

	return( fieldval);
}

epicsShareFunc void
Shutdown()
{
	int i, j;
	CAENHVRESULT retval;

	i = 0;
	while( i < MAX_CRATES && Crate[i].hvchan != NULL)
	{
		printf( "DEBUG: shutting down crate %d\n", i);
		retval = CAENHVDeinitSystem( Crate[i].name);
		if( retval != CAENHV_OK)
		{
			printf( "Shutdown: could not disconnect from crate %s\n", Crate[i].name);
		}
		else
		{
			for( j = 0; j < Crate[i].nsl; j++)
			{
				free( Crate[i].hvchmap[j].hvchan);
			}
			free( Crate[i].hvchmap);
			for( j = 0; j < Crate[i].nchan; j++)
			{
				free( Crate[i].hvchan[j].pplist);
			}
			free( Crate[i].hvchan);
			printf( "Shutdown: successfully disconnected from crate %s\n", Crate[i].name);
		}
		i++;
	}
}

#include <signal.h>

void
SigShutdownHandler( int signal)
{
#ifndef _WIN32
	if( signal < sizeof( sys_siglist))
		printf( "DEBUG: Caught a termination signal (%s).\n", sys_siglist[signal]);
	else
		return;
#endif
	Shutdown();
	exit(0);
}

epicsShareFunc void
SetSigShutdownHandler(void)
{
#ifndef _WIN32
	struct sigaction action;

	action.sa_handler = SigShutdownHandler;
	action.sa_flags = SA_ONESHOT | SA_NOCLDSTOP;
	(void)sigaction( SIGHUP, &action, NULL);
	(void)sigaction( SIGINT, &action, NULL);
	(void)sigaction( SIGQUIT, &action, NULL);
	(void)sigaction( SIGTERM, &action, NULL);
#endif
}

#if 0
void
iCallback( CALLBACK *pcallback)
{
	dbCommon *pior;
	struct rset *prset;

	callbackGetUser( pior, pcallback);
	prset = (struct rset *)pior->rset;
	dbScanLock( pior);
	(*prset->process)(pior);
	dbScanUnlock( pior);
}

void
oCallback( CALLBACK *pcallback)
{
	dbCommon *pior;
	struct rset *prset;

	callbackGetUser( pior, pcallback);
	prset = (struct rset *)pior->rset;
	dbScanLock( pior);
	(*prset->process)(pior);
	dbScanUnlock( pior);
}
#endif

#include <registryFunction.h>
#include <epicsExport.h>
#include <subRecord.h>
#include <drvSup.h>

/* RU!  I think the code below will be or already is deprecated */
#if 0
/* Read all values associated with the given channel */
static void
ReadChannel( char *name, HVCHAN *hvch)
{
	int i;
	CAENHVRESULT retval;

	for( i = 0; i < hvch->npar; i++)
	{
		/* get value of one parameter */
/*printf( "DEBUG: %d %d scan par %d\n", hvch->slot, hvch->chan, i);*/
		if( hvch->pplist[i].Type == PARAM_TYPE_NUMERIC)
		{
			retval = CAENHVGetChParam( name, hvch->slot, hvch->pplist[i].pname, 1, &(hvch->chan), &(hvch->pplist[i].pval.f));
		}
		else
		{
			retval = CAENHVGetChParam( name, hvch->slot, hvch->pplist[i].pname, 1, &(hvch->chan), &(hvch->pplist[i].pval.l));
		}

		/* put value into respective PV VAL field */
		if( retval != CAENHV_OK)
		{
		}
		else
		{
			if( strcasecmp( hvch->pplist[i].pname, "Vmon") == 0)
			{
				hvch->pplist[i].pval.d = (double)hvch->pplist[i].pval.f;
				if( hvch->pplist[i].PVaddr.precord == NULL)
					dbNameToAddr( hvch->pplist[i].PVname, &(hvch->pplist[i].PVaddr));
/*printf( "DEBUG: PV info: %s (%x)\n", hvch->pplist[i].PVname, &(hvch->pplist[i].PVaddr));*/
				if( hvch->pplist[i].PVaddr.precord != NULL)
					dbPutField( &(hvch->pplist[i].PVaddr), DBR_DOUBLE, &(hvch->pplist[i].pval.d), 1);
/*printf( "DEBUG: measured voltage (%d) = %f\n", hvch->chan, hvch->pplist[i].pval.f);*/
			}
		}
	}
}
#endif

void
ScanChannels()
{
	int i;
	CRATESCANLIST *csl;
	struct timeb tnow;
	float telapsed;
	short lapsed1, lapsed2, lapsed3, lapsedLong;
	char HvPwSM[64];
	CAENHVRESULT retval;

	lapsed1 = 0;
	lapsed2 = 0;
	lapsed3 = 0;
	lapsedLong = 0;
    ftime(&tnow);
	telapsed = ( tnow.time - Timer1.time) + ( tnow.millitm - Timer1.millitm) / 1.0e3f;
	if( telapsed > Period1)
	{
PDEBUG(4) printf( "DEBUG: lapsed 1 %f\n", telapsed);
		lapsed1 = 1;
		Timer1.time = tnow.time;
		Timer1.millitm = tnow.millitm;
	}
	telapsed = ( tnow.time - Timer2.time) + ( tnow.millitm - Timer2.millitm) / 1.0e3f;
	if( telapsed > Period2)
	{
PDEBUG(4) printf( "DEBUG: lapsed 2 %f\n", telapsed);
		lapsed2 = 1;
		Timer2.time = tnow.time;
		Timer2.millitm = tnow.millitm;
	}
	telapsed = ( tnow.time - Timer3.time) + ( tnow.millitm - Timer3.millitm) / 1.0e3f;
	if( telapsed > Period3)
	{
PDEBUG(4) printf( "DEBUG: lapsed 3 %f\n", telapsed);
		lapsed3 = 1;
		Timer3.time = tnow.time;
		Timer3.millitm = tnow.millitm;
	}
	telapsed = ( tnow.time - TimerLong.time) + ( tnow.millitm - TimerLong.millitm) / 1.0e3f;
	if( telapsed > PeriodLong)
	{
PDEBUG(4) printf( "DEBUG: lapsed Long %f\n", telapsed);
		lapsedLong = 1;
		TimerLong.time = tnow.time;
		TimerLong.millitm = tnow.millitm;
	}

	for( i = 0; i < MAX_CRATES && Crate[i].hvchan != NULL; i++)
	{
PDEBUG(4) printf( "DEBUG: scanning crate %d\n", i);
		
		/* crate connection check */
		if( Crate[i].connected == 1)
		{
			if( lapsed1)
			{
				while( Busy[Crate[i].crate])
					epicsThreadSleep( 0.01);

				Busy[Crate[i].crate] = 1;
				retval = CAENHVGetSysProp( Crate[i].name, "HvPwSM", HvPwSM);
				Busy[Crate[i].crate] = 0;
				/* If we lose the connection, the crate will
				   log us out, but if we misinterpreted we
				   have to force a logout. */
				if( retval == CAENHV_TIMEERR)
				{
					Crate[i].connected = 0;
					printf( "Lost connection to %s@%s\n", Crate[i].name, Crate[i].IPaddr);
					CAENHVDeinitSystem( Crate[i].name);
				}
			}
		}
		else
		{
			if( lapsedLong)
			{
				/* If we lose the connection, we have to log
				   back in. */
#if 1
				retval = CAENHVInitSystem( Crate[i].name, LINKTYPE_TCPIP, (void *)(Crate[i].IPaddr), "admin", "admin");
				if( retval == CAENHV_OK)
				{
					Crate[i].connected = 1;
					printf( "Regained connection to %s@%s\n", Crate[i].name, Crate[i].IPaddr);
				}
#else
/* Eventually, you should actually rebuild the raw data tables,
   unless you find the device support is gibbled in the process.
   Don't forget to free() pointers, i.e. need to do some sort of
   clear().*/
				if( ConnectCrate( Crate[i].name, Crate[i].IPaddr) == 0)
				{
					Crate[i].connected = 1;
					printf( "Regained connection to %s@%s\n", Crate[i].name, Crate[i].IPaddr);
				}
#endif
			}
		}
		if( Crate[i].connected == 1)
		{
			for( csl = Crate[i].csl; csl->next != NULL; csl = csl->next)
			{
				/* check timer */
				if( csl->period >= Period3) 
				{
					if ( lapsed3)
					{
						CAENx527GetAllChParVal( &(Crate[i]), csl->pname);
PDEBUG(6) printf( "DEBUG: scanning crate %d for %s\n", i, csl->pname);
					}
				}
				else if( csl->period >= Period2)
				{
					if( lapsed2)
					{
						CAENx527GetAllChParVal( &(Crate[i]), csl->pname);
PDEBUG(6) printf( "DEBUG: scanning crate %d for %s\n", i, csl->pname);
					}
				}
				else
				{
					if( lapsed1)
					{
						CAENx527GetAllChParVal( &(Crate[i]), csl->pname);
PDEBUG(6) printf( "DEBUG: scanning crate %d for %s\n", i, csl->pname);
					}
				}
			}
			/* ChName needs to be handled separately */
#if 1
			if( Crate[i].hvchan[0].chname_period >= Period3) 
			{
				if( lapsed3)
					CAENx527GetAllChName( &(Crate[i]));
			}
			else if( Crate[i].hvchan[0].chname_period >= Period2)
			{
				if( lapsed2)
					CAENx527GetAllChName( &(Crate[i]));
			}
			else
			{
				if( lapsed1)
					CAENx527GetAllChName( &(Crate[i]));
			}
#else
			if( lapsed3)
				CAENx527GetAllChName( &(Crate[i]));
#endif
		}
	}

	if( lapsed1)
		post_event( EVNTNO_T1);
	if( lapsed2)
		post_event( EVNTNO_T2);
	if( lapsed3)
		post_event( EVNTNO_T3);
}

void
InitScanChannels()
{
	int i, j, k, m;
	CRATESCANLIST *csl;
	HVCHAN *hvch;

	/* set up scan lists by parameter name, for each crate */
	for( i = 0;  i < MAX_CRATES && Crate[i].hvchan != NULL; i++)
	{
		Crate[i].csl = (CRATESCANLIST *)calloc( sizeof(CRATESCANLIST), 1);
		if( Crate[i].csl == NULL)
			printf( "InitScanChannels: Failed to calloc crate scanlist\n");
		for( j = 0; j < Crate[i].nsl; j++)
		{
PDEBUG(5) printf( "DEBUG: slot name (%d out of %d): %s\n", j, Crate[i].nsl, Crate[i].hvchmap[j].slname);
			if( Crate[i].hvchmap[j].slname[0] != '\0')
			{
				hvch = Crate[i].hvchmap[j].hvchan[0];
				for( k = 0; k < hvch->npar; k++)
				{
					for( csl = Crate[i].csl; csl->next != NULL && strcmp( csl->pname, hvch->pplist[k].pname) != 0; csl = csl->next);
PDEBUG(6) printf( "DEBUG: csl->next = %#x ?= NULL, '%s' ?= '%s'\n", (unsigned)csl->next, csl->pname, hvch->pplist[k].pname);
					if( csl->next == NULL)
					{
						csl->next = (CRATESCANLIST *)calloc( sizeof(CRATESCANLIST), 1);
						if( csl->next == NULL)
							printf( "InitScanChannels: Failed to calloc next crate scanlist\n");
						strcpy( csl->pname, hvch->pplist[k].pname);
						/* RU!  This should really
						default to the longest period
						and be explicit with shorter
						period parameters */
						csl->period = 30.0;
						csl->evntno = EVNTNO_T3;

						/* Default scan period setup */
						if( ( strcmp( csl->pname, "Status") == 0) ||
							( strcmp( csl->pname, "Pw") == 0) ||
							( strcmp( csl->pname, "VMon") == 0) ||
							( strcmp( csl->pname, "IMon") == 0)
						)
						{
				/* This is very short because the values
				   are dynamic */
							csl->period = 0.5;
							csl->evntno = EVNTNO_T1;
						}
						else if(
							( strcmp( csl->pname, "V0Set") == 0) ||
							( strcmp( csl->pname, "I0Set") == 0) ||
							( strcmp( csl->pname, "V1Set") == 0) ||
							( strcmp( csl->pname, "I1Set") == 0)
						)
						{
				/* This is short because we want to be
				   sure the associated variable gets set */
							csl->period = 5.0;
							csl->evntno = EVNTNO_T2;
						}
					}
					if( ( csl->next == NULL) || ( strcmp( csl->pname, hvch->pplist[k].pname) == 0))
					{
						for( m = 0; m < Crate[i].hvchmap[j].nchan; m++)
						{
							hvch[m].pplist[k].period = csl->period;
							hvch[m].pplist[k].evntno = csl->evntno;
PDEBUG( 7) printf( "DEBUG: %s: event #: %d\n", csl->pname, hvch[m].pplist[k].evntno);
						}
					}
				}
				/* channel name must be handled separately */
				for( m = 0; m < Crate[i].hvchmap[j].nchan; m++)
				{
					hvch[m].chname_period = 10.0;
					hvch[m].chname_evntno = EVNTNO_T3;
				}
			}
		}
	}

	/* Set up timers */
	Period1 = 0.5;
	Period2 = 5.0;
	Period3 = 10.0;
	PeriodLong = 30.0;
	ftime( &Timer1);
	Timer2.time = Timer1.time;
	Timer2.millitm = Timer1.millitm;
	Timer3.time = Timer1.time;
	Timer3.millitm = Timer1.millitm;
	TimerLong.time = Timer1.time;
	TimerLong.millitm = Timer1.millitm;
}

#if 1

void
ScanChannels_Thread( void *param)
{
	while( 1)
	{
		ScanChannels();
		epicsThreadSleep( ScanChannelsPeriod);
	}
}

static long
init()
{
	errlogPrintf("Starting CAEN x527 driver\n");

	ScanChannelsPeriod = 0.2f;
	InitScanChannels();
	scanThread = epicsThreadCreate( "HVCAENx527Thread", epicsThreadPriorityMedium + 5, epicsThreadGetStackSize(epicsThreadStackBig), ScanChannels_Thread, NULL);

	return( 0);
}

static long
report( int level)
{
	return( 0);
}

struct
{
	long number;
	DRVSUPFUN	report;
	DRVSUPFUN	init;
} drvCAENx527 =
	{
		2,
		report,
		init
	};
epicsExportAddress( drvet, drvCAENx527);
#endif

#if 0
epicsRegisterFunction( InitScanChannels);
epicsRegisterFunction( ScanChannels);
#endif

/*
 * $Log: HVCAENx527/HVCAENx527App/src/HVCAENx527.c  $
 * Revision 1.20 2007/06/01 13:32:57CST Ru Igarashi (igarasr) 
 * Member moved from EPICS/HVCAENx527App/src/HVCAENx527.c in project e:/MKS_Home/archive/cs/epics_local/drivers/CAENx527HV/project.pj to HVCAENx527/HVCAENx527App/src/HVCAENx527.c in project e:/MKS_Home/archive/cs/epics_local/drivers/CAENx527HV/project.pj.
 */