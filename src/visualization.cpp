#include "vtkActor.h"
#include "vtkContourFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkSmoothPolyDataFilter.h"
#include "vtkPolyDataNormals.h"
#include "vtkRenderer.h"
#include "vtkLight.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkSmartPointer.h"
#include "vtkMNIObjectReader.h"
#include "vtkAutoInit.h"
#include "vtkImplicitPlaneWidget2.h"
#include "vtkImplicitPlaneRepresentation.h"
#include "vtkPlane.h"

VTK_MODULE_INIT(vtkRenderingOpenGL2)
VTK_MODULE_INIT(vtkInteractionStyle)

class PlaneMoveCallback : public vtkCommand
{
public:
  static PlaneMoveCallback* New()
  {
    return new PlaneMoveCallback;
  }
  virtual void Execute(vtkObject* caller, unsigned long, void*)
  {
    vtkImplicitPlaneWidget2* planeWidget =
      reinterpret_cast<vtkImplicitPlaneWidget2*>(caller);
    vtkImplicitPlaneRepresentation* rep =
      reinterpret_cast<vtkImplicitPlaneRepresentation*>(planeWidget->GetRepresentation());
    rep->GetPlane(this->Plane);
  }

  PlaneMoveCallback() :Plane(0), Actor(0) {}
  vtkPlane* Plane;
  vtkActor* Actor;
};

int main(int argc, char** argv)
{
  /* Figure for cutting */

  vtkSmartPointer<vtkPlane> plane =
    vtkSmartPointer<vtkPlane>::New();
  plane->SetOrigin(1.0, 1.5, 2.0);
  plane->SetNormal(0.4, 0.2, 1.0);

  /* Reader */

  vtkSmartPointer<vtkMNIObjectReader> reader =
    vtkSmartPointer<vtkMNIObjectReader>::New();
  reader->SetFileName("C:\\mygit\\VTK_practice\\data\\gray_right_327680.obj");
  reader->Update();

  /* Filters */

  vtkSmartPointer<vtkSmoothPolyDataFilter> smoothFilter =
    vtkSmartPointer<vtkSmoothPolyDataFilter>::New();
  smoothFilter->SetInputConnection(reader->GetOutputPort());
  smoothFilter->SetNumberOfIterations(4);
  smoothFilter->SetRelaxationFactor(0.5);
  smoothFilter->FeatureEdgeSmoothingOff();
  smoothFilter->BoundarySmoothingOn();
  smoothFilter->Update();

  vtkSmartPointer<vtkPolyDataNormals> normalGenerator = vtkSmartPointer<vtkPolyDataNormals>::New();
  normalGenerator->SetInputConnection(smoothFilter->GetOutputPort());
  normalGenerator->ComputePointNormalsOn();
  normalGenerator->ComputeCellNormalsOn();
  normalGenerator->Update();

  /* Mapper */

  vtkSmartPointer<vtkPolyDataMapper> mapper =
    vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputConnection(normalGenerator->GetOutputPort());
  mapper->AddClippingPlane(plane);

  /* Actor */

  vtkSmartPointer<vtkActor> actor =
    vtkSmartPointer<vtkActor>::New();
  actor->SetMapper(mapper);

  /* Renderer */

  vtkSmartPointer<vtkRenderer> renderer =
    vtkSmartPointer<vtkRenderer>::New();
  renderer->AddActor(actor);

  /* Light */
  vtkSmartPointer<vtkLight> light1 =
    vtkSmartPointer<vtkLight>::New();
  light1->SetColor(0.8, 0.6, 0.6);
  light1->SetPosition(-3.0, 0.0, 0.0);
  vtkSmartPointer<vtkLight> light2 =
    vtkSmartPointer<vtkLight>::New();
  light2->SetColor(1.0, 0.72, 0.72);
  light2->SetPosition(2.0, 0.0, 1.5);
  vtkSmartPointer<vtkLight> light3 =
    vtkSmartPointer<vtkLight>::New();
  light3->SetColor(0.8, 0.6, 0.6);
  light3->SetPosition(0.0, 0.0, -3.0);
  renderer->AddLight(light1);
  renderer->AddLight(light2);
  renderer->AddLight(light3);

  /* Window */

  vtkSmartPointer<vtkRenderWindow> renderWindow =
    vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->AddRenderer(renderer);

  /* Interactor */

  vtkSmartPointer<vtkRenderWindowInteractor> interactor =
    vtkSmartPointer<vtkRenderWindowInteractor>::New();
  interactor->SetRenderWindow(renderWindow);

  vtkSmartPointer<vtkInteractorStyleTrackballCamera> style =
    vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
  interactor->SetInteractorStyle(style);

  vtkSmartPointer<PlaneMoveCallback> myCallback =
    vtkSmartPointer<PlaneMoveCallback>::New();
  myCallback->Plane = plane;
  myCallback->Actor = actor;

  vtkSmartPointer<vtkImplicitPlaneRepresentation> rep =
    vtkSmartPointer<vtkImplicitPlaneRepresentation>::New();
  rep->SetPlaceFactor(1.25); // This must be set prior to placing the widget
  rep->PlaceWidget(actor->GetBounds());
  rep->SetNormal(plane->GetNormal());
  rep->SetOrigin(plane->GetOrigin());

  vtkSmartPointer<vtkImplicitPlaneWidget2> planeWidget =
    vtkSmartPointer<vtkImplicitPlaneWidget2>::New();
  planeWidget->SetInteractor(interactor);
  planeWidget->SetRepresentation(rep);
  planeWidget->AddObserver(vtkCommand::InteractionEvent, myCallback);
  planeWidget->On();

  /* Run pipeline */

  renderWindow->Render();
  interactor->Start();

  return 0;
}