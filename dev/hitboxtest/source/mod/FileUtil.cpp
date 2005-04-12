#include "mod/FileUtil.h"
#include "localassert.h"
#include "util/nowarnings.h"
#include <stdio.h>
#include "stdarg.h"

// For FindFirst/FindNext functionality in BuildFileList
#ifdef WIN32
	#include "windows.h"
#else
	#include "util/LinuxSupport.h"
#endif

#include "public/interface.h"
#include "public/FileSystem.h"

IFileSystem* FUGetFileSystem()
{
	static IFileSystem* sFileSystem = NULL;

	if(!sFileSystem)
	{
		CSysModule *theFilesystem = Sys_LoadModule("FileSystem_Stdio");
		
		CreateInterfaceFn factory = Sys_GetFactory(theFilesystem);

		void* theRetVal = factory(FILESYSTEM_INTERFACE_VERSION, NULL );
		if(!theRetVal)
		{
			// Error
			int a = 0;
		}
		sFileSystem = (IFileSystem *)theRetVal;
	}
	
	ASSERT(sFileSystem);

	return sFileSystem;
}

// Pass in relative path, do search on path including mod directory, return files relative to mod directory
bool FUBuildFileList(const string& inBaseDirectoryName, const string& inDirectoryName, const string& inFileExtension, CStringList& outList)
{
	#ifdef WIN32
	const string kDelimiter("\\");
	#else
	const string kDelimiter("/");
	#endif

	bool theSuccess = false;

	string theBaseDirectoryName = inBaseDirectoryName;
	string theDirectoryName = inDirectoryName;

	#ifdef WIN32
	// Replace all forward slashes with \\'s if needed
	std::replace(theBaseDirectoryName.begin(), theBaseDirectoryName.end(), '/', '\\');
	std::replace(theDirectoryName.begin(), theDirectoryName.end(), '/', '\\');
	#endif
	
	string theFullDirName = theBaseDirectoryName + theDirectoryName;
	
	int theEndOffset = theDirectoryName.find_last_of(kDelimiter);
	string theBaseDirName = theDirectoryName.substr(0, theEndOffset);

	theFullDirName += inFileExtension;

	#ifdef WIN32

	FileFindHandle_t theHandle;
	const char* theCStrFoundFileName = FUGetFileSystem()->FindFirst(theFullDirName.c_str(), &theHandle);
	if(theCStrFoundFileName)
	{
		do
		{
			string theFoundFilename = string(theCStrFoundFileName);

	#else

	string theFoundFilename;
	FIND_DATA theFindData;

	int theRC = FindFirstFile(theFullDirName.c_str(), &theFindData);
	if(theRC != -1)
	{
		do
		{
			string theFoundFilename = string(theFindData.cFileName);
	#endif


			CString theCString;
			string theFullFileName = theBaseDirName + string("/") + theFoundFilename;

			// Precache requires / in the filename
			std::replace(theFullFileName.begin(), theFullFileName.end(), '\\', '/');

			theCString = theFullFileName;
			outList.push_back(theCString);
			theSuccess = true;



	#ifdef WIN32
		} 
		while((theCStrFoundFileName = FUGetFileSystem()->FindNext(theHandle)) != NULL);
	}
	#else

		}
		while(FindNextFile(0, &theFindData));
	}

	#endif
	
	
	//DIR theDirp = opendir(theDirName.c_str());
	//	while(theDirp) 
	//	{
	//		int theErrno = 0;
	//		     if ((dp = readdir(theDirp)) != NULL) {
	//			         if (strcmp(dp->d_name, name) == 0) {
	//				             closedir(theDirp);
	//				             return FOUND;
	//				         }
	//			     } else {
	//				         if (theErrno == 0) {
	//					             closedir(theDirp);
	//					             return NOT_FOUND;
	//					         }
	//				         closedir(theDirp);
	//				         return READ_ERROR;
	//				     }
	//			 }
	//	return OPEN_ERROR;

	return theSuccess;
}
