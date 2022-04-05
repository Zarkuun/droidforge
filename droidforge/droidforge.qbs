import qbs.FileInfo

QtApplication {
    Depends { name: "Qt.widgets" }

    cpp.defines: [
        // You can make your code fail to compile if it uses deprecated APIs.
        // In order to do so, uncomment the following line.
        //"QT_DISABLE_DEPRECATED_BEFORE=0x060000" // disables all the APIs deprecated before Qt 6.0.0
    ]

    files: [
        "main.cpp",
        "mainwindow.cpp",
        "modulebuilder.cpp",
        "modulebuilder.h",
        "moduletype.h",
        "moduletypeb32.cpp",
        "moduletypeb32.h",
        "moduletypeg8.cpp",
        "moduletypeg8.h",
        "moduletypeinvalid.cpp",
        "moduletypeinvalid.h",
        "moduletypemaster.cpp",
        "moduletypemaster.h",
        "mainwindow.h",
        "mainwindow.ui",
        "moduleview.cpp",
        "moduleview.h",
        "rack.cpp",
        "rack.h",
        "rackview.cpp",
        "rackview.h",
        "resources.qrc",
    ]

    install: true
    installDir: qbs.targetOS.contains("qnx") ? FileInfo.joinPaths("/tmp", name, "bin") : base
}
