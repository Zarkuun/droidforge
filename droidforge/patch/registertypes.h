#ifndef REGISTERTYPES_H
#define REGISTERTYPES_H

#define NUM_REGISTER_TYPES 10

#define    REGISTER_INPUT     'I'
#define    REGISTER_NORMALIZE 'N'
#define    REGISTER_OUTPUT    'O'
#define    REGISTER_GATE      'G'
#define    REGISTER_BUTTON    'B'
#define    REGISTER_LED       'L'
#define    REGISTER_POT       'P'
#define    REGISTER_SWITCH    'S'
#define    REGISTER_RGB       'R'
#define    REGISTER_EXTRA     'X'

extern const char register_types[];
const char *registerName(char registerType);

#endif // REGISTERTYPES_H
