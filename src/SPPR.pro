QT       += core gui
QT += charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
CONFIG  += qaxcontainer


include(xlsx/qtxlsx.pri)

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    aboutprogramform.cpp \
    asynccalculation.cpp \
    calculateasyncwiththreadpool.cpp \
    chartsform.cpp \
    dataprocessing.cpp \
    fileloadingdialog.cpp \
    info.cpp \
    io.cpp \
    loadingform.cpp \
    main.cpp \
    mainwindow.cpp \
    multi_threading_methods.cpp \
    openprojectslistform.cpp \
    preselectionchartsform.cpp \
    selectforexportform.cpp \
    setprojectgroupimportanceform.cpp \
    solution.cpp \
    solutiondb.cpp \
    startupconfigform.cpp

HEADERS += \
    aboutprogramform.h \
    asynccalculation.h \
    calculateasyncwiththreadpool.h \
    chartsform.h \
    dataprocessing.h \
    fileloadingdialog.h \
    info.h \
    io.h \
    loadingform.h \
    mainwindow.h \
    multi_threading_methods.h \
    openprojectslistform.h \
    preselectionchartsform.h \
    selectforexportform.h \
    setprojectgroupimportanceform.h \
    solution.h \
    solutiondb.h \
    startupconfigform.h

FORMS += \
    aboutprogramform.ui \
    chartsform.ui \
    info.ui \
    loadingform.ui \
    mainwindow.ui \
    openprojectslistform.ui \
    preselectionchartsform.ui \
    selectforexportform.ui \
    setprojectgroupimportanceform.ui \
    startupconfigform.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

RESOURCES += \
    icons.qrc
