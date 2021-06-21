#!/bin/bash
#set -x

# Get the bash script directory.
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
FILE_OUT="${SCRIPT_DIR}/icons.qrc"

function WriteLn()
{
	echo -e "${@}" >> "${FILE_OUT}"
}

# Create file using a time stamp.
echo -e "<!-- Generated at: $(date +%Y-%m-%dT%T%Z:::z) -->" > "${FILE_OUT}"

# Opening root tag.
WriteLn '<RCC>'

# Generate the resource for PNG icons.
WriteLn '	<qresource prefix="icon/png/">'
for file in icon/*.png ; do
	file=$(basename "$(echo "${file}" | sed -e 's/\.[^.]*$//')")
	WriteLn "		<file alias=\"${file}\">icon/${file}.png</file>"
done
# Closing tag.
WriteLn '	</qresource>'

# Generate the resource for SVG icons.
WriteLn '	<qresource prefix="icon/svg/">'
for file in icon/*.svg ; do
	file=$(basename "$(echo "${file}" | sed -e 's/\.[^.]*$//')")
	WriteLn "		<file alias=\"${file}\">icon/${file}.svg</file>"
done
# Closing tag.
WriteLn '	</qresource>'

# Closing root tag.
WriteLn "</RCC>"




