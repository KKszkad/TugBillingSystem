QT       += core gui network widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# QXlsx code for Application Qt project
QXLSX_PARENTPATH=./         # current QXlsx path is . (. means curret directory)
QXLSX_HEADERPATH=./header/  # current QXlsx header path is ./header/
QXLSX_SOURCEPATH=./source/  # current QXlsx source path is ./source/
include(./QXlsx.pri)

SOURCES += \
    costeditingprompt.cpp \
    format.cpp \
    httpclient.cpp \
    main.cpp \
    login.cpp \
    networksetting.cpp \
    ordercheckingprompt.cpp \
    orderconfirmingprompt.cpp \
    samainwindow.cpp \
    tcmainwindow.cpp

HEADERS += \
    InvoicePart.h \
    TugApplication.h \
    costeditingprompt.h \
    format.h \
    httpclient.h \
    login.h \
    networksetting.h \
    ordercheckingprompt.h \
    orderconfirmingprompt.h \
    samainwindow.h \
    tcmainwindow.h

FORMS += \
    costeditingprompt.ui \
    login.ui \
    networksetting.ui \
    ordercheckingprompt.ui \
    orderconfirmingprompt.ui \
    samainwindow.ui \
    tcmainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

RESOURCES += \
    sources.qrc
