#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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

public:
    MainWindow(Rack *, Patch *);
    ~MainWindow();

private slots:
    void open();

private:
    void createActions();
    bool maybeSave();
    void loadFile(const QString &filename);
};
#endif // MAINWINDOW_H
