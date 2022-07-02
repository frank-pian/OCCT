#include <JLCTest.hxx>

#include <Draw_PluginMacro.hxx>
#include <Message.hxx>
#include <TopoDS_Shape.hxx>
#include <DBRep.hxx>
#include <BRepExtrema_ShapeProximity.hxx>
#include <Draw.hxx>
#include <OSD_Timer.hxx>
#include <TopExp_Explorer.hxx>
#include <ObjExport.hxx>

static Standard_Integer JLCTestCommand(Draw_Interpretor &di,
                                       Standard_Integer argc,
                                       const char **theArgs)
{
    di << "Test JLC Command Package Loaded Success";
    return 0;
}

static int LCProximity (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3 || argc > 6)
  {
    Message::SendFail() << "Usage: " << argv[0] << " Shape1 Shape2 [-tol <value>] [-profile]";
    return 1;
  }

  TopoDS_Shape aShape1 = DBRep::Get (argv[1]);
  TopoDS_Shape aShape2 = DBRep::Get (argv[2]);

  if (aShape1.IsNull() || aShape2.IsNull())
  {
    Message::SendFail() << "Error: Failed to find specified shapes";
    return 1;
  }

  BRepExtrema_ShapeProximity aTool;

  Standard_Boolean aProfile = Standard_False;

  for (Standard_Integer anArgIdx = 3; anArgIdx < argc; ++anArgIdx)
  {
    TCollection_AsciiString aFlag (argv[anArgIdx]);
    aFlag.LowerCase();

    if (aFlag == "-tol")
    {
      if (++anArgIdx >= argc)
      {
        Message::SendFail() << "Error: wrong syntax at argument '" << aFlag;
        return 1;
      }

      const Standard_Real aTolerance = Draw::Atof (argv[anArgIdx]);
      if (aTolerance < 0.0)
      {
        Message::SendFail() << "Error: Tolerance value should be non-negative";
        return 1;
      }
      else
      {
        aTool.SetTolerance (aTolerance);
      }
    }

    if (aFlag == "-profile")
    {
      aProfile = Standard_True;
    }
  }

  Standard_Real aInitTime = 0.0;
  Standard_Real aWorkTime = 0.0;

  OSD_Timer aTimer;

  if (aProfile)
  {
    aTimer.Start();
  }

  aTool.LoadShape1 (aShape1);
  aTool.LoadShape2 (aShape2);

  if (aProfile)
  {
    aInitTime = aTimer.ElapsedTime();
    aTimer.Reset();
    aTimer.Start();
  }

  // Perform shape proximity test
  aTool.Perform();

  if (aProfile)
  {
    aWorkTime = aTimer.ElapsedTime();
    aTimer.Stop();
  }

  if (aTool.IsDone())
  {
    di << 1 << "";
  }else {
    di << 0 << "";
  }

  return 0;
}

static Standard_Integer LCWriteObj(Draw_Interpretor& di,
                                Standard_Integer argc,
                                const char** argv)
{
    TopoDS_Shape aShape = DBRep::Get(argv[1]);

    Standard_Boolean isMultiFile = Standard_False;
    Standard_Integer num = 1;
    if (argc > 3)
    {
        TCollection_AsciiString aVisArg (argv[3]);
        if (aVisArg == "0")
        {
            isMultiFile = Standard_False;
        }else if (aVisArg == "1") {
            isMultiFile = Standard_True;
        }
    }
    for (TopExp_Explorer aExpSolid(aShape, TopAbs_SOLID); aExpSolid.More(); aExpSolid.Next())
    {
        std::ostringstream path;
        path << argv[2] << "-" << num;
        std::string path_s = path.str();
        di << path_s.c_str() << "\n";
        std::unique_ptr<ObjExport> theObjExport(new ObjExport);
        theObjExport->assemShape2obj(aExpSolid.Current(), path_s, isMultiFile);
        num ++;
    }
    return 0;
}

//=======================================================================
// function : CommonCommands
// purpose  :
//=======================================================================
void JLCTest::CommonCommands(Draw_Interpretor &theCommands)
{
    const char *group = "JLC_Commands";

    theCommands.Add("JLCTestCommand",
                    "JLCTestCommand to test whether the package is loaded",
                    __FILE__,
                    JLCTestCommand,
                    group);

    theCommands.Add("LCProximity",
                    "proximity Shape1 Shape2 [-tol <value>] [-profile]",
                    __FILE__,
                    LCProximity,
                    group);
    
    theCommands.Add("LCWriteObj",
                    "Shape write Obj file, mutiple files",
                    __FILE__,
                    LCWriteObj,
                    group);
}

//==============================================================================
// JLCTest::Factory
//==============================================================================
void JLCTest::Factory(Draw_Interpretor &theCommands)
{
    JLCTest::CommonCommands(theCommands);
}

// Declare entry point PLUGINFACTORY
DPLUGIN(JLCTest)
