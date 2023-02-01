cmake_minimum_required(VERSION 3.5)

project(forgecmake VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Pdf PdfWidgets)
# Seems to be a bug that I have to add this line. Yurks. And not very portable either
# The funny thing. With Core, Gui, Widgets and Pdf, the -isystem options are added
# automatically.
add_compile_options(-isystem /Users/mk/Qt/6.3.1/macos/lib/QtPdfWidgets.framework/Headers)

set(PROJECT_SOURCES
animatedindicator.cpp
animatedindicator.h
atom.cpp
atom.h
atomcable.cpp
atomcable.h
atominvalid.cpp
atominvalid.h
atomnumber.cpp
atomnumber.h
atomoneliner.cpp
atomoneliner.h
atomregister.cpp
atomregister.h
atomselector.cpp
atomselector.h
atomselectordialog.cpp
atomselectordialog.h
atomsubselector.cpp
atomsubselector.h
cablecolorizer.cpp
cablecolorizer.h
cableselector.cpp
cableselector.h
cablestatusindicator.cpp
cablestatusindicator.h
circuit.cpp
circuit.h
circuitchoosedialog.cpp
circuitchoosedialog.h
circuitcollection.cpp
circuitcollection.h
circuitinfoview.cpp
circuitinfoview.h
circuitview.cpp
circuitview.h
clipboard.cpp
clipboard.h
clipboardindicator.cpp
clipboardindicator.h
colors.h
colors_dark.h
colors_light.h
colorscheme.cpp
colorscheme.h
commentdialog.cpp
commentdialog.h
controllerchoosedialog.cpp
controllerchoosedialog.h
controllerlabellingdialog.cpp
controllerlabellingdialog.h
controllerselector.cpp
controllerselector.h
controlselector.cpp
controlselector.h
cursorposition.cpp
cursorposition.h
dialog.cpp
dialog.h
dragcontrollerindicator.cpp
dragcontrollerindicator.h
dragregisterindicator.cpp
dragregisterindicator.h
dragsectionindicator.cpp
dragsectionindicator.h
droidfirmware.cpp
droidfirmware.h
editoractions.cpp
editoractions.h
ellipsislabel.cpp
ellipsislabel.h
framecursor.cpp
framecursor.h
generalparseexception.cpp
globals.h
hintdialog.cpp
hintdialog.h
iconbase.cpp
iconbase.h
iconmarker.cpp
iconmarker.h
inputoutputselector.cpp
inputoutputselector.h
inputvalidationexception.cpp
inputvalidationexception.h
jackassignment.cpp
jackassignment.h
jackassignmentinput.cpp
jackassignmentinput.h
jackassignmentoutput.cpp
jackassignmentoutput.h
jackassignmentunknown.cpp
jackassignmentunknown.h
jackchoosedialog.cpp
jackchoosedialog.h
jackcircuitview.cpp
jackcircuitview.h
jackline.cpp
jackline.h
jackselector.cpp
jackselector.h
jackvaluetabledialog.cpp
jackvaluetabledialog.h
jackview.cpp
jackview.h
keycapturelineedit.cpp
keycapturelineedit.h
macmidihost.cpp
macmidihost.h
main.cpp
mainwindow.cpp
mainwindow.h
memoryindicator.cpp
memoryindicator.h
midihost.cpp
midihost.h
module.cpp
module.h
moduleb32.cpp
moduleb32.h
moduleblind.cpp
moduleblind.h
modulebling.cpp
modulebling.h
modulebuilder.cpp
modulebuilder.h
moduleg8.cpp
moduleg8.h
modulem4.cpp
modulem4.h
modulemaster.cpp
modulemaster.h
modulep10.cpp
modulep10.h
modulep2b8.cpp
modulep2b8.h
modulep4b2.cpp
modulep4b2.h
modules10.cpp
modules10.h
moduleview.cpp
moduleview.h
modulex7.cpp
modulex7.h
mousedragger.cpp
mousedragger.h
namechoosedialog.cpp
namechoosedialog.h
numberselector.cpp
numberselector.h
os.h
pageselector.cpp
pageselector.h
parseexception.h
patch.cpp
patch.h
patcheditengine.cpp
patcheditengine.h
patchintegrationdialog.cpp
patchintegrationdialog.h
patchoperator.cpp
patchoperator.h
patchparser.cpp
patchparser.h
patchproblem.cpp
patchproblem.h
patchproblemindicator.cpp
patchproblemindicator.h
patchpropertiesdialog.cpp
patchpropertiesdialog.h
patchsection.cpp
patchsection.h
patchsectionmanager.cpp
patchsectionmanager.h
patchsectiontitleview.cpp
patchsectiontitleview.h
patchsectionview.cpp
patchsectionview.h
patchversion.cpp
patchversion.h
patchview.cpp
patchview.h
rackview.cpp
rackview.h
registerlabels.cpp
registerlabels.h
registerlabelwidget.cpp
registerlabelwidget.h
registerlist.cpp
registerlist.h
registermarker.cpp
registermarker.h
registerselector.cpp
registerselector.h
registertypes.cpp
registertypes.h
selection.cpp
selection.h
smoothellipseitem.cpp
smoothellipseitem.h
sourcecodeeditor.cpp
sourcecodeeditor.h
tuning.h
updatehub.cpp
updatehub.h
usermanual.cpp
usermanual.h
windowsmidihost.cpp
windowsmidihost.h
resources.qrc
)

qt_add_executable(forgecmake
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)

target_link_libraries(forgecmake PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(forgecmake PRIVATE Qt6::Pdf)
target_link_libraries(forgecmake PRIVATE Qt6::PdfWidgets)
target_link_libraries(forgecmake PRIVATE "-framework CoreMIDI")

set_target_properties(forgecmake PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(forgecmake)
endif()
