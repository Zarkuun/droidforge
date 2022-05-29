#include "registertypes.h"

// The order of the register types determines which one
// is detected in the rackview when we click on it.
// We prefer registers suitable for input here. And remap
// them to outputs when neccessary.
// So INPUT is before NORMALIZE and BUTTON before LED.
const char register_types[NUM_REGISTER_TYPES] = {
  // Can only be used as inputs
  REGISTER_INPUT,
  REGISTER_POT,
  REGISTER_BUTTON,
  REGISTER_SWITCH,

  // Can be both input or output
  REGISTER_GATE,

  // Usually used as outputs
  REGISTER_NORMALIZE,
  REGISTER_OUTPUT,
  REGISTER_LED,
  REGISTER_RGB,
  REGISTER_EXTRA,
};

const char *register_names[NUM_REGISTER_TYPES] = {
    "Input",
    "Potentiometer",
    "Button",
    "Switch",
    "Gate",
    "Input-Normalization",
    "Output",
    "LED",
    "RGB-LED",
    "Extra",
};

const char *registerName(char registerType)
{
    for (unsigned i=0; i<NUM_REGISTER_TYPES; i++)
        if (register_types[i] == registerType)
            return register_names[i];
    return "(invalid)";
}
