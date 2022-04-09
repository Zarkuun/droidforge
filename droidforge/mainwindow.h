#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "patchview.h"
#include "rackview.h"

#include <QMainWindow>

class Rack;
class Patch;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow *ui;
    Rack *rack;
    Patch *patch;
    RackView *rackview;
    PatchView *patchview;

public:
    MainWindow(Rack *, Patch *);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

private slots:
    void open();

private:
    void createActions();
    bool maybeSave();
    void loadFile(const QString &filename);
};
#endif // MAINWINDOW_H
