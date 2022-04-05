#include "modulemaster.h"

ModuleMaster::ModuleMaster()
{

}

QString ModuleMaster::name()
{
    return "master";
}

QString ModuleMaster::faceplate()
{
    return "faceplate-master-off";

}

QString ModuleMaster::title()
{
    return "DROID master";
}

unsigned ModuleMaster::hp()
{
    return 8;
}
