#ifndef _XCAFTemplate_HeaderFile
#define _XCAFTemplate_HeaderFile

#include <XCAFApp_Application.hxx>
#include <TDocStd_Document.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_ColorTool.hxx>

class XCAFTemplate
{
public: 
    XCAFTemplate();

protected:
    Handle_XCAFApp_Application myApp;
    Handle_TDocStd_Document myDoc;
    Handle_XCAFDoc_ShapeTool myShapeTool;
    Handle_XCAFDoc_ColorTool myColorTool;
};

#endif // _XCAFTemplate_HeaderFile