#ifndef REGISTERTYPES_H
#define REGISTERTYPES_H

#define NUM_REGISTER_TYPES 11

typedef char register_type_t;

#define    REGISTER_INPUT     'I'
#define    REGISTER_NORMALIZE 'N'
#define    REGISTER_OUTPUT    'O'
#define    REGISTER_GATE      'G'
#define    REGISTER_BUTTON    'B'
#define    REGISTER_LED       'L'
#define    REGISTER_POT       'P'
#define    REGISTER_ENCODER   'E'
#define    REGISTER_SWITCH    'S'
#define    REGISTER_RGB_LED   'R'
#define    REGISTER_EXTRA     'X'

#define    REGISTER_TYPE_NULL '\0'

extern const register_type_t register_types[];
const char *registerName(register_type_t registerType);

#endif // REGISTERTYPES_H
