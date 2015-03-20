# ------------------------------------------------------------------------- 
# This CMake code installs the needed support libraries on NON OSX platforms
# ------------------------------------------------------------------------- 

include( InstallRequiredSystemLibraries )

Message( ${Qt5Widgets_LIBRARIES})
get_target_property(Qt5Widgets_location ${Qt5Widgets_LIBRARIES} LOCATION)
install(FILES ${Qt5Widgets_location} DESTINATION bin COMPONENT Runtime)

foreach(itklib  ${ITK_LIBRARIES} )
message(${itklib})
get_target_property(itklib_location ${itklib} LOCATION)
message(${itklib_location})
install(FILES ${itklib_location} DESTINATION bin COMPONENT Runtime)
endforeach(itklib)

foreach(vtklib  ${VTK_LIBRARIES} )
get_target_property(vtklib_location ${vtklib} LOCATION)

install(FILES ${vtklib_location} DESTINATION bin COMPONENT Runtime)
endforeach(vtklib)