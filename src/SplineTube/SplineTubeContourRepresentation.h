/*
 * @class:	SplineTubeContourRepresentation
 * @author: mc
 * @date:   2023.2.8
 * @brief:	This class provides the representation with a spline tube for the vtkContourWidget
 */

#ifndef SplineTubeContourRepresentation_h
#define SplineTubeContourRepresentation_h

// vtk includes
#include <vtkOrientedGlyphContourRepresentation.h>
#include <vtkTubeFilter.h>

class SplineTubeContourRepresentation : public vtkOrientedGlyphContourRepresentation
{
public:
	// intantiate this class
	static SplineTubeContourRepresentation* New();

	// standard methods for instances of this class
	vtkTypeMacro(SplineTubeContourRepresentation, vtkOrientedGlyphContourRepresentation);

protected:
	SplineTubeContourRepresentation();
	~SplineTubeContourRepresentation();

	// build the lines and tube, overrided from parent class, by mc
	void BuildLines() override;
	void BuildRepresentation() override;

	// get all the actors, overrided from class "vtkWidgetRepresentation", by mc
	void GetActors(vtkPropCollection*) override;

	// overrided from parent class, by mc
	void ReleaseGraphicsResources(vtkWindow* w) override;
	int RenderOverlay(vtkViewport* viewport) override;
	int RenderOpaqueGeometry(vtkViewport* viewport) override;
	int RenderTranslucentPolygonalGeometry(vtkViewport* viewport) override;
	vtkTypeBool HasTranslucentPolygonalGeometry() override;

protected:

	// members about the tube, by mc
	vtkSmartPointer<vtkPolyData>					m_Tube;
	vtkSmartPointer<vtkPolyDataMapper>				m_TubeMapper;
	vtkSmartPointer<vtkActor>						m_TubeActor;
	vtkSmartPointer<vtkTubeFilter>					m_TubeFilter;
};

#endif