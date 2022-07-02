#include <XCAFTemplate.hxx>

XCAFTemplate::XCAFTemplate()
{
    this->myApp = XCAFApp_Application::GetApplication();
    this->myApp->NewDocument("MDTV-XCAF", this->myDoc);
    this->myShapeTool = XCAFDoc_DocumentTool::ShapeTool(this->myDoc->Main());
    this->myColorTool = XCAFDoc_DocumentTool::ColorTool(this->myDoc->Main());
}