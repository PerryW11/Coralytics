QT += core gui sql charts
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += c++17

SOURCES += \
    CreateCreatureDialog.cpp \
    CreatureProfilePage.cpp \
    EventWidget.cpp \
    LogParametersDialog.cpp \
    MainWindow.cpp \
    TankProfilePage.cpp \
    TaskDialog.cpp \
    TaskItemWidget.cpp \
    TrendsDialog.cpp \
    ViewAllParametersDialog.cpp \
    main.cpp

HEADERS += \
    CreateCreatureDialog.h \
    CreatureProfilePage.h \
    EventWidget.h \
    LogParametersDialog.h \
    MainWindow.h \
    TankProfilePage.h \
    TaskDialog.h \
    TaskItemWidget.h \
    TrendsDialog.h \
    ViewAllParametersDialog.h

FORMS += \
    MainWindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
