// File:      OpenGl_GraphicDriver_8.cxx
// Created:   20 October 2011
// Author:    Sergey ZERCHANINOV
// Copyright: OPEN CASCADE 2011

#include <OpenGl_GraphicDriver.hxx>

#include <OpenGl_CView.hxx>
#include <OpenGl_Structure.hxx>

static Handle(OpenGl_Workspace) myImmediateWS;

/*----------------------------------------------------------------------*/

Standard_Boolean OpenGl_GraphicDriver::BeginAddMode (const Graphic3d_CView& ACView)
{
  if (ACView.ViewId == -1)
    return Standard_False;

  const OpenGl_CView *aCView = (const OpenGl_CView *)ACView.ptrView;
  if (aCView && aCView->WS->BeginAddMode())
  {
    myImmediateWS = aCView->WS;
    return Standard_True;
  }

  return Standard_False;
}

void OpenGl_GraphicDriver::EndAddMode (void)
{
  if (!myImmediateWS.IsNull())
  {
    myImmediateWS->EndAddMode();
    myImmediateWS.Nullify();
  }
}

Standard_Boolean OpenGl_GraphicDriver::BeginImmediatMode (const Graphic3d_CView& ACView, const Aspect_CLayer2d& ACUnderLayer, const Aspect_CLayer2d& ACOverLayer, const Standard_Boolean DoubleBuffer, const Standard_Boolean RetainMode)
{
  if (ACView.ViewId == -1)
    return Standard_False;

  const OpenGl_CView *aCView = (const OpenGl_CView *)ACView.ptrView;
  if (aCView && aCView->WS->BeginImmediatMode(ACView, DoubleBuffer, RetainMode))
  {
    myImmediateWS = aCView->WS;
    return Standard_True;
  }

  return Standard_False;
}

void OpenGl_GraphicDriver::BeginPolyline ()
{
  if (!myImmediateWS.IsNull())
    myImmediateWS->BeginPolyline();
}

void OpenGl_GraphicDriver::ClearImmediatMode (const Graphic3d_CView& ACView, const Standard_Boolean aFlush)
{
  const OpenGl_CView *aCView = (const OpenGl_CView *)ACView.ptrView;
  if (aCView)
    aCView->WS->ClearImmediatMode(ACView,aFlush);
}

void OpenGl_GraphicDriver::Draw (const Standard_ShortReal X, const Standard_ShortReal Y, const Standard_ShortReal Z)
{
  if (!myImmediateWS.IsNull())
    myImmediateWS->Draw(X,Y,Z);
}

void OpenGl_GraphicDriver::DrawStructure (const Graphic3d_CStructure& ACStructure)
{
  OpenGl_Structure *astructure = (OpenGl_Structure *)ACStructure.ptrStructure;
  if (!astructure)
    return;

  if (!myImmediateWS.IsNull())
    myImmediateWS->DrawStructure(astructure);
}

void OpenGl_GraphicDriver::EndImmediatMode (const Standard_Integer Synchronize)
{
  if (!myImmediateWS.IsNull())
  {
    myImmediateWS->EndImmediatMode();
    myImmediateWS.Nullify();
  }
}

void OpenGl_GraphicDriver::EndPolyline ()
{
  if (!myImmediateWS.IsNull())
    myImmediateWS->EndPolyline();
}

void OpenGl_GraphicDriver::Move (const Standard_ShortReal X, const Standard_ShortReal Y, const Standard_ShortReal Z)
{
  if (!myImmediateWS.IsNull())
    myImmediateWS->Move(X,Y,Z);
}

void OpenGl_GraphicDriver::SetLineColor (const Standard_ShortReal R, const Standard_ShortReal G, const Standard_ShortReal B)
{
  if (!myImmediateWS.IsNull())
    myImmediateWS->SetLineColor(R,G,B);
}

void OpenGl_GraphicDriver::SetLineType (const Standard_Integer Type)
{
  if (!myImmediateWS.IsNull())
    myImmediateWS->SetLineType(Type);
}

void OpenGl_GraphicDriver::SetLineWidth (const Standard_ShortReal Width)
{
  if (!myImmediateWS.IsNull())
    myImmediateWS->SetLineWidth(Width);
}

void OpenGl_GraphicDriver::SetMinMax (const Standard_ShortReal X1, const Standard_ShortReal Y1, const Standard_ShortReal Z1, const Standard_ShortReal X2, const Standard_ShortReal Y2, const Standard_ShortReal Z2)
{
  if (!myImmediateWS.IsNull())
    myImmediateWS->SetMinMax(X1,Y1,Z1,X2,Y2,Z2);
}

void OpenGl_GraphicDriver::Transform (const TColStd_Array2OfReal& AMatrix, const Graphic3d_TypeOfComposition AType)
{
  if (!myImmediateWS.IsNull())
    myImmediateWS->Transform(AMatrix,AType);
}
