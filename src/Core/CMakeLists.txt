#---------------------------------------------------------------------
PROJECT( elxCore )

#---------------------------------------------------------------------
# Define lists of files in the subdirectories.

SET( KernelFilesForExecutables
  Kernel/elxElastixMain.cxx
  Kernel/elxElastixMain.h
  Kernel/elxTransformixMain.cxx
  Kernel/elxTransformixMain.h
)

SET( KernelFilesForComponents
  Kernel/elxElastixBase.cxx
  Kernel/elxElastixBase.h
  Kernel/elxElastixTemplate.h
  Kernel/elxElastixTemplate.hxx
)

SET( InstallFilesForExecutables
  Install/elxComponentLoader.cxx
  Install/elxComponentLoader.h
)

SET( InstallFilesForComponents
  Install/elxComponentDatabase.cxx
  Install/elxComponentDatabase.h
  Install/elxBaseComponent.h
  Install/elxBaseComponentSE.h
  Install/elxBaseComponentSE.hxx
  Install/elxInstallAllComponents.h
  Install/elxInstallFunctions.h
  Install/elxMacro.h
  Install/elxIncludes.h
)

SET( ConfigurationFiles
  Configuration/elxConfiguration.cxx
  Configuration/elxConfiguration.h
)

SET( ComponentBaseClassFiles
  ComponentBaseClasses/elxFixedImagePyramidBase.h
  ComponentBaseClasses/elxImageSamplerBase.h
  ComponentBaseClasses/elxInterpolatorBase.h
  ComponentBaseClasses/elxMetricBase.h
  ComponentBaseClasses/elxMovingImagePyramidBase.h
  ComponentBaseClasses/elxOptimizerBase.h
  ComponentBaseClasses/elxRegistrationBase.h
  ComponentBaseClasses/elxResampleInterpolatorBase.h
  ComponentBaseClasses/elxResamplerBase.h
  ComponentBaseClasses/elxTransformBase.h
  ComponentBaseClasses/elxFixedImagePyramidBase.hxx
  ComponentBaseClasses/elxImageSamplerBase.hxx
  ComponentBaseClasses/elxInterpolatorBase.hxx
  ComponentBaseClasses/elxMetricBase.hxx
  ComponentBaseClasses/elxMovingImagePyramidBase.hxx
  ComponentBaseClasses/elxOptimizerBase.hxx
  ComponentBaseClasses/elxRegistrationBase.hxx
  ComponentBaseClasses/elxResampleInterpolatorBase.hxx
  ComponentBaseClasses/elxResamplerBase.hxx
  ComponentBaseClasses/elxTransformBase.hxx
)

SET( ProgressCommandFiles
  elxProgressCommand.cxx
  elxProgressCommand.h
)

#---------------------------------------------------------------------
# Construct source groups for nice visualisation in Visual Studio.

SOURCE_GROUP( "Install" FILES
  ${InstallFilesForComponents}
  ${InstallFilesForExecutables}
)

SOURCE_GROUP( "Kernel" FILES
  ${KernelFilesForComponents}
  ${KernelFilesForExecutables}
)

SOURCE_GROUP( "Configuration" FILES ${ConfigurationFiles} )
SOURCE_GROUP( "ComponentBaseClasses" FILES ${ComponentBaseClassFiles} )
SOURCE_GROUP( "ProgressCommand" FILES ${ProgressCommandFiles} )

#---------------------------------------------------------------------
# Create the elxCore library.

ADD_LIBRARY( elxCore
  ${KernelFilesForComponents}
  ${InstallFilesForComponents}
  ${ConfigurationFiles}
  ${ComponentBaseClassFiles}
  ${ProgressCommandFiles}
)

#---------------------------------------------------------------------
# Link against other libraries.

TARGET_LINK_LIBRARIES( elxCore
  elxCommon
  xoutlib
  ITKBasicFilters
  ITKNumerics
  ITKIO
  ITKCommon
  ITKStatistics
)

#---------------------------------------------------------------------
# Add macro if mevisdicomtiff should be used
IF( ELASTIX_USE_MEVISDICOMTIFF )
  ADD_DEFINITIONS( -D_ELASTIX_USE_MEVISDICOMTIFF )
  SET( mevisdcmtifflib mevisdcmtiff )
ELSE()
  SET( mevisdcmtifflib )
ENDIF()


#---------------------------------------------------------------------
# Create the elastix executable.

ADD_EXECUTABLE( elastix
  Main/elastix.cxx
  Main/elastix.h
  Kernel/elxElastixMain.cxx
  Kernel/elxElastixMain.h
  ${InstallFilesForExecutables}
)

#---------------------------------------------------------------------
# Create the transformix executable.

ADD_EXECUTABLE( transformix
  Main/transformix.cxx
  Main/transformix.h
  Kernel/elxElastixMain.cxx
  Kernel/elxElastixMain.h
  Kernel/elxTransformixMain.cxx
  Kernel/elxTransformixMain.h
  ${InstallFilesForExecutables}
)

IF(ELASTIX_USE_EZMINC)
  TARGET_LINK_LIBRARIES(transformix ${MINC4ITK_LIBRARIES} ${EZMINC_LIBRARIES})
ENDIF(ELASTIX_USE_EZMINC)

#---------------------------------------------------------------------
# Link elastix against other libraries.

TARGET_LINK_LIBRARIES( elastix
  elxCommon
  elxCore
  param
  xoutlib
  ${mevisdcmtifflib}
  ITKBasicFilters
  ITKNumerics
  ITKIO
  ITKCommon
  ITKStatistics
  ${AllComponentLibs}
)

IF(ELASTIX_USE_EZMINC)
  TARGET_LINK_LIBRARIES(elastix ${MINC4ITK_LIBRARIES} ${EZMINC_LIBRARIES})
ENDIF(ELASTIX_USE_EZMINC)

#---------------------------------------------------------------------
# Link transformix against other libraries.

TARGET_LINK_LIBRARIES( transformix
  elxCommon
  elxCore
  param
  xoutlib
  ${mevisdcmtifflib}
  ITKBasicFilters
  ITKNumerics
  ITKIO
  ITKCommon
  ITKStatistics
  ${AllComponentLibs}
)

IF(ELASTIX_USE_EZMINC)
  TARGET_LINK_LIBRARIES(elastix ${MINC4ITK_LIBRARIES})
ENDIF(ELASTIX_USE_EZMINC)


#---------------------------------------------------------------------
# Define the install directory for elastix and transformix.

IF( WIN32 )
  INSTALL( TARGETS elastix transformix
    RUNTIME DESTINATION . )
    #COMPONENT core )
ELSE()
  # Tell the executables where to find the required .so files.
  SET_TARGET_PROPERTIES(elastix transformix
    PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib:${ITK_DIR}" )  
  # Put executables in bin and libraries in lib.
  INSTALL( TARGETS elastix transformix
    RUNTIME DESTINATION bin )
    #COMPONENT core )
ENDIF()

#---------------------------------------------------------------------
# The Core/Install directory contains a CMakeLists file for
# defining the elastix compilation types.

ADD_SUBDIRECTORY( Install )

