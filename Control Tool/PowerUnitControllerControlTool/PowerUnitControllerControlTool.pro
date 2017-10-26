#-------------------------------------------------
#
# Project created by QtCreator 2016-06-26T23:15:30
#
#-------------------------------------------------

CONFIG += c++11

QT       += core gui widgets

TARGET = LowBlowControlTool
TEMPLATE = app


SOURCES += main.cpp\
	mainwindow.cpp \
		Implementations/AdcTemperatureConvertor.cpp \
		NewADC2TempDialog.cpp \
		Implementations/SettingsStep.cpp \
		Implementations/SettingsGenerator.cpp \
		FossasSimpleGraph/Implementations/QSimpleGraph.cpp \
		Implementations/SettingsSaverLoader.cpp

HEADERS  += mainwindow.hpp \
		Interfaces/IAdcTemperatureConvertor.hpp \
		Implementations/AdcTemperatureConvertor.hpp \
		Auxiliary.hpp \
		NewADC2TempDialog.hpp \
		Interfaces/ISettingsStep.hpp \
		Implementations/SettingsStep.hpp \
		Interfaces/ISettingsGenerator.hpp \
		Implementations/SettingsGenerator.hpp \
		FossasSimpleGraph/Implementations/QSimpleGraph.hpp \
		FossasSimpleGraph/Interfaces/IQSimpleGraph.hpp \
		Interfaces/ISettingsSaverLoader.hpp \
		Implementations/SettingsSaverLoader.hpp

FORMS    += mainwindow.ui \
		NewADC2TempDialog.ui
