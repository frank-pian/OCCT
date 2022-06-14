#ifndef _JLCTest_HeaderFile
#define _JLCTest_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Draw_Interpretor.hxx>

class JLCTest
{
public:
    DEFINE_STANDARD_ALLOC

    Standard_EXPORT static void CommonCommands(Draw_Interpretor &theCommands);

    //! Loads all JLC Draw commands. Used for plugin.
    Standard_EXPORT static void Factory(Draw_Interpretor &theCommands);
};
#endif // _JLCTest_HeaderFile
