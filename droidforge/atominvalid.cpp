#include "atominvalid.h"

AtomInvalid *AtomInvalid::clone() const
{
    return new AtomInvalid(rawtext);
}
