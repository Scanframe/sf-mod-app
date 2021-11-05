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

#sudo apt install gcc-mingw-w64-x86-64 g++-mingw-w64-x86-64 gdb-mingw-w64

# Directory of this script.
SCRIPT_DIR="$(cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd)"
# Root for the Windows Qt installed MinGW files.
ROOT_DIR="/mnt/server/userdata/project/PROG/Qt"
# Get the Qt installed directory.
QT_LIB_DIR="$(bash "${SCRIPT_DIR}/com/cmake/QtLibDir.sh")"
# Qt version on Linux.
QT_VER="$(basename "${QT_LIB_DIR}")"
# Directory where the Linux Qt library cmake files are located.
DIR_FROM="${QT_LIB_DIR}/gcc_64/lib/cmake"
# Directory where the Windows Qt library cmake files are located.
DIR_TO="${ROOT_DIR}/${QT_VER}/mingw81_64/lib/cmake"

if [[ ! -d "${DIR_FROM}" ]]; then
	WriteLog "Directory '${DIR_FROM}' does not exist!"
	exit 1
fi

if [[ ! -d "${DIR_TO}" ]]; then
	WriteLog "Directory '${DIR_TO}' does not exist!"
	exit 1
fi

##
## Create symlink in from ~/lib/Qt/6.2.0/gcc_64/libexec to ${ROOT_DIR}
##
ln -sf "${QT_LIB_DIR}/gcc_64/libexec" "${ROOT_DIR}/${QT_VER}/mingw81_64/libexec"

##
## Create symlinks for applications needed in the make files.
##
for fn in "qtpaths" "qmake" \
	"qmldom" "qmllint" "qmlformat" "qmlprofiler" "qmlprofiler" "qmltime" "qmlplugindump" "qmltestrunner"\
	"androiddeployqt" "androidtestrunner" ; do
	if [[ ! -f "${QT_LIB_DIR}/gcc_64/bin/${fn}" ]] ; then
		WriteLog "Missing file to symlink: ${QT_LIB_DIR}/gcc_64/bin/${fn}"
	else
		WriteLog "Symlink to: ${QT_LIB_DIR}/gcc_64/bin/${fn}"
		ln -sf "${QT_LIB_DIR}/gcc_64/bin/${fn}" "${ROOT_DIR}/${QT_VER}/mingw81_64/bin"
	fi
done

#
# Replace all cmake files referencing windows EXE-tools.
#
pushd "${DIR_TO}" > /dev/null || exit
declare -a files
while IFS=  read -r -d $'\n'; do
	# Only file with a reverence to a '.exe' in it.
	if grep -qli "\.exe\"" "${REPLY}" ; then
		files+=("${REPLY}")
	fi
done < <(find "${DIR_TO}" -type f -name "*-relwithdebinfo.cmake" -printf "%P\n")
popd > /dev/null || exit

# Iterate through the files.
for fn in "${files[@]}" ; do
	WriteLog "Copying: $fn"
	cp "${DIR_FROM}/${fn}" "${DIR_TO}/${fn}"
done

#
# Files needed to find.
#

#	Qt6AxContainerTools/Qt6AxContainerToolsTargets-relwithdebinfo.cmake
#	Qt6AxServerTools/Qt6AxServerToolsTargets-relwithdebinfo.cmake
#	Qt6DBusTools/Qt6DBusToolsTargets-relwithdebinfo.cmake
#	Qt6GuiTools/win/Qt6GuiToolsTargets-relwithdebinfo.cmake
#	Qt6LinguistTools/Qt6LinguistToolsTargets-relwithdebinfo.cmake
#	Qt6QmlTools/Qt6QmlToolsTargets-relwithdebinfo.cmake
#	Qt6RemoteObjectsTools/Qt6RemoteObjectsToolsTargets-relwithdebinfo.cmake
#	Qt6ScxmlTools/Qt6ScxmlToolsTargets-relwithdebinfo.cmake
#	Qt6SerialBusTools/Qt6SerialBusToolsTargets-relwithdebinfo.cmake
#	Qt6ToolsTools/Qt6ToolsToolsTargets-relwithdebinfo.cmake
#	Qt6WidgetsTools/Qt6WidgetsToolsTargets-relwithdebinfo.cmake


