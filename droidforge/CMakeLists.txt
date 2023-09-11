cmake_minimum_required(VERSION 3.5)

project("DroidForge" VERSION 1.1 LANGUAGES CXX)

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
# add_compile_options(-isystem /Users/mk/Qt/6.3.1/macos/lib/QtPdfWidgets.framework/Headers)

include_directories(
    .
    ./main
    ./rackview
    ./modules
    ./utilities
    ./parser
    ./patchview
    ./patchview/jackselector
    ./patchview/circuitchooser
    ./patchview/atomeditor
    ./os
    ./patch
)

set(PROJECT_SOURCES
    rackview/animatedindicator.cpp
    rackview/animatedindicator.h
    patch/atom.cpp
    patch/atom.h
    patch/atomcable.cpp
    patch/atomcable.h
    patch/atominvalid.cpp
    patch/atominvalid.h
    patch/atomnumber.cpp
    patch/atomnumber.h
    patchview/atomeditor/atomoneliner.cpp
    patchview/atomeditor/atomoneliner.h
    patch/atomregister.cpp
    patch/atomregister.h
    patchview/atomeditor/atomselector.cpp
    patchview/atomeditor/atomselector.h
    patchview/atomeditor/atomselectordialog.cpp
    patchview/atomeditor/atomselectordialog.h
    patchview/atomeditor/atomsubselector.cpp
    patchview/atomeditor/atomsubselector.h
    patchview/cablecolorizer.cpp
    patchview/cablecolorizer.h
    patchview/atomeditor/cableselector.cpp
    patchview/atomeditor/cableselector.h
    main/cablestatusindicator.cpp
    main/cablestatusindicator.h
    patch/circuit.cpp
    patch/circuit.h
    patchview/circuitchooser/
    patchview/circuitchooser/circuitchoosedialog.cpp
    patchview/circuitchooser/circuitchoosedialog.h
    patchview/circuitchooser/circuitcollection.cpp
    patchview/circuitchooser/circuitcollection.h
    patchview/circuitchooser/circuitinfoview.cpp
    patchview/circuitchooser/circuitinfoview.h
    patchview/circuitchooser/circuitchoosedialog.cpp
    patchview/circuitchooser/circuitchoosedialog.h
    patchview/circuitchooser/circuitcollection.cpp
    patchview/circuitchooser/circuitcollection.h
    patchview/circuitchooser/circuitinfoview.cpp
    patchview/circuitchooser/circuitinfoview.h
    patchview/circuitview.cpp
    patchview/jackselector/jackcircuitview.cpp
    patchview/circuitview.h
    patchview/jackselector/jackcircuitview.h
    main/clipboard.cpp
    main/clipboard.h
    main/clipboardindicator.cpp
    main/clipboardindicator.h
    main/colorscheme.cpp
    main/colorscheme.h
    colors_dark.h
    colors_light.h
    main/colorscheme.cpp
    main/colorscheme.h
    patchview/commentdialog.cpp
    patchview/commentdialog.h
    rackview/controllerchoosedialog.cpp
    rackview/controllerchoosedialog.h
    rackview/controllerlabellingdialog.cpp
    rackview/controllerlabellingdialog.h
    rackview/controllerselector.cpp
    rackview/controllerselector.h
    patchview/atomeditor/controlselector.cpp
    patchview/atomeditor/controlselector.h
    patchview/cursorposition.cpp
    patchview/cursorposition.h
    main/patchpropertiesdialog.cpp
    main/preferencesdialog.cpp
    patchview/atomeditor/atomselectordialog.cpp
    patchview/atomeditor/jackvaluetabledialog.cpp
    patchview/circuitchooser/circuitchoosedialog.cpp
    patchview/commentdialog.cpp
    patchview/jackselector/jackchoosedialog.cpp
    patchview/patchintegrationdialog.cpp
    patchview/rewritecablesdialog.cpp
    rackview/controllerchoosedialog.cpp
    rackview/controllerlabellingdialog.cpp
    utilities/dialog.cpp
    utilities/hintdialog.cpp
    utilities/namechoosedialog.cpp
    main/patchpropertiesdialog.h
    main/preferencesdialog.h
    patchview/atomeditor/atomselectordialog.h
    patchview/atomeditor/jackvaluetabledialog.h
    patchview/circuitchooser/circuitchoosedialog.h
    patchview/commentdialog.h
    patchview/jackselector/jackchoosedialog.h
    patchview/patchintegrationdialog.h
    patchview/rewritecablesdialog.h
    rackview/controllerchoosedialog.h
    rackview/controllerlabellingdialog.h
    utilities/dialog.h
    utilities/hintdialog.h
    utilities/namechoosedialog.h
    rackview/dragcontrollerindicator.cpp
    rackview/dragcontrollerindicator.h
    rackview/dragregisterindicator.cpp
    rackview/dragregisterindicator.h
    main/dragsectionindicator.cpp
    main/dragsectionindicator.h
    main/droidfirmware.cpp
    main/droidfirmware.h
    main/editoractions.cpp
    main/editoractions.h
    utilities/ellipsislabel.cpp
    utilities/ellipsislabel.h
    patchview/framecursor.cpp
    patchview/framecursor.h
    parser/generalparseexception.cpp
    main/globals.h
    utilities/hintdialog.cpp
    utilities/hintdialog.h
    utilities/iconbase.cpp
    utilities/iconbase.h
    patchview/iconmarker.cpp
    patchview/iconmarker.h
    patchview/atomeditor/inputoutputselector.cpp
    patchview/atomeditor/inputoutputselector.h
    utilities/inputvalidationexception.cpp
    utilities/inputvalidationexception.h
    patch/jackassignment.cpp
    patch/jackassignment.h
    patch/jackassignmentinput.cpp
    patch/jackassignmentinput.h
    patch/jackassignmentoutput.cpp
    patch/jackassignmentoutput.h
    patch/jackassignmentunknown.cpp
    patch/jackassignmentunknown.h
    patchview/jackselector/jackchoosedialog.cpp
    patchview/jackselector/jackchoosedialog.h
    patchview/jackselector/jackcircuitview.cpp
    patchview/jackselector/jackcircuitview.h
    patchview/jackselector/jackline.cpp
    patchview/jackselector/jackline.h
    patchview/jackselector/jackselector.cpp
    patchview/jackselector/jackselector.h
    patchview/atomeditor/jackvaluetabledialog.cpp
    patchview/atomeditor/jackvaluetabledialog.h
    patchview/jackselector/jackview.cpp
    patchview/jackselector/jackview.h
    utilities/keycapturelineedit.cpp
    utilities/keycapturelineedit.h
    os/macmidihost.cpp
    os/macmidihost.h
    main/main.cpp
    main/mainwindow.cpp
    main/mainwindow.h
    os/macmidihost.cpp
    os/midihost.cpp
    os/windowsmidihost.cpp
    os/macmidihost.h
    os/midihost.h
    os/windowsmidihost.h
    modules/module.cpp
    modules/module.h
    modules/moduleb32.cpp
    modules/moduleb32.h
    modules/moduleblind.cpp
    modules/moduleblind.h
    modules/modulebling.cpp
    modules/modulebling.h
    modules/modulebuilder.cpp
    modules/modulebuilder.h
    modules/moduleg8.cpp
    modules/moduleg8.h
    modules/modulem4.cpp
    modules/modulem4.h
    modules/modulemaster.cpp
    modules/modulemaster.h
    modules/modulep10.cpp
    modules/modulep10.h
    modules/modulep2b8.cpp
    modules/modulep2b8.h
    modules/modulep4b2.cpp
    modules/modulep4b2.h
    modules/modules10.cpp
    modules/modules10.h
    modules/moduleview.cpp
    modules/moduleview.h
    modules/modulex7.cpp
    modules/modulex7.h
    main/mousedragger.cpp
    main/mousedragger.h
    utilities/namechoosedialog.cpp
    utilities/namechoosedialog.h
    patchview/atomeditor/numberselector.cpp
    patchview/atomeditor/numberselector.h
    os/os.h
    main/pageselector.cpp
    main/pageselector.h
    parser/parseexception.h
    patch/patch.cpp
    patch/patch.h
    patch/patcheditengine.cpp
    patch/patcheditengine.h
    patchview/patchintegrationdialog.cpp
    patchview/patchintegrationdialog.h
    main/patchoperator.cpp
    main/patchoperator.h
    parser/patchparser.cpp
    parser/patchparser.h
    patch/patchproblem.cpp
    patch/patchproblem.h
    main/patchproblemindicator.cpp
    main/patchproblemindicator.h
    main/patchpropertiesdialog.cpp
    main/patchpropertiesdialog.h
    patch/patchsection.cpp
    patch/patchsection.h
    main/patchsectionmanager.cpp
    main/patchsectionmanager.h
    patchview/patchsectiontitleview.cpp
    patchview/patchsectiontitleview.h
    patchview/patchsectionview.cpp
    patchview/patchsectionview.h
    patch/patchversion.cpp
    patch/patchversion.h
    main/patchview.cpp
    main/patchview.h
    rackview/rackview.cpp
    rackview/rackview.h
    main/registerlabels.cpp
    main/registerlabels.h
    rackview/registerlabelwidget.cpp
    rackview/registerlabelwidget.h
    patch/registerlist.cpp
    patch/registerlist.h
    rackview/registermarker.cpp
    rackview/registermarker.h
    patchview/registerselector.cpp
    patchview/registerselector.h
    patch/registertypes.cpp
    patch/registertypes.h
    patchview/selection.cpp
    patchview/selection.h
    rackview/smoothellipseitem.cpp
    rackview/smoothellipseitem.h
    main/sourcecodeeditor.cpp
    main/sourcecodeeditor.h
    main/tuning.h
    main/updatehub.cpp
    main/updatehub.h
    main/usermanual.cpp
    main/usermanual.h
    os/windowsmidihost.cpp
    os/windowsmidihost.h
    resources.qrc
)

qt_add_executable(DroidForge
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)

target_link_libraries(DroidForge PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(DroidForge PRIVATE Qt6::Pdf)
target_link_libraries(DroidForge PRIVATE Qt6::PdfWidgets)
# target_link_libraries(DroidForge PRIVATE "-framework CoreMIDI")

set_target_properties(DroidForge PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER droidforge.dmmdm.de
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(DroidForge)
endif()
