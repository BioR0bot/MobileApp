QT       += core gui \
            androidextras \
            sensors


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 \
          mobility \
          object_parallel_to_source

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += "$$PWD/ThirdParty/build/libdatachannel/include" \
               "$$PWD/ThirdParty/src/libdatachannel/deps/json/single_include" \

LIBS += -L"$$PWD/ThirdParty/build/libdatachannel/lib" \
        -ldatachannel \

SOURCES += \
    AppDataManager.cpp \
    ConnectIndicator.cpp \
    DatachannelThread.cpp \
    Utils.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    AppDataManager.h \
    ConnectIndicator.h \
    DatachannelThread.h \
    Utils.h \
    mainwindow.h

FORMS += \
    ConnectIndicator.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    android/AndroidManifest.xml \
    android/build.gradle \
    android/gradle.properties \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew \
    android/gradlew.bat \
    android/res/values/libs.xml

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
