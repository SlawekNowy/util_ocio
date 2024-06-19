include(${CMAKE_SOURCE_DIR}/cmake/pr_common.cmake)

set(PROJ_NAME util_ocio)
pr_add_library(${PROJ_NAME} STATIC)

pr_add_dependency(${PROJ_NAME} util_image TARGET PUBLIC)

pr_add_external_dependency(${PROJ_NAME} OpenColorIO LIBRARY)
pr_add_external_dependency(${PROJ_NAME} OpenImageIO LIBRARY)


pr_add_headers(${PROJ_NAME} "include/")
pr_add_sources(${PROJ_NAME} "src/")

if(UNIX)
	target_link_libraries(${PROJ_NAME} PRIVATE dl)
endif()

pr_finalize(${PROJ_NAME})
