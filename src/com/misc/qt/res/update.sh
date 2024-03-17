#!/bin/bash
#set -x

# Get the bash script directory.
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
FILE_OUT="${SCRIPT_DIR}/icons.qrc"

function WriteLn()
{
	echo -e "${@}" >> "${FILE_OUT}"
}

# Opening root tag and create file.
echo -e "<!DOCTYPE RCC>" > "${FILE_OUT}"

# Create file using a time stamp.
WriteLn "<!-- Generated at: $(date +%Y-%m-%dT%T%Z:::z) -->"
WriteLn '<RCC version="1.0">'

# Generate the resource for PNG icons.
WriteLn '	<qresource prefix="icon/png/">'
for file in ${SCRIPT_DIR}/icon/*.png ; do
	file=$(basename "$(echo "${file}" | sed -e 's/\.[^.]*$//')")
	WriteLn "		<file alias=\"${file}\">icon/${file}.png</file>"
done
# Closing tag.
WriteLn '	</qresource>'

# Generate the resource for SVG icons.
WriteLn '	<qresource prefix="icon/svg/">'
for file in ${SCRIPT_DIR}/icon/*.svg ; do
	file=$(basename "$(echo "${file}" | sed -e 's/\.[^.]*$//')")
	WriteLn "		<file alias=\"${file}\">icon/${file}.svg</file>"
done
# Closing tag.
WriteLn '	</qresource>'

# Closing root tag.
WriteLn "</RCC>"




