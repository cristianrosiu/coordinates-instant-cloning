cmake_minimum_required(VERSION 3.11 FATAL_ERROR)
project(coordinates)

find_package(OpenCV REQUIRED)
find_package(CGAL REQUIRED)
find_package(OpenMP REQUIRED)
find_package(Boost 1.66 REQUIRED COMPONENTS program_options system iostreams)

set(MAIN_EXE_NAME "mvcc")
set(public_libs "")
set(private_libs "")
set(include_dirs "")
set(link_dirs "")

list(APPEND public_libs ${OpenCV_LIBS} CGAL::CGAL ${Boost_LIBRARIES})
list(APPEND private_libs CGFramework OpenMP::OpenMP_CXX Boost::boost)
list(APPEND include_dirs ${CMAKE_CURRENT_BINARY_DIR} ${OpenCV_INCLUDE_DIRS} ${Boost_INCLUDE_DIR})

# Binaries directly to the binary dir without subfolders.
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/framework")
	# Create framework library and include CMake scripts (compiler warnings, sanitizers and static analyzers).
	add_subdirectory("framework") 
else() 
	# During development the framework lives in parent folder.
	add_subdirectory("../../../framework/" "${CMAKE_BINARY_DIR}/framework/")
endif()

add_executable(${MAIN_EXE_NAME} 
					"src/main.cpp"
					"src/mvc_solver.cpp"
					"src/adaptive_mesh.cpp"
					"src/mask_painter.cpp")

include_directories(${include_dirs})
target_include_directories(${MAIN_EXE_NAME} PUBLIC "include/")
target_link_libraries(${MAIN_EXE_NAME} PUBLIC ${public_libs} PRIVATE ${private_libs})
target_compile_features(${MAIN_EXE_NAME} PRIVATE cxx_std_20)
enable_sanitizers(${MAIN_EXE_NAME})
set_project_warnings(${MAIN_EXE_NAME})

# SET cwd for the MSVS debugger: https://stackoverflow.com/questions/41864259/how-to-set-working-directory-for-visual-studio-2017-rc-cmake-project
# set (VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}) 
set_target_properties(
    ${MAIN_EXE_NAME} PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

# # Copy all files in the resources folder to the build directory after every successful build.
# add_custom_command(TARGET ${MAIN_EXE_NAME} POST_BUILD
# 	COMMAND ${CMAKE_COMMAND} -E copy_directory
# 	"${CMAKE_CURRENT_LIST_DIR}/data" $<TARGET_FILE_DIR:${MAIN_EXE_NAME}>/data)

 
# Preprocessor definitions for path.
target_compile_definitions(${MAIN_EXE_NAME} PRIVATE "-DDATA_DIR=\"${CMAKE_CURRENT_LIST_DIR}/data/\"" "-DOUTPUT_DIR=\"${CMAKE_CURRENT_LIST_DIR}/outputs\"")

if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/grading_tests/")
	add_subdirectory("grading_tests")
endif()	