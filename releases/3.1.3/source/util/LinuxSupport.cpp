//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose:
//
// $NoKeywords: $  
//=============================================================================
#ifdef LINUX

#include "util/LinuxSupport.h"

int strlwr(char *src)
{
	while(*src!='\0')
	{
		*src=tolower(*src);
		src++;
	}
	return 1;
}

char selectBuf[PATH_MAX];

int FileSelect(const struct dirent *ent)
{
	const char *mask=selectBuf;
	const char *name=ent->d_name;

	//printf("Test:%s %s\n",mask,name);

	if(!strcmp(name,".") || !strcmp(name,"..") ) return 0;

	if(!strcmp(selectBuf,"*.*")) return 1;

	while( *mask && *name )
	{
		if(*mask=='*')
		{
			mask++; // move to the next char in the mask
			if(!*mask) // if this is the end of the mask its a match 
			{
				return 1;
			}
			while(*name && toupper(*name)!=toupper(*mask)) 
			{ // while the two don't meet up again
				name++;
			}
			if(!*name) 
			{ // end of the name
				break; 
			}
		}
		else if (*mask!='?')
		{
			if( toupper(*mask) != toupper(*name) )
			{	// mismatched!
				return 0;
			}
			else
			{	
				mask++;
				name++;
				if( !*mask && !*name) 
				{ // if its at the end of the buffer
					return 1;
				}
				
			}

		}
		else /* mask is "?", we don't care*/
		{
			mask++;
			name++;
		}
	}	
		
	return( !*mask && !*name ); // both of the strings are at the end
}

int FillDataStruct(FIND_DATA *dat)
{
	struct stat fileStat;

	if(dat->numMatches<0)
		return -1;

	strcpy(dat->cFileName,dat->namelist[dat->numMatches]->d_name);

	if(!stat(dat->cFileName,&fileStat))
	{
		dat->dwFileAttributes=fileStat.st_mode;           
	}
	else
	{
		dat->dwFileAttributes=0;
	}	
	//printf("%s\n", dat->namelist[dat->numMatches]->d_name);
      	free(dat->namelist[dat->numMatches]);

  	dat->numMatches--;
	return 1;
}


int FindFirstFile(const char *fileName, FIND_DATA *dat)
{
	char nameStore[PATH_MAX];
	char *dir=NULL;
	int n,iret=-1;
	
	strcpy(nameStore,fileName);

	if(strrchr(nameStore,'/') )
	{
		dir=nameStore;
		while(strrchr(dir,'/') )
		{
			struct stat dirChk;

			// zero this with the dir name
			dir=strrchr(nameStore,'/');
			*dir='\0';

			dir=nameStore;
			stat(dir,&dirChk);
		
			if( S_ISDIR( dirChk.st_mode ) )
			{
				break;	
			}
		}
	}
	else
	{
		// couldn't find a dir seperator...
		return -1;
	}

	if( strlen(dir)>0 )
	{
		strcpy(selectBuf,fileName+strlen(dir)+1);

		n = scandir(dir, &dat->namelist, FileSelect, alphasort);
           	if (n < 0)
           	    perror("scandir");
          	else 
		{
			dat->numMatches=n-1; // n is the number of matches
			iret=FillDataStruct(dat);
			if(iret<0)
			{
				free(dat->namelist);
			}

		}
	}

	//printf("Returning: %i \n",iret);
	return iret;
}

bool FindNextFile(int handle, FIND_DATA *dat)
{
	if(dat->numMatches<0)
	{	
		free(dat->namelist);
		return false; // no matches left
	}	

	FillDataStruct(dat);
	return true;
}

bool FindClose(int handle)
{
	return true;
}

#endif
