/*=========================================================================

  Program:   Visualization Toolkit
  Module:    Medical3.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

//
// This example reads a volume dataset, extracts two isosurfaces that
// represent the skin and bone, creates three orthogonal planes
// (sagittal, axial, coronal), and displays them.
//
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkVolume16Reader.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkOutlineFilter.h>
#include <vtkCamera.h>
#include <vtkStripper.h>
#include <vtkLookupTable.h>
#include <vtkImageDataGeometryFilter.h>
#include <vtkProperty.h>
#include <vtkPolyDataNormals.h>
#include <vtkContourFilter.h>
#include <vtkImageData.h>
#include <vtkImageMapToColors.h>
#include <vtkImageActor.h>
#include <vtkSmartPointer.h>
#include <vtkImageMapper3D.h>
#include <vtkStructuredPointsReader.h>

int main (int argc, char *argv[])
{
  
  // Create the renderer, the render window, and the interactor. The
  // renderer draws into the render window, the interactor enables
  // mouse- and keyboard-based interaction with the data within the
  // render window.
  //

  vtkSmartPointer<vtkRenderer> aRenderer = vtkSmartPointer<vtkRenderer>::New();
  vtkSmartPointer<vtkRenderWindow> renWin = vtkSmartPointer<vtkRenderWindow>::New();
  renWin->AddRenderer(aRenderer);
  vtkSmartPointer<vtkRenderWindowInteractor> iren =
    vtkSmartPointer<vtkRenderWindowInteractor>::New();
  iren->SetRenderWindow(renWin);

  // Set a background color for the renderer and set the size of the
  // render window (expressed in pixels).
    aRenderer->SetBackground(.2, .3, .4);

  renWin->SetSize(640, 480);

  // The following reader is used to read a series of 2D slices (images)
  // that compose the volume. The slice dimensions are set, and the
  // pixel spacing. The data Endianness must also be specified. The
  // reader uses the FilePrefix in combination with the slice number to
  // construct filenames using the format FilePrefix.%d. (In this case
  // the FilePrefix is the root name of the file: quarter.)
  
  /*
  vtkSmartPointer<vtkVolume16Reader> v16 = vtkSmartPointer<vtkVolume16Reader>::New();
  v16->SetDataDimensions(64,64);
  v16->SetImageRange(1, 93);
  v16->SetDataByteOrderToLittleEndian();
  v16->SetFilePrefix ("headsq/quarter");
  v16->SetDataSpacing (3.2, 3.2, 1.5);
  v16->Update();*/

  vtkSmartPointer<vtkStructuredPointsReader> v16 =
      vtkSmartPointer<vtkStructuredPointsReader>::New();
  v16->SetFileName("temperature.dat");
  v16->Update();

 
  // An outline provides context around the data.
  //
  vtkSmartPointer<vtkOutlineFilter> outlineData =
    vtkSmartPointer<vtkOutlineFilter>::New();
  outlineData->SetInputConnection(v16->GetOutputPort());
  outlineData->Update();

  vtkSmartPointer<vtkPolyDataMapper> mapOutline =
    vtkSmartPointer<vtkPolyDataMapper>::New();
  mapOutline->SetInputConnection(outlineData->GetOutputPort());

  vtkSmartPointer<vtkActor> outline =
    vtkSmartPointer<vtkActor>::New();
  outline->SetMapper(mapOutline);
  outline->GetProperty()->SetColor(0,0,0);

  // Now we are creating three orthogonal planes passing through the
  // volume. Each plane uses a different texture map and therefore has
  // different coloration.


  // Now create a lookup table that consists of the full hue circle
  // (from HSV).
  vtkSmartPointer<vtkLookupTable> hueLut =
    vtkSmartPointer<vtkLookupTable>::New();
  hueLut->SetTableRange (0, 1);
  hueLut->SetHueRange (0,0.667);
  hueLut->SetSaturationRange (1, 1);
  hueLut->SetValueRange (1, 1);
  hueLut->Build(); //effective built

  

  // Create the first  plane. The filter vtkImageMapToColors
  // maps the data through the corresponding lookup table created above.  The
  // vtkImageActor is a type of vtkProp and conveniently displays an image on
  // a single quadrilateral plane. It does this using texture mapping and as
  // a result is quite fast. (Note: the input image has to be unsigned char
  // values, which the vtkImageMapToColors produces.) Note also that by
  // specifying the DisplayExtent, the pipeline requests data of this extent
  // and the vtkImageMapToColors only processes a slice of data.

  vtkSmartPointer<vtkImageMapToColors> colors =
    vtkSmartPointer<vtkImageMapToColors>::New();
  colors->SetInputConnection(v16->GetOutputPort());
  colors->SetLookupTable(hueLut);
  colors->Update();

  vtkSmartPointer<vtkImageActor> sagittal =
    vtkSmartPointer<vtkImageActor>::New();
  sagittal->GetMapper()->SetInputConnection(colors->GetOutputPort());
  sagittal->SetDisplayExtent(0,9, 17,17, 0,9);

  vtkSmartPointer<vtkImageActor> axial =
      vtkSmartPointer<vtkImageActor>::New();
  axial->GetMapper()->SetInputConnection(colors->GetOutputPort());
  axial->SetDisplayExtent(0, 17, 0, 17, 0, 9);

  vtkSmartPointer<vtkImageActor> coronal =
      vtkSmartPointer<vtkImageActor>::New();
  coronal->GetMapper()->SetInputConnection(colors->GetOutputPort());
  coronal->SetDisplayExtent(9, 9, 0, 9, 0, 9);

  
  // It is convenient to create an initial view of the data. The
  // FocalPoint and Position form a vector direction. Later on
  // (ResetCamera() method) this vector is used to position the camera
  // to look at the data in this direction.
  vtkSmartPointer<vtkCamera> aCamera =
    vtkSmartPointer<vtkCamera>::New();
  aCamera->SetViewUp(1, 0, 0);
  aCamera->SetPosition(0, 0, 1); //Probed in the X direction.
  aCamera->Azimuth(0.0);
  //aCamera->Elevation(0.0);
  //aCamera->Dolly(1.0);

  // Actors are added to the renderer.
  aRenderer->AddActor(outline);
  aRenderer->AddActor(sagittal);
  aRenderer->AddActor(coronal);
  aRenderer->AddActor(axial);


  // An initial camera view is created.  The Dolly() method moves
  // the camera towards the FocalPoint, thereby enlarging the image.
  aRenderer->SetActiveCamera(aCamera);

  // Calling Render() directly on a vtkRenderer is strictly forbidden.
  // Only calling Render() on the vtkRenderWindow is a valid call.
  renWin->Render();

  aRenderer->ResetCamera ();
  aCamera->Dolly(1.5);

  // Note that when camera movement occurs (as it does in the Dolly()
  // method), the clipping planes often need adjusting. Clipping planes
  // consist of two planes: near and far along the view direction. The
  // near plane clips out objects in front of the plane; the far plane
  // clips out objects behind the plane. This way only what is drawn
  // between the planes is actually rendered.
  aRenderer->ResetCameraClippingRange ();

  // interact with data
  iren->Initialize();
  iren->Start();

  return EXIT_SUCCESS;
}
