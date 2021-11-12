cmake_minimum_required(VERSION 3.3)


PROJECT (Medical)

find_package(VTK COMPONENTS
  vtkFiltersGeometry
  vtkFiltersModeling
  vtkIOImage
  vtkIOXML
  vtkIOLegacy
  vtkImagingStatistics
  vtkInteractionStyle
  vtkRenderingVolumeOpenGL2
  vtkTestingRendering
  vtksys
)
include(${VTK_USE_FILE})

add_executable(Medical Medical.cxx)
target_link_libraries(Medical ${VTK_LIBRARIES})

