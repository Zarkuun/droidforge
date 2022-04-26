#include "registercomments.h"
#include "QtCore/qdebug.h"


RegisterComments::~RegisterComments()
{
    for (qsizetype i=0; i<<comments.count(); i++)
        delete comments[i].atom;
}


void RegisterComments::addComment(AtomRegister *atom, QString shorthand, QString comment)
{
    RegisterComment c;
    c.atom = atom;
    c.shorthand = shorthand;
    c.comment = comment;
    comments.append(c);
    qDebug() << "Comment for " << comment;
}
