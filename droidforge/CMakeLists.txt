cmake_minimum_required(VERSION 3.16)

project(DROIDForge VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core Widgets Pdf PdfWidgets)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Pdf PdfWidgets)

set(PROJECT_SOURCES
    main/main.cpp
    main/mainwindow.cpp
    main/mainwindow.h
    main/patchgenerator.cpp
    main/patchgenerator.h
    main/patchgeneratorbase.cpp
    main/patchgeneratorbase.h
    main/patchgeneratordialog.cpp
    main/patchgeneratordialog.h
    main/patchsizeindicator.cpp
    main/patchsizeindicator.h
    main/preferencesdialog.cpp
    main/preferencesdialog.h
    main/statusdumpindicator.cpp
    main/statusdumpindicator.h
    main/waitforsddialog.cpp
    main/waitforsddialog.h
    main/windowlist.cpp
    main/windowlist.h
    modules/modulebling.cpp
    modules/modulebling.h
    modules/moduledb8e.cpp
    modules/moduledb8e.h
    modules/modulee4.cpp
    modules/modulee4.h
    modules/modulemaster18.cpp
    modules/modulemaster18.h
    modules/modulep8s8.cpp
    modules/modulep8s8.h
    os/midihost.cpp
    os/midihost.h
    os/macmidihost.cpp
    os/macmidihost.h
    os/windowsmidihost.cpp
    os/windowsmidihost.h
    main/cablestatusindicator.cpp
    main/cablestatusindicator.h
    main/colorscheme.cpp
    main/colorscheme.h
    main/dragsectionindicator.cpp
    main/dragsectionindicator.h
    main/editoractions.cpp
    main/editoractions.h
    parser/statusdump.cpp
    parser/statusdump.h
    patch/atomtext.cpp
    patch/atomtext.h
    patch/jackdeduplicator.cpp
    patch/jackdeduplicator.h
    patchview/findpanel.cpp
    patchview/findpanel.h
    patchview/memoryanalysiswindow.cpp
    patchview/memoryanalysiswindow.h
    patchview/rewritecablesdialog.cpp
    patchview/rewritecablesdialog.h
    patchview/sectioncommentview.cpp
    patchview/sectioncommentview.h
    rackview/smoothellipseitem.cpp
    rackview/smoothellipseitem.h
    utilities/cableselectorlineedit.cpp
    utilities/cableselectorlineedit.h
    utilities/ellipsislabel.cpp
    utilities/ellipsislabel.h
    utilities/iconbase.cpp
    utilities/iconbase.h
    main/patchoperator.cpp
    main/patchoperator.h
    main/patchproblemindicator.cpp
    main/patchproblemindicator.h
    main/patchsectionmanager.cpp
    main/patchsectionmanager.h
    main/patchview.cpp
    main/patchview.h
    main/updatehub.cpp
    main/updatehub.h
    utilities/imagecache.cpp
    utilities/imagecache.h
    utilities/keycapturelineedit.cpp
    utilities/keycapturelineedit.h
    main/pageselector.cpp
    main/pageselector.h
    main/usermanual.cpp
    main/usermanual.h
    main/globals.h
    patch/patcheditengine.cpp
    patch/patcheditengine.h
    patch/patchproblem.cpp
    patch/patchproblem.h
    patch/patchversion.cpp
    patch/patchversion.h
    main/registerlabels.cpp
    main/registerlabels.h
    main/sourcecodeeditor.cpp
    main/sourcecodeeditor.h
    patchview/atomeditor/jackvaluetabledialog.cpp
    patchview/atomeditor/jackvaluetabledialog.h
    patchview/cablecolorizer.cpp
    patchview/cablecolorizer.h
    main/clipboard.cpp
    main/clipboard.h
    main/clipboardindicator.cpp
    main/clipboardindicator.h
    patchview/framecursor.cpp
    patchview/framecursor.h
    patchview/iconmarker.cpp
    patchview/iconmarker.h
    patchview/selection.cpp
    patchview/selection.h
    rackview/animatedindicator.cpp
    rackview/animatedindicator.h
    rackview/controllerlabellingdialog.cpp
    rackview/controllerlabellingdialog.h
    rackview/dragcontrollerindicator.cpp
    rackview/dragcontrollerindicator.h
    rackview/dragregisterindicator.cpp
    rackview/dragregisterindicator.h
    main/mousedragger.cpp
    main/mousedragger.h
    patchview/patchsectiontitleview.cpp
    patchview/patchsectiontitleview.h
    rackview/registerlabelwidget.cpp
    rackview/registerlabelwidget.h
    main/droidfirmware.cpp
    main/droidfirmware.h
    main/main.cpp
    main/mainwindow.cpp
    main/mainwindow.h
    os/os.h
    main/patchpropertiesdialog.cpp
    main/patchpropertiesdialog.h
    main/tuning.h
    modules/module.cpp
    modules/module.h
    modules/moduleb32.cpp
    modules/moduleb32.h
    modules/moduleblind.cpp
    modules/moduleblind.h
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
    parser/generalparseexception.cpp
    parser/parseexception.h
    parser/patchparser.cpp
    parser/patchparser.h
    patch/atom.cpp
    patch/atom.h
    patch/atomcable.cpp
    patch/atomcable.h
    patch/atominvalid.cpp
    patch/atominvalid.h
    patch/atomnumber.cpp
    patch/atomnumber.h
    patch/atomregister.cpp
    patch/atomregister.h
    patch/circuit.cpp
    patch/circuit.h
    patch/jackassignment.cpp
    patch/jackassignment.h
    patch/jackassignmentinput.cpp
    patch/jackassignmentinput.h
    patch/jackassignmentoutput.cpp
    patch/jackassignmentoutput.h
    patch/jackassignmentunknown.cpp
    patch/jackassignmentunknown.h
    patch/patch.cpp
    patch/patch.h
    patch/patchsection.cpp
    patch/patchsection.h
    patch/registerlist.cpp
    patch/registerlist.h
    patch/registertypes.cpp
    patch/registertypes.h
    patchview/circuitview.cpp
    patchview/circuitview.h
    patchview/commentdialog.cpp
    patchview/commentdialog.h
    patchview/cursorposition.cpp
    patchview/cursorposition.h
    patchview/patchintegrationdialog.cpp
    patchview/patchintegrationdialog.h
    patchview/patchsectionview.cpp
    patchview/patchsectionview.h
    patchview/registerselector.cpp
    patchview/registerselector.h
    patchview/circuitchooser/circuitchoosedialog.cpp
    patchview/circuitchooser/circuitchoosedialog.h
    patchview/circuitchooser/circuitcollection.cpp
    patchview/circuitchooser/circuitcollection.h
    patchview/circuitchooser/circuitinfoview.cpp
    patchview/circuitchooser/circuitinfoview.h
    patchview/jackselector/jackchoosedialog.cpp
    patchview/jackselector/jackchoosedialog.h
    patchview/jackselector/jackline.cpp
    patchview/jackselector/jackline.h
    patchview/jackselector/jackselector.cpp
    patchview/jackselector/jackselector.h
    patchview/jackselector/jackcircuitview.cpp
    patchview/jackselector/jackcircuitview.h
    patchview/jackselector/jackview.cpp
    patchview/jackselector/jackview.h
    patchview/atomeditor/atomoneliner.cpp
    patchview/atomeditor/atomoneliner.h
    patchview/atomeditor/atomselector.cpp
    patchview/atomeditor/atomselector.h
    patchview/atomeditor/atomselectordialog.cpp
    patchview/atomeditor/atomselectordialog.h
    patchview/atomeditor/atomsubselector.cpp
    patchview/atomeditor/atomsubselector.h
    patchview/atomeditor/cableselector.cpp
    patchview/atomeditor/cableselector.h
    patchview/atomeditor/controlselector.cpp
    patchview/atomeditor/controlselector.h
    patchview/atomeditor/inputoutputselector.cpp
    patchview/atomeditor/inputoutputselector.h
    patchview/atomeditor/numberselector.cpp
    patchview/atomeditor/numberselector.h
    rackview/controllerchoosedialog.cpp
    rackview/controllerchoosedialog.h
    rackview/controllerselector.cpp
    rackview/controllerselector.h
    rackview/rackview.cpp
    rackview/rackview.h
    rackview/registermarker.cpp
    rackview/registermarker.h
    utilities/dialog.cpp
    utilities/dialog.h
    utilities/hintdialog.cpp
    utilities/hintdialog.h
    utilities/inputvalidationexception.cpp
    utilities/inputvalidationexception.h
    utilities/namechoosedialog.cpp
    utilities/namechoosedialog.h
    utilities/searchlineedit.cpp
    utilities/searchlineedit.h
    utilities/tooltip.cpp
    utilities/tooltip.h
    utilities/utilities.cpp
    utilities/utilities.h
    resources.qrc
)

qt_add_executable(${PROJECT_NAME}
MANUAL_FINALIZATION
${PROJECT_SOURCES}
)


target_include_directories(${PROJECT_NAME} PRIVATE
    .
    os
    patch
    modules
    patchview
    patchview/jackselector
    patchview/atomeditor
    patchview/circuitchooser
    rackview
    utilities
    main
    parser
)

if(APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE
            Qt${QT_VERSION_MAJOR}::Widgets
            Qt${QT_VERSION_MAJOR}::Pdf
            Qt${QT_VERSION_MAJOR}::PdfWidgets
            "-framework CoreFoundation"
            "-framework CoreMIDI")
elseif(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE
            Qt${QT_VERSION_MAJOR}::Widgets
            Qt${QT_VERSION_MAJOR}::Pdf
            Qt${QT_VERSION_MAJOR}::PdfWidgets
            winmm)
elseif(LINUX)
    target_link_libraries(${PROJECT_NAME} PRIVATE
            Qt${QT_VERSION_MAJOR}::Widgets
            Qt${QT_VERSION_MAJOR}::Pdf
            Qt${QT_VERSION_MAJOR}::PdfWidgets
            )
endif()


set_target_properties(${PROJECT_NAME} PROPERTIES
    ${BUNDLE_ID_OPTION}
    OUTPUT_NAME "DROID Forge"
    MACOSX_BUNDLE_BUNDLE_NAME "DROID Forge"
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif()
