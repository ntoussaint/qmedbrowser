cmake_minimum_required(VERSION 3.0)

project(QMedBrowser
  VERSION 0.1.0)

set(CMAKE_POLICY_DEFAULT_CMP0054 NEW)
cmake_policy(SET CMP0054 NEW)
if (NOT WIN32)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-deprecated-declarations")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
endif()

option(BUILD_SHARED_LIBS OFF)
if (BUILD_SHARED_LIBS)
  set(${PROJECT_NAME}_BUILD_TYPE SHARED)
endif()
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
if(WIN32)
  set(LIBRARY_OUTPUT_PATH  ${CMAKE_BINARY_DIR}/bin)
else()
  set(LIBRARY_OUTPUT_PATH  ${CMAKE_BINARY_DIR}/lib)
endif(WIN32)
if (APPLE)
  set(GUI_EXECUTABLE MACOSX_BUNDLE)
endif()
file(WRITE ${PROJECT_BINARY_DIR}/VERSION "${PROJECT_VERSION}")


set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
find_package(Qt5 REQUIRED Core Concurrent Gui Widgets)


option(USE_ITK "Use ITK to have be able to read medical images" OFF)
if (USE_ITK)
  find_package(ITK REQUIRED)
  include(${ITK_USE_FILE})
  add_definitions(-Duse_itk)
endif()

include_directories(
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_SOURCE_DIR}/QMedBrowsing
  )

add_subdirectory(QMedBrowsing)

set (resource_files 
  resources/QMedBrowser.qrc
  )

add_executable(QMedBrowser ${GUI_EXECUTABLE}
  QMedBrowser.cxx
  ${resource_files}
  )
target_link_libraries(QMedBrowser
  QMedBrowsing
  )

install(TARGETS QMedBrowser
  BUNDLE DESTINATION bin
  RUNTIME DESTINATION bin
  )

install(RUNTIME_DEPENDENCY_SET QMedBrowser)

if (APPLE)
  set(MACOSX_BUNDLE_INFO_STRING "QMedBrowser - Medical Image Browser")
  set(MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJECT_VERSION}")
  set(MACOSX_BUNDLE_BUNDLE_NAME "QMedBrowser")
  set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}")
  set(MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}")
endif()

## #################################################################
## Packaging
## #################################################################
include(InstallRequiredSystemLibraries)

if (WIN32)
  set(CPACK_GENERATOR "NSIS")
endif()

set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VENDOR "https://scholar.google.com/citations?user=XX0ZzxUAAAAJ&hl=en")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_NAME} - QMedBrowser - Medical Image Browser")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT "nicolas.toussaint@gmail.com")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/license.txt")
set(CPACK_RESOURCE_FILE_README "${PROJECT_SOURCE_DIR}/README.md")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/README.md")
# Do not ask for admin access rights (no UAC dialog), to allow installation without admin access
set(CPACK_NSIS_DEFINES ${CPACK_NSIS_DEFINES} "RequestExecutionLevel user")
set(CPACK_NSIS_MODIFY_PATH ON)
# set(CPACK_INSTALL_CMAKE_PROJECTS "${CPACK_INSTALL_CMAKE_PROJECTS};${ITK_DIR};ITK;ALL;/")
# set(CPACK_INSTALL_CMAKE_PROJECTS "${CPACK_INSTALL_CMAKE_PROJECTS};${PROJECT_BINARY_DIR};ALL;/")
set(CPACK_PACKAGE_EXECUTABLES
  "QMedBrowser" "Medical Image Browser"
  )

# This must always be last!
include(CPack)
