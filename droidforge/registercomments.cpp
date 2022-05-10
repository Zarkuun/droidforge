#include "registercomments.h"
#include "tuning.h"


RegisterComments::~RegisterComments()
{
    for (qsizetype i=0; i < comments.count(); i++)
        delete comments[i].atom;
}


RegisterComments *RegisterComments::clone() const
{
    RegisterComments *newrc = new RegisterComments();
    for (qsizetype i=0; i < comments.count(); i++) {
        RegisterComment c = comments[i];
        c.atom = comments[i].atom->clone();
        newrc->comments.append(c);
    }
    return newrc;
}


void RegisterComments::addComment(AtomRegister *atom, QString shorthand, QString comment)
{
    RegisterComment c;
    c.atom = atom;
    c.shorthand = shorthand;
    c.comment = comment;
    comments.append(c);
}


QString RegisterComments::toString() const
{
    QString s;

    typedef struct {
        char reg;
        const char *title;
    } regtitle_t;


    // First output global registers
    static regtitle_t globalRegtypes[] =  {
        { 'I', "INPUTS" },
        { 'N', "NORMALIZATIONS" },
        { 'O', "OUTPUTS" },
        { 'G', "GATES" },
        { 'R', "RGB LEDS" },
    };

    for (unsigned i=0; i<sizeof(globalRegtypes) / sizeof(regtitle_t); i++)
        s += toString(globalRegtypes[i].reg, 0, globalRegtypes[i].title);


    // And now the registers on the controllers
    static regtitle_t controllerRegtypes[] =  {
        { 'P', "POTS" },
        { 'B', "BUTTONS" },
        { 'L', "LEDS" },
        { 'S', "SWITCHES" },
        { 'R', "RGB LEDS" },
    };

    for (unsigned cn=0; cn<MAX_NUM_CONTROLLERS; cn++) {
        QString sc;
        for (unsigned j=0; j<sizeof(controllerRegtypes) / sizeof(regtitle_t); j++)
            sc += toString(controllerRegtypes[j].reg, cn+1);
        if (!sc.isEmpty()) {
            s += "# CONTROLLER " + QString::number(cn+1) + ":\n";
            s += sc;
            s += "\n";
        }
    }
    return s;
}


QString RegisterComments::toString(char reg, unsigned controller, const char *title) const
{
    QString s;
    bool first = true;
    for (qsizetype i=0; i < comments.count(); i++) {
        const RegisterComment &c = comments[i];
        if (c.atom->getRegisterType() == reg && c.atom->getController() == controller) {
            if (first) {
                first = false;
                if (title)
                    s += QString("# ") + title + ":\n";
            }
            s += "#   " + c.atom->toString() + ": ";
            if (!c.shorthand.isEmpty())
                s += "[" + c.shorthand + "] ";
            s += c.comment;
            s += "\n";
        }
    }
    if (title && !s.isEmpty())
        s += "\n";
    return s;
}
