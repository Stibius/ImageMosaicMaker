cmake_minimum_required (VERSION 3.8)

# solution
project(ImageMosaicMaker)
# project
set(APP_NAME "ImageMosaicMaker")

set( CMAKE_AUTORCC ON )
set( CMAKE_AUTOUIC ON )
set( CMAKE_AUTOMOC ON )

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 QUIET COMPONENTS Widgets Concurrent)   
find_package(OpenCV REQUIRED QUIET)  

set(GUI_FILES
   src/MainWindow.ui
)

set(RESOURCE_FILES
   src/resources.qrc 
)

set(HEADER_FILES
   src/ConcurrencyHandler.h
   src/Grid.h
   src/GridGenerator.h
   src/ImageLoader.h
   src/ImageMosaicMaker.h
   src/InputImage.h
   src/MainWindow.h
   src/Mosaic.h
   src/MosaicGenerator.h
   src/MyQGraphicsScene.h
   src/MyQGraphicsView.h
   src/MyQListWidget.h   
   src/TileSet.h
   src/Utility.h
)

set(SOURCE_FILES
   src/ConcurrencyHandler.cpp
   src/Grid.cpp
   src/GridGenerator.cpp
   src/ImageLoader.cpp
   src/ImageMosaicMaker.cpp
   src/InputImage.cpp
   src/main.cpp
   src/MainWindow.cpp
   src/Mosaic.cpp
   src/MosaicGenerator.cpp
   src/MyQGraphicsScene.cpp
   src/MyQGraphicsView.cpp
   src/MyQListWidget.cpp  
   src/TileSet.cpp
   src/Utility.cpp
)

# Define a grouping for source files in IDE project generation
source_group("GUI Files" FILES ${GUI_FILES})

# Define a grouping for source files in IDE project generation
source_group("Resource Files" FILES ${RESOURCE_FILES})

# Define a grouping for source files in IDE project generation
source_group("Source Files" FILES ${SOURCE_FILES})

# Define a grouping for source files in IDE project generation
source_group("Header Files" FILES ${HEADER_FILES})

add_executable(${APP_NAME} ${SOURCE_FILES} ${HEADER_FILES} ${GUI_FILES} ${RESOURCE_FILES})                                   

target_link_libraries(${APP_NAME} Qt5::Widgets 
                                  Qt5::Concurrent
                                  ${OpenCV_LIBS}
                                  )
                     