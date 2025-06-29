QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    addingredientdialog.cpp \
    addrecipedialog.cpp \
    allrecipesdialog.cpp \
    chefmainwindow.cpp \
    chefsignupdialog.cpp \
    clientmainwindow.cpp \
    clientpreferencesdialog.cpp \
    clientsignupdialog.cpp \
    forgotpassworddialog.cpp \
    logindialog.cpp \
    main.cpp \
    mainwindow.cpp \
    myordersdialog.cpp \
    myrecipesdialog.cpp \
    recipesearchdialog.cpp \
    resetpassworddialog.cpp \
    shoppinglistdialog.cpp \
    signupdialog.cpp

HEADERS += \
    addingredientdialog.h \
    addrecipedialog.h \
    allrecipesdialog.h \
    chefmainwindow.h \
    chefsignupdialog.h \
    clientmainwindow.h \
    clientpreferencesdialog.h \
    clientsignupdialog.h \
    forgotpassworddialog.h \
    logindialog.h \
    mainwindow.h \
    myordersdialog.h \
    myrecipesdialog.h \
    recipesearchdialog.h \
    resetpassworddialog.h \
    shoppinglistdialog.h \
    signupdialog.h

FORMS += \
    addingredientdialog.ui \
    addrecipedialog.ui \
    allrecipesdialog.ui \
    chefmainwindow.ui \
    chefsignupdialog.ui \
    clientmainwindow.ui \
    clientpreferencesdialog.ui \
    clientsignupdialog.ui \
    forgotpassworddialog.ui \
    logindialog.ui \
    mainwindow.ui \
    myordersdialog.ui \
    myrecipesdialog.ui \
    recipesearchdialog.ui \
    resetpassworddialog.ui \
    shoppinglistdialog.ui \
    signupdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
QT += network

RESOURCES += \
    Resources.qrc
