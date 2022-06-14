#include <JLCTest.hxx>

#include <Draw_PluginMacro.hxx>

static Standard_Integer JLCTestCommand(Draw_Interpretor &theInterpretor,
                                       Standard_Integer theArgNb,
                                       const char **theArgs)
{
    theInterpretor << "Test JLC Command Package Loaded Success";
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
