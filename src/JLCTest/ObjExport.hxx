#ifndef _ObjExport_HeaderFile
#define _ObjExport_HeaderFile

#include <vector>
#include <XCAFTemplate.hxx>
#include <Standard_Type.hxx>
#include <TopTools_MapOfShape.hxx>
#define MAX2(X, Y) (Abs(X) > Abs(Y) ? Abs(X) : Abs(Y))
#define MAX3(X, Y, Z) (MAX2(MAX2(X, Y), Z))

class ObjExport : public XCAFTemplate
{
public:
    ObjExport();
    void assemShape2obj(const TopoDS_Shape& shape, std::string exportPath, Standard_Boolean isMultiFile);
private:
    void GenMtlData(Standard_OStream& mtlOS);

    Standard_Integer Shape2Obj(TopoDS_Shape& S,
                                TopLoc_Location& location,
                                TopTools_MapOfShape& theFacesMap,
                                Standard_Integer totalnodes,
                                std::ostream& theOStream);

    TCollection_ExtendedString GetLabelName(const TDF_Label& lab);

    void GetSubShapes( const TDF_Label& lab,
                            std::vector<TopLoc_Location>& locs,
                            std::vector<TCollection_ExtendedString>& groups,
                            Standard_CString& prevColorName,
                            Standard_Integer subLevel,
                            Standard_Integer& totalNodes,
                            Standard_OStream& OS);

};
#endif // _ObjExport_HeaderFile