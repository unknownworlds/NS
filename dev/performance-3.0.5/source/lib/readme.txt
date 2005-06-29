This directory holds library files required for building the client and server. 

There are two types of libraries being stored here: libraries built by the 
Natural_Selection project, which aren't checked into source control, and 
libraries built with external makefiles, which are source controlled.

The "debug" and "release" subdirectories are targets for project builds, and any 
libraries in those directories should *not* be placed under source control.  Any 
libraries "external_debug" and "external_release" are built outside of the 
repository, and *do* belong in source control.