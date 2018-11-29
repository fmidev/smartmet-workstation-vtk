Background story from SmartMet-workstation development perspective:
1. Using VTK's 3D features to visualize meteorological data in 3D (new feature)
 - Creating separate MFC dialog module as own library that can be used with SmartMet project
2. Using VTK's 2D visualizations to replace SmartMet's current visualization library
 - Idea is to do at least the isoline, color contour and streamline visualizations with VTK
 - Currently used AVS ToolMaster is commercially licensed library
 - It doesn't fit well with the Open source development of SmartMet
----------------------------------------------------------------------

A newbase data visualizer based on the Visualization Toolkit

Overview of the project  

fmivisbase - a dll project with the bulk of the functionality. the major classes it contains are  

NewBaseSourcer and its children read newbase data and feed it into the VTK pipeline to be processed and rendered  
NewBaseSourcerBase  
  contains a handful of common methods  
-newBaseSourcer (pimpl)  

--nbsWindVectors  
A specialization for vector data  


--nbsSurface (pimpl)  
  A specialization for heightmapped mesh data  
  
---nbsSurfaceWind  
    with vectors  
    

ParamVisualizers own an instance of newbasesourcer and then construct and manage the VTK pipeline to perform rendering  


ParamVisualizerBase  
  Contains most of the logic for communicating with upper layers of the application, and the basic control interface  
  
ParamVisualizer2D  
  visualizer for nbsSurface's mesh data  
  
ParamVisualizer3D  
  visualizes standard newbase grids as volumes or contour surfaces  
  
ParamVisualizerWindVec  

ParamVisualizerWindVec2D  
  wind specializations  

ParamVisualizerIcon  
  visualizes surface data with icons  

ParamVisualizerText, ParamVisualizerArrayText and ParamVisualizerPolyText are (failed) attempts at making text visualization  
Perhaps a regular VTK labeler could achieve what they could not  


DrawOptions.h contains some functions to convert smartnet config into VTK primitives, and the groundwork for json config.  

AreaUtil.h contains the code to transform camera view into a newbase grid, in order to crop the data to be requested down.  


VisualizerManager owns and organizes visualizers and passes interface interaction onto them.   
It additionally owns the ContourLabeler used by visualizers to draw labels on isolines  

ViewportManager & co contain Viewports that are a very transparent layer between UI and VisualizerManagers to have multiple views on the screen  


Interactor.h contains the classes that implement specializations of VTK UI handling classes for basic keypress processing.  

They own TimeAnimator that handles automatically stepping through time  
Finally, ViewportFactory.h contains the code to build the full stack capable of rendering a 2D grid view.  


The actual executables themselves are lightweight and located in surfacevis and volumevis respectively, 
with volumevis setup (due to config implementation) being handled in VolVis.h/cpp  


MFC ports require the cmake option Module_vtkGUISupportMFC and patching the faulty opengl call.
vtkEvenlySpacedStreamlines is flawed and will crash https://gitlab.kitware.com/vtk/vtk/issues/17380
