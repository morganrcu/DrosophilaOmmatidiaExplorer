cmake_minimum_required(VERSION 2.8.8)

add_definitions("-std=c++11 -fpermissive -g")

project(DrosophilaOmmatidiaExplorer)
include(CPack)
find_package(ITK REQUIRED)
INCLUDE(${ITK_USE_FILE})
find_package(VTK COMPONENTS
  vtkCommonCore
  vtkFiltersCore
  vtkInfovisCore
  vtkInteractionStyle

  vtkRenderingFreeType${VTK_RENDERING_BACKEND}
  vtkRenderingVolume${VTK_RENDERING_BACKEND}
  vtkViewsQt
  vtkjsoncpp
  vtkChartsCore
  vtkRenderingContext${VTK_RENDERING_BACKEND}
  vtkViewsContext2D
)
include(${VTK_USE_FILE})

if("${VTK_QT_VERSION}" STREQUAL "")
  message(FATAL_ERROR "VTK was not built with Qt")
endif()

# Use the include path and library for Qt that is used by VTK.
include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
)

# Set your files and resources here
set( Srcs main.cpp DrosophilaOmmatidiaExplorer.cpp DrosophilaOmmatidiaJSONProject.cpp  DetectVerticesDialog.cpp ShowVolumeGroupBox.cpp VertexListDockWidget.cpp EdgeListDockWidget.cpp GraphPlotterDockWidget.cpp)

set( Hdrs DrosophilaOmmatidiaExplorer.h   DetectVerticesDialog.h VertexListDockWidget.h EdgeListDockWidget.h  GraphPlotterDockWidget.h )#AJGraphSelectionInteractorStyle.h)

set( MOC_Hdrs DrosophilaOmmatidiaExplorer.h DetectVerticesDialog.h VertexListDockWidget.h  EdgeListDockWidget.h GraphPlotterDockWidget.h)#AJGraphSelectionInteractorStyle.h)

set( UIs DrosophilaOmmatidiaExplorer.ui DetectVerticesDialog.ui ShowVolumeGroupBox.ui VertexListDockWidget.ui  EdgeListDockWidget.ui  GraphPlotterDockWidget.ui)

set( QRCs Icons/icons.qrc )


# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

  # We have ui files, this will bring in the macro: qt5_wrap_ui
  find_package(Qt5Widgets REQUIRED QUIET)

  qt5_wrap_ui(UI_Srcs ${UIs})
  qt5_add_resources(QRC_Srcs ${QRCs} )

  source_group("Resources" FILES
    ${UIs}
    ${QRCs}
    ${EXE_ICON} # Not present
  )

  source_group("Generated" FILES
    ${UI_Srcs}
    ${MOC_Srcs}
    ${QRC_Srcs}
    ${QRC_Srcs}
  )

#add_executable(VertexMolecularDistributionTest VertexMolecularDistributionTest.cpp  )
#  target_link_libraries(VertexMolecularDistributionTest ${VTK_LIBRARIES} ${ITK_LIBRARIES})

  add_executable(DrosophilaOmmatidiaExplorer MACOSX_BUNDLE  ${Srcs} ${Hdrs} ${UI_Srcs} ${MOC_Hdrs} ${QRC_Srcs} )

  qt5_use_modules(DrosophilaOmmatidiaExplorer Core Gui Widgets)
  target_link_libraries(DrosophilaOmmatidiaExplorer ${VTK_LIBRARIES} ${ITK_LIBRARIES})


install(TARGETS DrosophilaOmmatidiaExplorer RUNTIME DESTINATION bin)


SET(CPACK_GENERATOR "DEB")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Rodrigo Cilla")
