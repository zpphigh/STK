
set(Slicer_SKIP_SlicerEnableExtensionTesting TRUE)

set(SlicerModules
  VolumeDisplayController
  FiducialMarkerRegistration 
  SurgeryRoutePlanning
  #TumorContourSketch
  )

foreach(module ${SlicerModules})
 add_subdirectory(${module})
endforeach(module)


