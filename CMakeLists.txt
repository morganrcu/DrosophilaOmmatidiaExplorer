cmake_minimum_required(VERSION 2.8.8)



project(DrosophilaOmmatidiaExplorer)


add_definitions("-std=c++11 -fpermissive ")
#add_definitions("-std=c++11 -fpermissive -O3")
set(CXX_STANDARD 11)
set(CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Release)

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

find_package(ITK REQUIRED)
INCLUDE(${ITK_USE_FILE})


#include_directories (/usr/include/eigen3)


find_package(VTK 6.3 COMPONENTS
  vtkCommonCore
  vtkFiltersCore
  vtkInfovisCore
  vtkInteractionStyle

  #vtkRenderingFreeType${VTK_RENDERING_BACKEND}
  #vtkRenderingVolume${VTK_RENDERING_BACKEND}
  vtkRenderingVolume
  vtkRenderingVolume${VTK_RENDERING_BACKEND}
  vtkViewsQt
  
  vtkjsoncpp
  vtkChartsCore
  vtkRenderingContext${VTK_RENDERING_BACKEND}
  vtkViewsContext2D
  #vtkIOFFMPEG

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

set( Srcs main.cpp DrosophilaOmmatidiaExplorer.cpp DrosophilaOmmatidiaJSONProject.cpp  DetectVerticesDialog.cpp ShowVolumeGroupBox.cpp VertexListDockWidget.cpp EdgeListDockWidget.cpp GraphPlotterDockWidget.cpp CellListDockWidget.cpp AJTrackingFrame.cpp OmmatidiaTissueDockWidget.cpp  OmmatidiaTissueTreeModel.cpp )

set( Hdrs DrosophilaOmmatidiaExplorer.h   DetectVerticesDialog.h VertexListDockWidget.h EdgeListDockWidget.h  GraphPlotterDockWidget.h CellListDockWidget.h  AJTrackingFrame.h OmmatidiaTissueDockWidget.h  OmmatidiaTissueTreeModel.h  ) #AJGraphSelectionInteractorStyle.h)

set( MOC_Hdrs DrosophilaOmmatidiaExplorer.h DetectVerticesDialog.h VertexListDockWidget.h  EdgeListDockWidget.h GraphPlotterDockWidget.h CellListDockWidget.h AJTrackingFrame.h OmmatidiaTissueDockWidget.h OmmatidiaTissueTreeModel.h  )#AJGraphSelectionInteractorStyle.h)

set( UIs DrosophilaOmmatidiaExplorer.ui DetectVerticesDialog.ui ShowVolumeGroupBox.ui VertexListDockWidget.ui  EdgeListDockWidget.ui  GraphPlotterDockWidget.ui CellListDockWidget.ui AJTrackingFrame.ui OmmatidiaTissueDockWidget.ui )

set( QRCs Icons/icons.qrc )


# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

  # We have ui files, this will bring in the macro: qt5_wrap_ui
  find_package(Qt5Widgets REQUIRED )
  #find_package(Qt COMPONENTS Qt5Widgets  REQUIRED)

  
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
#target_link_libraries(DrosophilaOmmatidiaExplorer ${VTK_LIBRARIES} ${ITK_LIBRARIES} Cbc OsiClp Osi CoinUtils)
target_link_libraries(DrosophilaOmmatidiaExplorer ${VTK_LIBRARIES} ${ITK_LIBRARIES})


#install(TARGETS DrosophilaOmmatidiaExplorer DESTINATION bin  COMPONENT Runtime)

include(DrosophilaOmmatidiaExplorerCPack.cmake )

#SET(CPACK_GENERATOR "DEB")
#SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Rodrigo Cilla")
enable_testing()
IF(WITH_GTEST )
	find_package(GTest REQUIRED)
	include_directories(${GTEST_INCLUDE_DIR})
ENDIF(WITH_GTEST)
#add_executable(MinCostMaxFlowAJAssociationCommandTest MinCostMaxFlowAJAssociationCommandTest.cpp DrosophilaOmmatidiaJSONProject.cpp )
#target_link_libraries(MinCostMaxFlowAJAssociationCommandTest ${ITK_LIBRARIES} ${VTK_LIBRARIES} ${GTEST_LIBRARY} Cbc OsiClp Osi CoinUtils	)

add_executable(EnergyModelTissueSmootherTest EnergyModelTissueSmootherTest.cpp DrosophilaOmmatidiaJSONProject.cpp)
target_link_libraries(EnergyModelTissueSmootherTest ${ITK_LIBRARIES} ${VTK_LIBRARIES} ${GTEST_LIBRARY} 	)
 
#add_executable(MetaheuristicTissueAssociationCommandTest MetaheuristicTissueAssociationCommandTest.cpp DrosophilaOmmatidiaJSONProject.cpp )
#target_link_libraries(MetaheuristicTissueAssociationCommandTest ${ITK_LIBRARIES} ${VTK_LIBRARIES})

#add_executable(S2EmbeddingTest S2EmbeddingTest.cpp)
#target_link_libraries(S2EmbeddingTest ${ITK_LIBRARIES})

#add_executable(TissueSurfaceMeshSourceTest TissueSurfaceMeshSourceTest.cpp)
#target_link_libraries(TissueSurfaceMeshSourceTest ${ITK_LIBRARIES} )


#add_executable(S2LLEEmbeddingTest S2LLEEmbeddingTest.cpp)
#target_link_libraries(S2LLEEmbeddingTest ${ITK_LIBRARIES})


					
					
#add_test(
    #NAME MinCostMaxFlowAJAssociationCommandTest
    #COMMAND MinCostMaxFlowAJAssociationCommandTest
#)
