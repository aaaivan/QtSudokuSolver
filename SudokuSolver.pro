QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    IO/SaveLoadManager.cpp \
    adddigitscontrols.cpp \
    bruteforcesolverthread.cpp \
    cellcontentbutton.cpp \
    contextmenuwindow.cpp \
    drawkillerscontrols.cpp \
    drawregionscontrols.cpp \
    editgridcontrols.cpp \
    gridgraphicaloverlay.cpp \
    killercagewidget.cpp \
    main.cpp \
    mainwindow.cpp \
    mainwindowcontent.cpp \
    puzzledata.cpp \
    savepuzzlethread.cpp \
    solver/BruteForceSolver.cpp \
    solver/FishTechniques.cpp \
    solver/FishTechniquesV2.cpp \
    solver/GhostCagesManager.cpp \
    solver/GridProgressManager.cpp \
    solver/InniesAndOuties.cpp \
    solver/LockedCandidates.cpp \
    solver/NakedSubsets.cpp \
    solver/Progress.cpp \
    solver/RandomGuessTreeNode.cpp \
    solver/Region.cpp \
    solver/RegionUpdatesManager.cpp \
    solver/RegionsManager.cpp \
    solver/Scanning.cpp \
    solver/SolvingTechnique.cpp \
    solver/SudokuCell.cpp \
    solver/SudokuGrid.cpp \
    solver/SudokuParser.cpp \
    solver/VariantConstraints.cpp \
    solvercontextmenu.cpp \
    solvercontrols.cpp \
    startmenu.cpp \
    sudokucellwidget.cpp \
    sudokugridwidget.cpp \
    sudokusolverthread.cpp \
    thirdparty/dancing_links.cpp \
    thirdparty/linked_matrix.cpp \
    variantcluewidget.cpp

HEADERS += \
    IO/SaveLoadManager.h \
    adddigitscontrols.h \
    bruteforcesolverthread.h \
    cellcontentbutton.h \
    contextmenuwindow.h \
    drawkillerscontrols.h \
    drawregionscontrols.h \
    editgridcontrols.h \
    gridgraphicaloverlay.h \
    killercagewidget.h \
    mainwindow.h \
    mainwindowcontent.h \
    puzzledata.h \
    savepuzzlethread.h \
    solver/BruteForceSolver.h \
    solver/GhostCagesManager.h \
    solver/GridProgressManager.h \
    solver/Progress.h \
    solver/ProgressTypes.h \
    solver/RandomGuessTreeNode.h \
    solver/Region.h \
    solver/RegionUpdatesManager.h \
    solver/RegionsManager.h \
    solver/Scanning.h \
    solver/SolvingTechnique.h \
    solver/SudokuCell.h \
    solver/SudokuGrid.h \
    solver/SudokuParser.h \
    solver/Types.h \
    solver/VariantConstraints.h \
    solvercontextmenu.h \
    solvercontrols.h \
    startmenu.h \
    sudokucellwidget.h \
    sudokugridwidget.h \
    sudokusolverthread.h \
    thirdparty/dancing_links.h \
    thirdparty/linked_matrix.h \
    variantcluewidget.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
