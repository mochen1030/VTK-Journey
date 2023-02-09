// own includes
#include "ContourTubeRepresentation.h"

// vtk includes
#include <vtkAutoInit.h>
#include <vtkCamera.h>
#include <vtkContourWidget.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>

// initialize vtl module 
VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkInteractionStyle);

int main(int, char* [])
{
	vtkNew<vtkNamedColors> colors;

	// Create the contour widget
	vtkNew<vtkContourWidget> contourWidget;

	// Override the default representation for the contour widget to customize its look
	vtkNew<ContourTubeRepresentation> contourTubeRepresentation;
	contourTubeRepresentation->GetLinesProperty()->SetColor(colors->GetColor3d("Red").GetData());
	contourWidget->SetRepresentation(contourTubeRepresentation);

	// Create the renderer to visualize the scene
	vtkNew<vtkRenderer> renderer;
	renderer->SetBackground(colors->GetColor3d("MidnightBlue").GetData());

	// Create the GUI window to hold the rendered scene
	vtkNew<vtkRenderWindow> renderWindow;
	renderWindow->AddRenderer(renderer);
	renderWindow->SetWindowName("SplineTube");
	renderWindow->SetSize(600, 600);

	// Create the events manager for the renderer window
	vtkNew<vtkRenderWindowInteractor> interactor;
	interactor->SetRenderWindow(renderWindow);

	// Use the "trackball camera" interactor style, rather than the default "joystick camera"
	vtkNew<vtkInteractorStyleTrackballCamera> style;
	interactor->SetInteractorStyle(style);

	// Set up the contour widget within the visualization pipeline just assembled
	contourWidget->SetInteractor(interactor);
	// Turn on the interactor observer
	contourWidget->On();
	// Reposition camera to fit the scene elements
	renderer->ResetCamera();
	renderer->GetActiveCamera()->SetDistance(500);

	// Start the interaction
	renderWindow->Render();
	interactor->Start();

	return EXIT_SUCCESS;
}