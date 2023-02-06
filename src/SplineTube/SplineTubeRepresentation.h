#ifndef SPLINETUBEREPRESENTATION_H
#define SPLINETUBEREPRESENTATION_H

// vtk includes, mc
#include <vtkContourRepresentation.h>
#include <vtkParametricSpline.h>
#include <vtkParametricFunctionSource.h>
#include <vtkTubeFilter.h>

class vtkProperty;
class vtkActor;
class vtkPolyDataMapper;
class vtkPolyData;
class vtkGlyph3D;
class vtkPoints;
class SplineTubeRepresentation : public vtkContourRepresentation
{
public:
	/**
	 * Instantiate this class.
	 */
	static SplineTubeRepresentation* New();

	///@{
	/**
	 * Standard methods for instances of this class.
	 */
	vtkTypeMacro(SplineTubeRepresentation, vtkContourRepresentation);
	void PrintSelf(ostream& os, vtkIndent indent) override;
	///@}

	///@{
	/**
	 * Specify the cursor shape. Keep in mind that the shape will be
	 * aligned with the constraining plane by orienting it such that
	 * the x axis of the geometry lies along the normal of the plane.
	 */
	void SetCursorShape(vtkPolyData* cursorShape);
	vtkPolyData* GetCursorShape();
	///@}

	///@{
	/**
	 * Specify the shape of the cursor (handle) when it is active.
	 * This is the geometry that will be used when the mouse is
	 * close to the handle or if the user is manipulating the handle.
	 */
	void SetActiveCursorShape(vtkPolyData* activeShape);
	vtkPolyData* GetActiveCursorShape();
	///@}

	///@{
	/**
	 * This is the property used when the handle is not active
	 * (the mouse is not near the handle)
	 */
	vtkGetObjectMacro(Property, vtkProperty);
	///@}

	///@{
	/**
	 * This is the property used when the user is interacting
	 * with the handle.
	 */
	vtkGetObjectMacro(ActiveProperty, vtkProperty);
	///@}

	///@{
	/**
	 * This is the property used by the lines.
	 */
	vtkGetObjectMacro(LinesProperty, vtkProperty);
	///@}

	///@{
	/**
	 * Subclasses of SplineTubeRepresentation must implement these methods. These
	 * are the methods that the widget and its representation use to
	 * communicate with each other.
	 */
	void SetRenderer(vtkRenderer* ren) override;
	void BuildRepresentation() override;
	void StartWidgetInteraction(double eventPos[2]) override;
	void WidgetInteraction(double eventPos[2]) override;
	int ComputeInteractionState(int X, int Y, int modified = 0) override;
	///@}

	///@{
	/**
	 * Methods to make this class behave as a vtkProp.
	 */
	void GetActors(vtkPropCollection*) override;
	void ReleaseGraphicsResources(vtkWindow*) override;
	int RenderOverlay(vtkViewport* viewport) override;
	int RenderOpaqueGeometry(vtkViewport* viewport) override;
	int RenderTranslucentPolygonalGeometry(vtkViewport* viewport) override;
	vtkTypeBool HasTranslucentPolygonalGeometry() override;
	///@}

	/**
	 * Get the points in this contour as a vtkPolyData.
	 */
	vtkPolyData* GetContourRepresentationAsPolyData() override;

	///@{
	/**
	 * Controls whether the contour widget should always appear on top
	 * of other actors in the scene. (In effect, this will disable OpenGL
	 * Depth buffer tests while rendering the contour).
	 * Default is to set it to false.
	 */
	vtkSetMacro(AlwaysOnTop, vtkTypeBool);
	vtkGetMacro(AlwaysOnTop, vtkTypeBool);
	vtkBooleanMacro(AlwaysOnTop, vtkTypeBool);
	///@}

	/**
	 * Convenience method to set the line color.
	 * Ideally one should use GetLinesProperty()->SetColor().
	 */
	void SetLineColor(double r, double g, double b);

	/**
	 * A flag to indicate whether to show the Selected nodes
	 * Default is to set it to false.
	 */
	void SetShowSelectedNodes(vtkTypeBool) override;

	/**
	 * Return the bounds of the representation
	 */
	double* GetBounds() override;

protected:
	SplineTubeRepresentation();
	~SplineTubeRepresentation() override;

	// Render the cursor
	vtkActor* Actor;
	vtkPolyDataMapper* Mapper;
	vtkGlyph3D* Glypher;
	vtkActor* ActiveActor;
	vtkPolyDataMapper* ActiveMapper;
	vtkGlyph3D* ActiveGlypher;
	vtkPolyData* CursorShape;
	vtkPolyData* ActiveCursorShape;
	vtkPolyData* FocalData;
	vtkPoints* FocalPoint;
	vtkPolyData* ActiveFocalData;
	vtkPoints* ActiveFocalPoint;

	vtkPolyData* SelectedNodesData;
	vtkPoints* SelectedNodesPoints;
	vtkActor* SelectedNodesActor;
	vtkPolyDataMapper* SelectedNodesMapper;
	vtkGlyph3D* SelectedNodesGlypher;
	vtkPolyData* SelectedNodesCursorShape;
	void CreateSelectedNodesRepresentation();

	vtkPolyData* Lines;
	vtkPolyDataMapper* LinesMapper;
	vtkActor* LinesActor;

	// Support picking
	double LastPickPosition[3];
	double LastEventPosition[2];

	// Methods to manipulate the cursor
	void Translate(double eventPos[2]);
	void Scale(double eventPos[2]);
	void ShiftContour(double eventPos[2]);
	void ScaleContour(double eventPos[2]);

	void ComputeCentroid(double* ioCentroid);

	// Properties used to control the appearance of selected objects and
	// the manipulator in general.
	vtkProperty* Property;
	vtkProperty* ActiveProperty;
	vtkProperty* LinesProperty;
	void CreateDefaultProperties();

	// Distance between where the mouse event happens and where the
	// widget is focused - maintain this distance during interaction.
	double InteractionOffset[2];

	vtkTypeBool AlwaysOnTop;

	void BuildLines() override;

	//vtkPolyData*											Tube;								// 样条管道 polydata, mc
	//vtkPolyDataMapper*									TubeMapper;							// 样条管道 polydata mapper, mc
	//vtkActor*												TubeActor;							// 样条管道 polydata actor, mc
	//vtkProperty*											TubeProperty;						// 样条管道属性, mc
	vtkParametricSpline*									TubeSpline;
	vtkParametricFunctionSource*							TubeFunctionSource;
	vtkTubeFilter*											Tube;

private:
	SplineTubeRepresentation(const SplineTubeRepresentation&) = delete;
	void operator=(const SplineTubeRepresentation&) = delete;
};

#endif