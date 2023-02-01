#ifndef INPUTVALIDATIONEXCEPTION_H
#define INPUTVALIDATIONEXCEPTION_H

#include <QWidget>
#include <QException>

class InputValidationException : public QException
{
    QWidget *widget;
    QString reason;

public:
    InputValidationException(QWidget *widget, QString reason)
        : widget(widget), reason(reason) {};
    QString toString() const { return reason; };
    QWidget *getWidget() { return widget; };
};

#endif // INPUTVALIDATIONEXCEPTION_H
