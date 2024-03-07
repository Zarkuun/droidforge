#ifndef MEMORYANALYSISWINDOW_H
#define MEMORYANALYSISWINDOW_H

#include "patch.h"

#include <sourcecodeeditor.h>

typedef enum {
    BY_TOTAL,
    BY_COUNT,
    BY_AVERAGE,
    BY_NAME,
} sortby_t;

class MemoryAnalysisWindow : public SourceCodeEditor
{
    const Patch *patch;

public:
    MemoryAnalysisWindow(const Patch *patch, QWidget *parent);
    void showEvent(QShowEvent *event) override;

private slots:
    void update(sortby_t sortby);
};

#endif // MEMORYANALYSISWINDOW_H
