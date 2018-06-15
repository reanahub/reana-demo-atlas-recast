#
# Build configuration for the MyEventSelection package
#

# Set the name of the package:
atlas_subdir( MyEventSelection )

# Set up which packages this package depends on:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools
   PhysicsAnalysis/AnalysisCommon/PATCore
   PhysicsAnalysis/D3PDTools/EventLoop
   PhysicsAnalysis/ElectronPhotonID/ElectronPhotonFourMomentumCorrection
   PhysicsAnalysis/Interfaces/AsgAnalysisInterfaces
   PRIVATE
   Control/AthContainers
   Control/CxxUtils
   Control/xAODRootAccess
   Event/xAOD/xAODCutFlow
   Event/xAOD/xAODBase
   Event/xAOD/xAODCore
   Event/xAOD/xAODMuon
   Event/xAOD/xAODEgamma
   Event/xAOD/xAODEventInfo
   Event/xAOD/xAODTracking )

# External(s) used by the package:
find_package( ROOT COMPONENTS Core Hist Physics REQUIRED )

# Generate a dictionary for the library:
atlas_add_root_dictionary( MyEventSelectionLib MyEventSelectionLibDictSrc
   ROOT_HEADERS MyEventSelection/MyEventSelectionAlg.h Root/LinkDef.h
   EXTERNAL_PACKAGES ROOT )

# Build the shared library of the package:
atlas_add_library( MyEventSelectionLib
   MyEventSelection/*.h Root/*.h Root/*.cxx ${MyEventSelectionLibDictSrc}
   PUBLIC_HEADERS MyTrigAnalysis
   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES AsgTools PATCoreLib EventLoop
   AsgAnalysisInterfaces
   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers CxxUtils
   xAODRootAccess xAODBase xAODCore xAODEgamma xAODEventInfo xAODTracking xAODMuon xAODCutFlow)

# Install the python script(s) of the package:
atlas_install_scripts( macros/LocalJob.py )

atlas_add_executable( myEventSelection util/myEventSelection.cxx
  LINK_LIBRARIES MyEventSelectionLib
 )
