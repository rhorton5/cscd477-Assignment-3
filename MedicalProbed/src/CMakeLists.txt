cmake_minimum_required(VERSION 3.3...3.12 FATAL_ERROR)


PROJECT (MedicalProbed)

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

add_executable(MedicalProbed MedicalProbed.cxx)

target_link_libraries(MedicalProbed ${VTK_LIBRARIES})
