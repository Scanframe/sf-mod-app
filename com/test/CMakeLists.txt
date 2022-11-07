cmake_minimum_required(VERSION 3.23)
project(wine-exec
	VERSION 1.2.3
	DESCRIPTION "Executable executing wine-exec.sh to launch Windows apps."
	HOMEPAGE_URL "https://wwww.scanframe.nl"
	)

# Add target.
Sf_AddExecutable("${PROJECT_NAME}")
# Add custom target to report resource stored versions.
Sf_AddExifTarget("${PROJECT_NAME}")
# Set the sources for this target.
target_sources("${PROJECT_NAME}" PRIVATE exec-wine.cpp)
# SEt the suffix for the produced target binary.
Sf_SetBinarySuffix("${PROJECT_NAME}")
# Add version resource to the target.
Sf_AddVersionResource("${PROJECT_NAME}")
