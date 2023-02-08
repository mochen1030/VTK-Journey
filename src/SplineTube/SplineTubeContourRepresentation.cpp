// own inclues
#include "SplineTubeContourRepresentation.h"

// vtk includes
#include <vtkActor.h>
#include <vtkBezierContourLineInterpolator.h>
#include <vtkCamera.h>
#include <vtkCleanPolyData.h>
#include <vtkCursor2D.h>
#include <vtkCylinderSource.h>
#include <vtkDoubleArray.h>
#include <vtkFocalPlanePointPlacer.h>
#include <vtkGlyph3D.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>

// implement the standard form of the New() method
vtkStandardNewMacro(SplineTubeContourRepresentation);

SplineTubeContourRepresentation::SplineTubeContourRepresentation()
{
	/********** Constructor copied from vtkOrientedGlyphContourRepresentation.cxx **********/

	// Initialize state
	this->InteractionState = vtkContourRepresentation::Outside;

	this->CursorShape = nullptr;
	this->ActiveCursorShape = nullptr;

	this->HandleSize = 0.01;

	this->PointPlacer = vtkFocalPlanePointPlacer::New();
	this->LineInterpolator = vtkBezierContourLineInterpolator::New();

	// Represent the position of the cursor
	this->FocalPoint = vtkPoints::New();
	this->FocalPoint->SetNumberOfPoints(100);
	this->FocalPoint->SetNumberOfPoints(1);
	this->FocalPoint->SetPoint(0, 0.0, 0.0, 0.0);

	vtkDoubleArray* normals = vtkDoubleArray::New();
	normals->SetNumberOfComponents(3);
	normals->SetNumberOfTuples(100);
	normals->SetNumberOfTuples(1);
	double n[3] = { 0, 0, 0 };
	normals->SetTuple(0, n);

	// Represent the position of the cursor
	this->ActiveFocalPoint = vtkPoints::New();
	this->ActiveFocalPoint->SetNumberOfPoints(100);
	this->ActiveFocalPoint->SetNumberOfPoints(1);
	this->ActiveFocalPoint->SetPoint(0, 0.0, 0.0, 0.0);

	vtkDoubleArray* activeNormals = vtkDoubleArray::New();
	activeNormals->SetNumberOfComponents(3);
	activeNormals->SetNumberOfTuples(100);
	activeNormals->SetNumberOfTuples(1);
	activeNormals->SetTuple(0, n);

	this->FocalData = vtkPolyData::New();
	this->FocalData->SetPoints(this->FocalPoint);
	this->FocalData->GetPointData()->SetNormals(normals);
	normals->Delete();

	this->ActiveFocalData = vtkPolyData::New();
	this->ActiveFocalData->SetPoints(this->ActiveFocalPoint);
	this->ActiveFocalData->GetPointData()->SetNormals(activeNormals);
	activeNormals->Delete();

	this->Glypher = vtkGlyph3D::New();
	this->Glypher->SetInputData(this->FocalData);
	this->Glypher->SetVectorModeToUseNormal();
	this->Glypher->OrientOn();
	this->Glypher->ScalingOn();
	this->Glypher->SetScaleModeToDataScalingOff();
	this->Glypher->SetScaleFactor(1.0);

	this->ActiveGlypher = vtkGlyph3D::New();
	this->ActiveGlypher->SetInputData(this->ActiveFocalData);
	this->ActiveGlypher->SetVectorModeToUseNormal();
	this->ActiveGlypher->OrientOn();
	this->ActiveGlypher->ScalingOn();
	this->ActiveGlypher->SetScaleModeToDataScalingOff();
	this->ActiveGlypher->SetScaleFactor(1.0);

	// The transformation of the cursor will be done via vtkGlyph3D
	// By default a vtkCursor2D will be used to define the cursor shape
	vtkCursor2D* cursor2D = vtkCursor2D::New();
	cursor2D->AllOff();
	cursor2D->PointOn();
	cursor2D->Update();
	this->SetCursorShape(cursor2D->GetOutput());
	cursor2D->Delete();

	vtkCylinderSource* cylinder = vtkCylinderSource::New();
	cylinder->SetResolution(64);
	cylinder->SetRadius(0.5);
	cylinder->SetHeight(0.0);
	cylinder->CappingOff();
	cylinder->SetCenter(0, 0, 0);

	vtkCleanPolyData* clean = vtkCleanPolyData::New();
	clean->PointMergingOn();
	clean->CreateDefaultLocator();
	clean->SetInputConnection(cylinder->GetOutputPort());

	vtkTransform* t = vtkTransform::New();
	t->RotateZ(90.0);

	vtkTransformPolyDataFilter* tpd = vtkTransformPolyDataFilter::New();
	tpd->SetInputConnection(clean->GetOutputPort());
	tpd->SetTransform(t);
	clean->Delete();
	cylinder->Delete();

	tpd->Update();
	this->SetActiveCursorShape(tpd->GetOutput());
	tpd->Delete();
	t->Delete();

	this->Glypher->SetSourceData(this->CursorShape);
	this->ActiveGlypher->SetSourceData(this->ActiveCursorShape);

	this->Mapper = vtkPolyDataMapper::New();
	this->Mapper->SetInputConnection(this->Glypher->GetOutputPort());

	// This turns on resolve coincident topology for everything
	// as it is a class static on the mapper
	vtkPolyDataMapper::SetResolveCoincidentTopologyToPolygonOffset();
	this->Mapper->ScalarVisibilityOff();
	// Put this on top of other objects
	this->Mapper->SetRelativeCoincidentTopologyLineOffsetParameters(-1, -1);
	this->Mapper->SetRelativeCoincidentTopologyPolygonOffsetParameters(-1, -1);
	this->Mapper->SetRelativeCoincidentTopologyPointOffsetParameter(-1);

	this->ActiveMapper = vtkPolyDataMapper::New();
	this->ActiveMapper->SetInputConnection(this->ActiveGlypher->GetOutputPort());
	this->ActiveMapper->ScalarVisibilityOff();
	this->ActiveMapper->SetRelativeCoincidentTopologyLineOffsetParameters(-1, -1);
	this->ActiveMapper->SetRelativeCoincidentTopologyPolygonOffsetParameters(-1, -1);
	this->ActiveMapper->SetRelativeCoincidentTopologyPointOffsetParameter(-1);

	// Set up the initial properties
	this->CreateDefaultProperties();

	this->Actor = vtkActor::New();
	this->Actor->SetMapper(this->Mapper);
	this->Actor->SetProperty(this->Property);

	this->ActiveActor = vtkActor::New();
	this->ActiveActor->SetMapper(this->ActiveMapper);
	this->ActiveActor->SetProperty(this->ActiveProperty);

	this->Lines = vtkPolyData::New();
	this->LinesMapper = vtkPolyDataMapper::New();
	this->LinesMapper->SetInputData(this->Lines);
	vtkPolyDataMapper::SetResolveCoincidentTopologyToPolygonOffset();
	this->LinesMapper->SetRelativeCoincidentTopologyLineOffsetParameters(-1, -1);
	this->LinesMapper->SetRelativeCoincidentTopologyPolygonOffsetParameters(-1, -1);
	this->LinesMapper->SetRelativeCoincidentTopologyPointOffsetParameter(-1);

	this->LinesActor = vtkActor::New();
	this->LinesActor->SetMapper(this->LinesMapper);
	this->LinesActor->SetProperty(this->LinesProperty);

	this->InteractionOffset[0] = 0.0;
	this->InteractionOffset[1] = 0.0;

	this->AlwaysOnTop = 0;

	this->SelectedNodesPoints = nullptr;
	this->SelectedNodesData = nullptr;
	this->SelectedNodesCursorShape = nullptr;
	this->SelectedNodesGlypher = nullptr;
	this->SelectedNodesMapper = nullptr;
	this->SelectedNodesActor = nullptr;

	// initialize the members about the tube, by mc
	this->m_Tube = nullptr;
	this->m_TubeMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	this->m_TubeActor = vtkSmartPointer<vtkActor>::New();
	this->m_TubeActor->SetMapper(this->m_TubeMapper);
	this->m_TubeFilter = vtkSmartPointer<vtkTubeFilter>::New();
	this->m_TubeFilter->SetRadius(2.4);
	this->m_TubeFilter->SetNumberOfSides(20);
}

SplineTubeContourRepresentation::~SplineTubeContourRepresentation()
{
	/********** Destructor copied from vtkOrientedGlyphContourRepresentation.cxx **********/

	this->FocalPoint->Delete();
	this->FocalData->Delete();

	this->ActiveFocalPoint->Delete();
	this->ActiveFocalData->Delete();

	this->SetCursorShape(nullptr);
	this->SetActiveCursorShape(nullptr);

	this->Glypher->Delete();
	this->Mapper->Delete();
	this->Actor->Delete();

	this->ActiveGlypher->Delete();
	this->ActiveMapper->Delete();
	this->ActiveActor->Delete();

	this->Lines->Delete();
	this->LinesMapper->Delete();
	this->LinesActor->Delete();

	this->Property->Delete();
	this->ActiveProperty->Delete();
	this->LinesProperty->Delete();

	// Clear the selected nodes representation
	if (this->SelectedNodesPoints)
	{
		this->SelectedNodesPoints->Delete();
	}
	if (this->SelectedNodesData)
	{
		this->SelectedNodesData->Delete();
	}
	if (this->SelectedNodesCursorShape)
	{
		this->SelectedNodesCursorShape->Delete();
	}
	if (this->SelectedNodesGlypher)
	{
		this->SelectedNodesGlypher->Delete();
	}
	if (this->SelectedNodesMapper)
	{
		this->SelectedNodesMapper->Delete();
	}
	if (this->SelectedNodesActor)
	{
		this->SelectedNodesActor->Delete();
	}
}

void SplineTubeContourRepresentation::BuildLines()
{
	// copied from parent class

	vtkPoints* points = vtkPoints::New();
	vtkCellArray* lines = vtkCellArray::New();

	int i, j;
	vtkIdType index = 0;

	int count = this->GetNumberOfNodes();
	for (i = 0; i < this->GetNumberOfNodes(); i++)
	{
		count += this->GetNumberOfIntermediatePoints(i);
	}

	points->SetNumberOfPoints(count);
	vtkIdType numLines;

	if (this->ClosedLoop && count > 0)
	{
		numLines = count + 1;
	}
	else
	{
		numLines = count;
	}

	if (numLines > 0)
	{
		vtkIdType* lineIndices = new vtkIdType[numLines];

		double pos[3];
		for (i = 0; i < this->GetNumberOfNodes(); i++)
		{
			// Add the node
			this->GetNthNodeWorldPosition(i, pos);
			points->InsertPoint(index, pos);
			lineIndices[index] = index;
			index++;

			int numIntermediatePoints = this->GetNumberOfIntermediatePoints(i);

			for (j = 0; j < numIntermediatePoints; j++)
			{
				this->GetIntermediatePointWorldPosition(i, j, pos);
				points->InsertPoint(index, pos);
				lineIndices[index] = index;
				index++;
			}
		}

		if (this->ClosedLoop)
		{
			lineIndices[index] = 0;
		}

		lines->InsertNextCell(numLines, lineIndices);
		delete[] lineIndices;
	}

	this->Lines->SetPoints(points);
	this->Lines->SetLines(lines);

	points->Delete();
	lines->Delete();

	// build the tube with the lines, by mc
	this->m_TubeFilter->SetInputData(this->Lines);
	this->m_TubeFilter->Update();
	this->m_Tube = this->m_TubeFilter->GetOutput();
	this->m_TubeMapper->SetInputData(this->m_Tube);
}

void SplineTubeContourRepresentation::BuildRepresentation()
{
	// copied from parent class

	// Make sure we are up to date with any changes made in the placer
	this->UpdateContour();

	if (this->AlwaysOnTop)
	{
		// max value 65536 so we subtract 66000 to make sure we are
		// zero or negative
		this->LinesMapper->SetRelativeCoincidentTopologyLineOffsetParameters(0, -66000);
		this->LinesMapper->SetRelativeCoincidentTopologyPolygonOffsetParameters(0, -66000);
		this->LinesMapper->SetRelativeCoincidentTopologyPointOffsetParameter(-66000);
		this->Mapper->SetRelativeCoincidentTopologyLineOffsetParameters(0, -66000);
		this->Mapper->SetRelativeCoincidentTopologyPolygonOffsetParameters(0, -66000);
		this->Mapper->SetRelativeCoincidentTopologyPointOffsetParameter(-66000);
		this->ActiveMapper->SetRelativeCoincidentTopologyLineOffsetParameters(0, -66000);
		this->ActiveMapper->SetRelativeCoincidentTopologyPolygonOffsetParameters(0, -66000);
		this->ActiveMapper->SetRelativeCoincidentTopologyPointOffsetParameter(-66000);

		// by mc
		this->m_TubeMapper->SetRelativeCoincidentTopologyLineOffsetParameters(0, -66000);
		this->m_TubeMapper->SetRelativeCoincidentTopologyPolygonOffsetParameters(0, -66000);
		this->m_TubeMapper->SetRelativeCoincidentTopologyPointOffsetParameter(-66000);
	}
	else
	{
		this->LinesMapper->SetRelativeCoincidentTopologyLineOffsetParameters(-1, -1);
		this->LinesMapper->SetRelativeCoincidentTopologyPolygonOffsetParameters(-1, -1);
		this->LinesMapper->SetRelativeCoincidentTopologyPointOffsetParameter(-1);
		this->Mapper->SetRelativeCoincidentTopologyLineOffsetParameters(-1, -1);
		this->Mapper->SetRelativeCoincidentTopologyPolygonOffsetParameters(-1, -1);
		this->Mapper->SetRelativeCoincidentTopologyPointOffsetParameter(-1);
		this->ActiveMapper->SetRelativeCoincidentTopologyLineOffsetParameters(-1, -1);
		this->ActiveMapper->SetRelativeCoincidentTopologyPolygonOffsetParameters(-1, -1);
		this->ActiveMapper->SetRelativeCoincidentTopologyPointOffsetParameter(-1);

		// by mc
		this->m_TubeMapper->SetRelativeCoincidentTopologyLineOffsetParameters(0, -66000);
		this->m_TubeMapper->SetRelativeCoincidentTopologyPolygonOffsetParameters(0, -66000);
		this->m_TubeMapper->SetRelativeCoincidentTopologyPointOffsetParameter(-66000);
	}

	double p1[4], p2[4];
	this->Renderer->GetActiveCamera()->GetFocalPoint(p1);
	p1[3] = 1.0;
	this->Renderer->SetWorldPoint(p1);
	this->Renderer->WorldToView();
	this->Renderer->GetViewPoint(p1);

	double depth = p1[2];
	double aspect[2];
	this->Renderer->ComputeAspect();
	this->Renderer->GetAspect(aspect);

	p1[0] = -aspect[0];
	p1[1] = -aspect[1];
	this->Renderer->SetViewPoint(p1);
	this->Renderer->ViewToWorld();
	this->Renderer->GetWorldPoint(p1);

	p2[0] = aspect[0];
	p2[1] = aspect[1];
	p2[2] = depth;
	p2[3] = 1.0;
	this->Renderer->SetViewPoint(p2);
	this->Renderer->ViewToWorld();
	this->Renderer->GetWorldPoint(p2);

	double distance = sqrt(vtkMath::Distance2BetweenPoints(p1, p2));

	const int* size = this->Renderer->GetRenderWindow()->GetSize();
	double viewport[4];
	this->Renderer->GetViewport(viewport);

	double x, y, scale;

	x = size[0] * (viewport[2] - viewport[0]);
	y = size[1] * (viewport[3] - viewport[1]);

	scale = sqrt(x * x + y * y);

	distance = 1000 * distance / scale;

	this->Glypher->SetScaleFactor(distance * this->HandleSize);
	this->ActiveGlypher->SetScaleFactor(distance * this->HandleSize);
	int numPoints = this->GetNumberOfNodes();
	int i;
	if (this->ShowSelectedNodes && this->SelectedNodesGlypher)
	{
		this->SelectedNodesGlypher->SetScaleFactor(distance * this->HandleSize);
		this->FocalPoint->Reset();
		this->FocalPoint->SetNumberOfPoints(0);
		this->FocalData->GetPointData()->GetNormals()->SetNumberOfTuples(0);
		this->SelectedNodesPoints->Reset();
		this->SelectedNodesPoints->SetNumberOfPoints(0);
		this->SelectedNodesData->GetPointData()->GetNormals()->SetNumberOfTuples(0);
		for (i = 0; i < numPoints; i++)
		{
			if (i != this->ActiveNode)
			{
				double worldPos[3];
				double worldOrient[9];
				this->GetNthNodeWorldPosition(i, worldPos);
				this->GetNthNodeWorldOrientation(i, worldOrient);
				if (this->GetNthNodeSelected(i))
				{
					this->SelectedNodesPoints->InsertNextPoint(worldPos);
					this->SelectedNodesData->GetPointData()->GetNormals()->InsertNextTuple(worldOrient + 6);
				}
				else
				{
					this->FocalPoint->InsertNextPoint(worldPos);
					this->FocalData->GetPointData()->GetNormals()->InsertNextTuple(worldOrient + 6);
				}
			}
		}
		this->SelectedNodesPoints->Modified();
		this->SelectedNodesData->GetPointData()->GetNormals()->Modified();
		this->SelectedNodesData->Modified();
	}
	else
	{
		if (this->ActiveNode >= 0 && this->ActiveNode < this->GetNumberOfNodes())
		{
			this->FocalPoint->SetNumberOfPoints(numPoints - 1);
			this->FocalData->GetPointData()->GetNormals()->SetNumberOfTuples(numPoints - 1);
		}
		else
		{
			this->FocalPoint->SetNumberOfPoints(numPoints);
			this->FocalData->GetPointData()->GetNormals()->SetNumberOfTuples(numPoints);
		}
		int idx = 0;
		for (i = 0; i < numPoints; i++)
		{
			if (i != this->ActiveNode)
			{
				double worldPos[3];
				double worldOrient[9];
				this->GetNthNodeWorldPosition(i, worldPos);
				this->GetNthNodeWorldOrientation(i, worldOrient);
				this->FocalPoint->SetPoint(idx, worldPos);
				this->FocalData->GetPointData()->GetNormals()->SetTuple(idx, worldOrient + 6);
				idx++;
			}
		}
	}

	this->FocalPoint->Modified();
	this->FocalData->GetPointData()->GetNormals()->Modified();
	this->FocalData->Modified();

	if (this->ActiveNode >= 0 && this->ActiveNode < this->GetNumberOfNodes())
	{
		double worldPos[3];
		double worldOrient[9];
		this->GetNthNodeWorldPosition(this->ActiveNode, worldPos);
		this->GetNthNodeWorldOrientation(this->ActiveNode, worldOrient);
		this->ActiveFocalPoint->SetPoint(0, worldPos);
		this->ActiveFocalData->GetPointData()->GetNormals()->SetTuple(0, worldOrient + 6);

		this->ActiveFocalPoint->Modified();
		this->ActiveFocalData->GetPointData()->GetNormals()->Modified();
		this->ActiveFocalData->Modified();
		this->ActiveActor->VisibilityOn();
	}
	else
	{
		this->ActiveActor->VisibilityOff();
	}
}

void SplineTubeContourRepresentation::GetActors(vtkPropCollection* pc)
{
	// copied from parent class
	this->Actor->GetActors(pc);
	this->ActiveActor->GetActors(pc);
	this->LinesActor->GetActors(pc);
	if (this->ShowSelectedNodes && this->SelectedNodesActor)
	{
		this->SelectedNodesActor->GetActors(pc);
	}

	// by mc
	this->m_TubeActor->GetActors(pc);
}

void SplineTubeContourRepresentation::ReleaseGraphicsResources(vtkWindow* win)
{
	// copied from parent class
	this->Actor->ReleaseGraphicsResources(win);
	this->ActiveActor->ReleaseGraphicsResources(win);
	this->LinesActor->ReleaseGraphicsResources(win);

	// by mc
	this->m_TubeActor->ReleaseGraphicsResources(win);
}

int SplineTubeContourRepresentation::RenderOverlay(vtkViewport * viewport)
{
	// copied from parent class

	int count = 0;
	count += this->LinesActor->RenderOverlay(viewport);
	if (this->Actor->GetVisibility())
	{
		count += this->Actor->RenderOverlay(viewport);
	}
	if (this->ActiveActor->GetVisibility())
	{
		count += this->ActiveActor->RenderOverlay(viewport);
	}

	// by mc
	if (this->m_TubeActor->GetVisibility())
	{
		count += this->m_TubeActor->RenderOverlay(viewport);
	}

	return count;
}

int SplineTubeContourRepresentation::RenderOpaqueGeometry(vtkViewport * viewport)
{
	// copied from parent class

	// Since we know RenderOpaqueGeometry gets called first, will do the
	// build here
	this->BuildRepresentation();

	int count = 0;
	count += this->LinesActor->RenderOpaqueGeometry(viewport);
	if (this->Actor->GetVisibility())
	{
		count += this->Actor->RenderOpaqueGeometry(viewport);
	}
	if (this->ActiveActor->GetVisibility())
	{
		count += this->ActiveActor->RenderOpaqueGeometry(viewport);
	}
	if (this->ShowSelectedNodes && this->SelectedNodesActor &&
		this->SelectedNodesActor->GetVisibility())
	{
		count += this->SelectedNodesActor->RenderOpaqueGeometry(viewport);
	}

	// by mc
	if (this->m_TubeActor->GetVisibility())
	{
		count += this->m_TubeActor->RenderOpaqueGeometry(viewport);
	}

	return count;
}

int SplineTubeContourRepresentation::RenderTranslucentPolygonalGeometry(vtkViewport * viewport)
{
	// copied from parent class

	int count = 0;
	count += this->LinesActor->RenderTranslucentPolygonalGeometry(viewport);
	if (this->Actor->GetVisibility())
	{
		count += this->Actor->RenderTranslucentPolygonalGeometry(viewport);
	}
	if (this->ActiveActor->GetVisibility())
	{
		count += this->ActiveActor->RenderTranslucentPolygonalGeometry(viewport);
	}

	// by mc
	if (this->m_TubeActor->GetVisibility())
	{
		count += this->m_TubeActor->RenderTranslucentPolygonalGeometry(viewport);
	}

	return count;
}

vtkTypeBool SplineTubeContourRepresentation::HasTranslucentPolygonalGeometry()
{
	// copied from parent class

	int result = 0;
	result |= this->LinesActor->HasTranslucentPolygonalGeometry();
	if (this->Actor->GetVisibility())
	{
		result |= this->Actor->HasTranslucentPolygonalGeometry();
	}
	if (this->ActiveActor->GetVisibility())
	{
		result |= this->ActiveActor->HasTranslucentPolygonalGeometry();
	}

	// by mc
	if (this->m_TubeActor->GetVisibility())
	{
		result |= this->m_TubeActor->HasTranslucentPolygonalGeometry();
	}

	return result;
}
