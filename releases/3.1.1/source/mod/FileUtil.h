#ifndef NS_FILEUTIL_H
#define NS_FILEUTIL_H

#include "types.h"
#include "util/CString.h"

bool FUBuildFileList(const string& inBaseDirectoryName, const string& inDirectoryName, const string& inFileExtension, CStringList& outList);

#endif