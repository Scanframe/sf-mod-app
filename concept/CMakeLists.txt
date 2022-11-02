# Required first entry checking the cmake version.
cmake_minimum_required(VERSION 3.18)
# Set the global project name.
project("concept")
# Add Sub Projects in the right order of dependency.
if (SF_BUILD_QT)
	add_subdirectory(rt-ui-serialize)
	add_subdirectory(rt-shared-lib)
	add_subdirectory(custom-ui-plugin)
	add_subdirectory(script-engine)
	add_subdirectory(multi-docs)
	add_subdirectory(delegates)
	add_subdirectory(task-menu)
	add_subdirectory(treemodel)
	add_subdirectory(dock-widgets)
endif (SF_BUILD_QT)
add_subdirectory(unit-test)
