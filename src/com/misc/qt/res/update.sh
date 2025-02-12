#!/bin/bash
#set -x

# Get the bash script directory.
script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
file_out="${script_dir}/icons.qrc"

function WriteLn()
{
	echo -e "${@}" >> "${file_out}"
}

# Opening root tag and create file.
echo -e "<!DOCTYPE RCC>" > "${file_out}"

# Create file using a time stamp.
WriteLn "<!-- Generated at: $(date +%Y-%m-%dT%T%Z:::z) -->"
WriteLn '<RCC version="1.0">'

# Generate the resource for PNG icons.
WriteLn '	<qresource prefix="icon/png/">'
for file in "${script_dir}/icon/"*.png ; do
	file=$(basename "$(echo "${file}" | sed -e 's/\.[^.]*$//')")
	WriteLn "		<file alias=\"${file}\">icon/${file}.png</file>"
done
# Closing tag.
WriteLn '	</qresource>'

# Generate the resource for SVG icons.
WriteLn '	<qresource prefix="icon/svg/">'
for file in "${script_dir}/icon/"*.svg ; do
	file=$(basename "$(echo "${file}" | sed -e 's/\.[^.]*$//')")
	WriteLn "		<file alias=\"${file}\">icon/${file}.svg</file>"
done
# Closing tag.
WriteLn '	</qresource>'

# Closing root tag.
WriteLn "</RCC>"




