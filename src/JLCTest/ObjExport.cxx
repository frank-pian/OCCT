#include <ObjExport.hxx>

#include <Standard_Type.hxx>
#include <Quantity_Color.hxx>
#include <Quantity_ColorRGBA.hxx>
#include <TDataStd_Name.hxx>
#include <TopTools_MapOfShape.hxx>
#include <Bnd_Box.hxx>
#include <BRepBndLib.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <TopExp_Explorer.hxx>
#include <CSLib.hxx>
#include <BRep_Builder.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <TopoDS.hxx>

ObjExport::ObjExport():XCAFTemplate()
{

}

Standard_Integer ObjExport::Shape2Obj(TopoDS_Shape& S,
                                      TopLoc_Location& location,
                                      TopTools_MapOfShape& theFacesMap,
                                      Standard_Integer totalnodes,
                                      std::ostream& theOStream)
{
    if (S.IsNull()) return 1;

    TopLoc_Location L;
    TopExp_Explorer ex;

    Standard_Boolean OK = Standard_True;

    gp_Vec D1U,D1V;
    gp_Vec D2U,D2V,D2UV;
    gp_Dir Nor;
    gp_Pnt P;

    Standard_Real U, V;
    CSLib_DerivativeStatus aStatus;
    CSLib_NormalStatus NStat;

    Standard_Real x, y, z;
    Standard_Integer n1, n2, n3;
    Standard_Integer k1, k2, k3;

    Standard_Integer nbNodes, nbpolygons = 0;

    Standard_Integer i, nbface = 0;
    for (ex.Init(S, TopAbs_FACE); ex.More(); ex.Next())
    {
        if(!theFacesMap.Add(ex.Current()))
            continue;

        nbface++;
        TopoDS_Face F = TopoDS::Face(ex.Current());
        Handle(Poly_Triangulation) Tr = BRep_Tool::Triangulation(F, L);
        L = L.Multiplied(location);

        // nodes
        if (!Tr.IsNull())
        {
            nbNodes = Tr->NbNodes();
            const Poly_ArrayOfNodes& Nodes = Tr->InternalNodes();

            theOStream << "\n# NumberOfVertex: " << nbNodes << std::endl;
            for (i = Nodes.Lower(); i <= Nodes.Upper(); i++)
            {
                gp_Pnt Pnt = Nodes.Value(i).Transformed(L.Transformation());
                theOStream << "v " << Pnt.X() << " " <<Pnt.Y() << " " << Pnt.Z() << std::endl;
            }

            if (Tr->HasUVNodes())
            {
                const Poly_ArrayOfUVNodes& UVNodes = Tr->InternalUVNodes();
                BRepAdaptor_Surface BS(F, Standard_False);

                theOStream << "\n# NumberOfVertexNormals: "<<nbNodes<<std::endl;
                for (i = UVNodes.Lower(); i <= UVNodes.Upper(); i++)
                {
                    U = UVNodes.Value(i).X();
                    V = UVNodes.Value(i).Y();

                    BS.D1(U,V,P,D1U,D1V);
                    CSLib::Normal (D1U, D1V, Precision::Angular(), aStatus, Nor);
                    if (aStatus != CSLib_Done)
                    {
                        BS.D2(U,V,P,D1U,D1V,D2U,D2V,D2UV);
                        CSLib::Normal(D1U,D1V,D2U,D2V,D2UV,Precision::Angular(),OK,NStat,Nor);
                    }
                    if (F.Orientation() == TopAbs_REVERSED) Nor.Reverse();

                    theOStream <<"vn "<<Nor.X()<<" "<<Nor.Y()<<" "<<Nor.Z()<<std::endl;
                }

            }

            //outfile<<"\n# NumberOfSmoothGroups: "<<nbface<<std::endl;
            //outfile<<"s "<<nbface<<std::endl;

            // triangles.
            Standard_Integer nbTriangles = Tr->NbTriangles();
            const Poly_Array1OfTriangle& triangles = Tr->InternalTriangles();

            theOStream << "\n# numberOfTriangles: "<<nbTriangles<<std::endl;
            for (i = 1; i <= nbTriangles; i++)
            {
                if (F.Orientation()  == TopAbs_REVERSED)
                  triangles(i).Get(n1, n3, n2);
                else
                  triangles(i).Get(n1, n2, n3);
                k1 = n1+totalnodes;
                k2 = n2+totalnodes;
                k3 = n3+totalnodes;

                theOStream << "f "<<k1<<"//"<<k1<<" "<<
                               k2<<"//"<<k2<<" "<<
                               k3<<"//"<<k3<< std::endl;
            }

            nbpolygons += nbTriangles;
            totalnodes += nbNodes;

        }
    }

#ifndef WASM_VER //linux main
    std::cout<<"\r" <<totalnodes << " nodes";
    std::cout.flush();
#endif

    return totalnodes;
}

TCollection_ExtendedString ObjExport::GetLabelName(const TDF_Label& lab)
{
    Handle(TDataStd_Name) N;
    if (lab.FindAttribute(TDataStd_Name::GetID(), N))
    {
        TCollection_ExtendedString name = N->Get();
        return name;
    }else {
        return TCollection_ExtendedString("NoName");
    }
}

void ObjExport::GetSubShapes( const TDF_Label& lab,
                            std::vector<TopLoc_Location>& locs,
                            std::vector<TCollection_ExtendedString>& groups,
                            Standard_CString& prevColorName,
                            Standard_Integer subLevel,
                            Standard_Integer& totalNodes,
                            Standard_OStream& OS)
{
    subLevel++;
    if (this->myShapeTool->IsAssembly(lab))
    {
        TDF_LabelSequence l_a;
        this->myShapeTool->GetComponents(lab, l_a);

        for (Standard_Integer i=0; i<l_a.Length(); i++)
        {
            const TDF_Label& label = l_a.Value(i+1);
            if (this->myShapeTool->IsReference(label))
            {
                groups.push_back(GetLabelName(lab));
                TDF_Label label_reference;
                this->myShapeTool->GetReferredShape(label, label_reference);
                TopLoc_Location loc = this->myShapeTool->GetLocation(label);
                locs.push_back(loc);
                GetSubShapes(label_reference, locs, groups, prevColorName, subLevel, totalNodes, OS);
                groups.pop_back();
                locs.pop_back();
            }
        }
    }else if (this->myShapeTool->IsSimpleShape(lab))
    {
        OS << "\ng";
        for (auto g=groups.begin(); g!=groups.end(); ++g)
        {
            OS << " " << *g;
        }

        TopLoc_Location loc;
        for (auto l=locs.begin(); l!=locs.end(); ++l)
        {
            loc = loc.Multiplied(*l);
        }

        TopTools_MapOfShape theFacesMap;
        TopoDS_Shape dispShape(this->myShapeTool->GetShape(lab));

        Bnd_Box B;
        Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
        BRepBndLib::Add(dispShape, B);
        B.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
        Standard_Real aDeflection = MAX3( aXmax-aXmin, aYmax-aYmin, aZmax-aZmin) * 0.01;

        BRepMesh_IncrementalMesh aMesh (dispShape, aDeflection);

        // 对于a label of simple shape遍历策略
        // 先检查是否有subshapes,如果有那么就基于subshapes进行颜色匹配
        // 如果没有subshapes,那么直接采用shape颜色
        TDF_LabelSequence l_c;
        if (this->myShapeTool->GetSubShapes(lab, l_c))
        {
            for (Standard_Integer i=0; i<l_c.Length(); i++)
            {
                const TDF_Label& subLabel = l_c.Value(i+1);

                TopoDS_Shape subDispShape(this->myShapeTool->GetShape(subLabel));
                if (subDispShape.ShapeType() != TopAbs_WIRE &&
                    subDispShape.ShapeType() != TopAbs_EDGE &&
                    subDispShape.ShapeType() != TopAbs_VERTEX)
                {
                    Quantity_ColorRGBA c;
                    if (this->myColorTool->GetColor(subLabel, XCAFDoc_ColorGen, c) ||
                        this->myColorTool->GetColor(subLabel, XCAFDoc_ColorSurf, c))
                    {
                        Standard_CString colorName = Quantity_Color::StringName(c.GetRGB().Name());
                        if (colorName != prevColorName)
                        {
                            OS << "\nusemtl " << colorName << std::endl;
                            prevColorName = colorName;
                        }
                    }else
                    {
                        if (std::strcmp("Default", prevColorName) != 0)
                        {
                            OS << "\nusemtl " << "Default" << std::endl;
                            prevColorName = "Default";
                        }
                    }

                    totalNodes = Shape2Obj(subDispShape, loc, theFacesMap, totalNodes, OS);
                }
            }
        }

        {
            if ( dispShape.ShapeType() != TopAbs_WIRE &&
                 dispShape.ShapeType() != TopAbs_EDGE &&
                 dispShape.ShapeType() != TopAbs_VERTEX)
            {
                OS << "\no " << GetLabelName(lab) << std::endl;
                Quantity_ColorRGBA c;
                if (this->myColorTool->GetColor(lab, XCAFDoc_ColorGen, c) ||
                        this->myColorTool->GetColor(lab, XCAFDoc_ColorSurf, c) )
                {

                    Standard_CString colorName=  Quantity_Color::StringName(c.GetRGB().Name());
                    if ( colorName != prevColorName)
                    {
                        OS << "\nusemtl " << colorName << std::endl;
                        prevColorName =  colorName;
                    }
                }
                else
                {
                    if ( std::strcmp("Default", prevColorName) !=0)
                    {
                        OS << "\nusemtl " << "Default" << std::endl;
                        prevColorName =  "Default";
                    }
                }

                totalNodes = Shape2Obj(dispShape, loc, theFacesMap, totalNodes, OS);
            }
        }
    }
}

void ObjExport::GenMtlData(Standard_OStream& mtlOS)
{
    mtlOS << "\nnewmtl " << "Default" << std::endl;
    mtlOS << "Ka 0.0 0.0 0.0" << std::endl;
    mtlOS << "Kd 0.8 0.8 0.8" << std::endl;
    mtlOS << "Ks 0.0 0.0 0.0" << std::endl;
    mtlOS << "d 1" << std::endl;
    mtlOS << "endmtl" << std::endl;

    TDF_LabelSequence theColorLabelSeq;
    this->myColorTool->GetColors(theColorLabelSeq);
    mtlOS << "\n# NumberOfOtherColors: "<< theColorLabelSeq.Length() << std::endl;
    for (Standard_Integer i = 0; i < theColorLabelSeq.Length(); ++i)
    {
        Quantity_ColorRGBA aColor;
        if (this->myColorTool->GetColor(theColorLabelSeq.Value(i+1), aColor))
        {
            const Quantity_Color& aRgb = aColor.GetRGB();
            mtlOS << "newmtl "<< Quantity_Color::StringName(aRgb.Name()) << std::endl;
            mtlOS << "Ka 0.0 0.0 0.0" << std::endl;
            mtlOS << "Kd " << aRgb.Red() << " " << aRgb.Green() << " " << aRgb.Blue() << std::endl;
            mtlOS << "Ks 0.0 0.0 0.0" << std::endl;
            mtlOS << "d " << aColor.Alpha() << std::endl;
            mtlOS << "endmtl" << std::endl;
        }
    }

}

void ObjExport::assemShape2obj(const TopoDS_Shape& shape, std::string exportPath, Standard_Boolean isMultiFile)
{
    std::ofstream outfile(exportPath + ".obj");

    this->myShapeTool->AddShape(shape, false);
    outfile <<"# Designed by EasyEDA Pro" << std::endl;

    if (!isMultiFile){
        this->GenMtlData(outfile);
    }else {
        std::ofstream outMtlFile( exportPath + ".mtl" );
        outMtlFile <<"# Designed by EasyEDA Pro" << std::endl;
        this->GenMtlData(outMtlFile);
        outMtlFile.close();
    }

    std::vector<TopLoc_Location> locs;
    std::vector<TCollection_ExtendedString> groups;
    int totalnodes=0;
    Standard_CString prevColorName="";

    TDF_LabelSequence theShapeLabelSeq;
    this->myShapeTool->GetFreeShapes(theShapeLabelSeq);

    for (Standard_Integer i = 0; i < theShapeLabelSeq.Length(); ++i)
    {
        const TDF_Label& rootItem = theShapeLabelSeq.Value(i+1);
        int sublevel=0;
        GetSubShapes( rootItem,
                      locs, groups,prevColorName,
                      sublevel, totalnodes, outfile );
    }

    outfile.close();
    std::cout << "\ndone!"<<std::endl;
}