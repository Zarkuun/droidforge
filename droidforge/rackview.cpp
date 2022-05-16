#include "rackview.h"
#include "mainwindow.h"
#include "modulebuilder.h"
#include "tuning.h"
#include "controllerchoosedialog.h"

#include <QGraphicsItem>
#include <QResizeEvent>

RackView::RackView()
    : QGraphicsView()
{
    setMinimumHeight(RACV_MIN_HEIGHT);
    setMaximumHeight(RACV_MAX_HEIGHT * 2);

    QGraphicsScene *thescene = new QGraphicsScene();
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setScene(thescene);
    QPixmap background(":images/rackbackground.png");
    QBrush brush(background.scaledToHeight(RACV_BACKGROUND_HEIGHT)); //kheight() * 50));
    scene()->setBackgroundBrush(brush);
}

void RackView::resizeEvent(QResizeEvent *)
{
    updateSize();
}

void RackView::setPatch(Patch *newPatch)
{
    patch = newPatch;
    updateGraphics();
}

void RackView::mousePressEvent(QMouseEvent *event)
{
    if (event->type() == QMouseEvent::MouseButtonPress) {
        addController();
    }
}

void RackView::updateGraphics()
{
    scene()->clear();
    if (!patch)
        return;

    // Add strut, so space above and below the modules is visible
    scene()->addLine(0, 0, 0, RACV_BACKGROUND_HEIGHT, QPen(QColor(0, 0, 0, 0)));

    x = 10;
    addModule("master");
    if (patch->needG8())
        addModule("g8");
    if (patch->needX7())
        addModule("x7");
    addModule("blind");

    for (qsizetype i=0; i<patch->numControllers(); i++)
        addModule(patch->controller(i));
    updateSize();
}

void RackView::updateSize()
{
    fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
}

void RackView::addModule(const QString &name)
{
    Module *module = ModuleBuilder::buildModule(name);
    QPixmap *image = module->faceplateImage();
    QGraphicsItem *gi = scene()->addPixmap(*image);
    gi->setPos(x, RACV_TOP_MARGIN);
    x += module->hp() * RACV_PIXEL_PER_HP;
}

void RackView::addController()
{
    QString controller = ControllerChooseDialog::chooseController();
    if (!controller.isEmpty()) {
        QString actionTitle = tr("adding %1 controller").arg(controller.toUpper());
        the_forge->registerEdit(actionTitle);
        patch->addController(controller);
        updateGraphics();
        the_forge->patchHasChanged();
    }
}
