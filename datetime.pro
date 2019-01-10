QT              += widgets svg
TEMPLATE         = lib
CONFIG          += plugin c++11

TARGET           = datetime1
DISTFILES       += datetime.json

HEADERS += \
    datetimeplugin.h \
    datetimewidget.h \
    calendarwidget.h

SOURCES += \
    datetimeplugin.cpp \
    datetimewidget.cpp \
    calendarwidget.cpp

RESOURCES += resources.qrc