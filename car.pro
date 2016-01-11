HEADERS += main.h
HEADERS += ball.h
HEADERS += car.h
HEADERS += vector.h
HEADERS += physicsengine.h

SOURCES += main.cpp
SOURCES += physicsengine.cpp
SOURCES += ball.cpp
SOURCES += vector.cpp
SOURCES += car.cpp

target.path = car
sources.files= $$SOURCES $$HEADERS ball.pro
sources.path = .
INSTALLS += target sources
