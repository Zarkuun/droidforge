import qbs.FileInfo

QtApplication {
    Depends { name: "Qt.widgets" }

    cpp.defines: [
        // You can make your code fail to compile if it uses deprecated APIs.
        // In order to do so, uncomment the following line.
        //"QT_DISABLE_DEPRECATED_BEFORE=0x060000" // disables all the APIs deprecated before Qt 6.0.0
    ]

    files: [
        "atom.cpp",
        "atom.h",
        "atomcable.cpp",
        "atomcable.h",
        "atomnumber.cpp",
        "atomnumber.h",
        "atomregister.cpp",
        "atomregister.h",
        "circuit.cpp",
        "circuit.h",
        "circuittype.cpp",
        "circuittype.h",
        "circuitview.cpp",
        "circuitview.h",
        "droidfirmware.cpp",
        "droidfirmware.h",
        "droidforge.cpp",
        "droidforge.h",
        "jackassignment.cpp",
        "jackassignment.h",
        "main.cpp",
        "mainwindow.cpp",
        "module.h",
        "moduleb32.cpp",
        "moduleb32.h",
        "modulebuilder.cpp",
        "modulebuilder.h",
        "moduleg8.cpp",
        "moduleg8.h",
        "moduleinvalid.cpp",
        "moduleinvalid.h",
        "modulem4.cpp",
        "modulem4.h",
        "modulemaster.cpp",
        "mainwindow.h",
        "mainwindow.ui",
        "modulemaster.h",
        "modulep10.cpp",
        "modulep10.h",
        "modulep2b8.cpp",
        "modulep2b8.h",
        "modulep4b2.cpp",
        "modulep4b2.h",
        "modules10.cpp",
        "modules10.h",
        "moduleview.cpp",
        "moduleview.h",
        "patch.cpp",
        "patch.h",
        "patchparser.cpp",
        "patchparser.h",
        "patchsection.cpp",
        "patchsection.h",
        "patchsectionview.cpp",
        "patchsectionview.h",
        "patchview.cpp",
        "patchview.h",
        "rack.cpp",
        "rack.h",
        "rackview.cpp",
        "rackview.h",
        "resources.qrc",
        "tuning.h",
    ]

    install: true
    installDir: qbs.targetOS.contains("qnx") ? FileInfo.joinPaths("/tmp", name, "bin") : base
}