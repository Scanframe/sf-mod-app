#!/bin/bash
###
### This script is Qt version and directory locations specific.
###

#set -x

# Writes to stderr.
#
function WriteLog()
{
	echo "$@" 1>&2;
}

##
## Install only 64bit compilers.
##

#sudo apt install gcc-mingw-w64-x86-64 g++-mingw-w64-x86-64

# Directory where the Linux Qt library cmake files are located.
DIR_FROM="${HOME}/lib/Qt/6.2.0/gcc_64/lib/cmake"
# Root for the Windows Qt installed MinGW files.
ROOT_DIR="/mnt/server/userdata/project/PROG/Qt/6.2.0/mingw81_64"
# Directory where the Windows Qt library cmake files are located.
DIR_TO="${ROOT_DIR}/lib/cmake"

##
## Create symlink in from ~/lib/Qt/6.2.0/gcc_64/libexec to ${ROOT_DIR}
##

# ln -s "${HOME}/lib/Qt/6.2.0/gcc_64/libexec" "${ROOT_DIR}/libexec"

##
## Replace all cmake files referencing windows EXE-tools.
## Found using (could be automated):
## 	find /mnt/server/userdata/project/PROG/Qt/6.2.0/mingw81_64 -type f -name "*.cmake" -exec grep -li "\.exe" {} \;
##

declare -a MODULES=(
	"Qt6AxContainerTools/Qt6AxContainerToolsTargets-relwithdebinfo.cmake"
	"Qt6AxServerTools/Qt6AxServerToolsTargets-relwithdebinfo.cmake"
	"Qt6DBusTools/Qt6DBusToolsTargets-relwithdebinfo.cmake"
	"Qt6GuiTools/win/Qt6GuiToolsTargets-relwithdebinfo.cmake"
	"Qt6LinguistTools/Qt6LinguistToolsTargets-relwithdebinfo.cmake"
	"Qt6QmlTools/Qt6QmlToolsTargets-relwithdebinfo.cmake"
	"Qt6RemoteObjectsTools/Qt6RemoteObjectsToolsTargets-relwithdebinfo.cmake"
	"Qt6ScxmlTools/Qt6ScxmlToolsTargets-relwithdebinfo.cmake"
	"Qt6SerialBusTools/Qt6SerialBusToolsTargets-relwithdebinfo.cmake"
	"Qt6ToolsTools/Qt6ToolsToolsTargets-relwithdebinfo.cmake"
	"Qt6WidgetsTools/Qt6WidgetsToolsTargets-relwithdebinfo.cmake"
)
# Iterate the string array using for loop
for fn in "${MODULES[@]}" ; do
	WriteLog "File: ${fn}"
	cp "${DIR_FROM}/${fn}" "${DIR_TO}/${fn}"
done

