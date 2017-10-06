﻿/*
Copyright 2017 White Fossa aka Artyom Vetrov.

This file is part of project "LowBlow" (advanced programmable cooler controller).

All parts of "LowBlow" is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

"LowBlow" project files is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with project "LowBlow" files. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

/**
	* Main window class
	*/

#include <Auxiliary.hpp>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpinBox>
#include <Interfaces/IAdcTemperatureConvertor.hpp>
#include <Implementations/AdcTemperatureConvertor.hpp>
#include <NewADC2TempDialog.hpp>
#include <Implementations/SettingsGenerator.hpp>
#include <Implementations/SettingsStep.hpp>
#include <FossasSimpleGraph/Implementations/QSimpleGraph.hpp>


#define MW_GRAPH_STRETCH_FACTOR 10

namespace Ui {
	class MainWindow;

	/**
	 * @brief Columns of Steps Table (ui->mw_StepsTable)
	 */
	enum STEPS_TABLE_ROWS
	{
		ADC_LEVEL = 0, /**< Row with ADC level */
		TEMPERATURE = 1, /**< Row with temperature */
		RPM_LEVEL = 2, /**< Row with RPM level */
		RPM_PERCENT = 3, /**< Row with RPM percent */
		ADC_DELTA = 4,  /**< Row with ADC level increase */
		TEMPERATURE_DELTA = 5, /**< Row with temperature increase */
		RPM_DELTA = 6, /**< Row with RPM level increase */
		RPM_DELTA_PERCENT = 7  /**< Row with RPM level increase in percents */
	};

}


class MainWindow : public QMainWindow
{
	Q_OBJECT

public:

	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

/* Public slots */
public slots:
	/**
	 * @brief mn_slot_exit
	 * Called when user want to close application.
	 */
	void MwSlotExit();

	/**
	 * @brief Called when new this->tconv initialized with the new instance of ADC to Temperature convertor.
	 */
	void MwSlotConvertorChanged();

	/**
	 * @brief Called when user wants to create a new ADC->Temperature conversion settings
	 */
	void MwSlotCreateNewADC2Temp();

	/**
	 * @brief Called when user changes base temperature
	 * @param temp - new temperature in Celsius
	 */
	void MwSlotBaseTemperatureChanged(double tempC);

	/**
	 * @brief MwSlotBaseRPMChanged Called when user changes base RPM level
	 * @param RPM - new RPM value
	 */
	void MwSlotBaseRPMChanged(int RPM);

	/**
	 * @brief Called when ADC delta value of any spinbox is changed. It process change, determines what spinbox caused the change and then
	 * emits MwADCDeltaChanged(uint StepNumber, uint NewValue) with step and new value.
	 * Connect all ADC delta spinboxes in steps table to it.
	 * @param newDelta New delta value.
	 */
	void MwSlotADCDeltaChangedRaw(int newDelta);

	/**
	 * @brief As MwSlotADCDeltaChangedRaw(), but for RPM delta
	 * @param newDelta New RPM delta.
	 */
	void MwSlotRPMDeltaChangedRaw(int newDelta);

	/**
	 * @brief Called when ADC delta value of any spinbox is chaned.
	 * @param StepNumber Step number, for what change occured.
	 * @param NewDelta New ADC delta.
	 */
	void MwSlotADCDeltaChanged(uint StepNumber, uint NewDelta);

	/**
	 * @brief As MwSlotADCDeltaChanged(), but for RPM Delta change.
	 * @param StepNumber See MwSlotADCDeltaChanged().
	 * @param NewDelta New RPM delta.
	 */
	void MwSlotRPMDeltaChanged(uint StepNumber, uint NewDelta);

	/**
	 * @brief Updates UI steps table according to steps from this->_setgen
	 */
	void MwSlotUpdateStepsTable();

	/**
	 * @brief Sets filename to window title and status bar.
	 * @param filename Filename to set.
	 */
	void MwSlotSetFileName(QString filename);

/* Signals */
signals:
	/**
	 * @brief Emit it when this->tconv initialized with the new instance of ADC to Temperature convertor.
	 */
	void MwSignalConvertorChanged();

	/**
	 * @brief MwADCDeltaChanged Emitted when ADC delta changed for cell in steps table.
	 * @param StepNumber - Step number.
	 * @param NewValue - New ADC delta value.
	 */
	void MwSignalADCDeltaChanged(uint StepNumber, uint NewDelta);

	/**
	 * @brief As MwADCDeltaChanged(), but for RPM delta.
	 * @param StepNumber See MwADCDeltaChanged().
	 * @param NewDelta See MwADCDeltaChanged().
	 */
	void MwSignalRPMDeltaChanged(uint StepNumber, uint NewDelta);

	/**
	 * @brief Emit it when steps table (UI) needs to be re-read from this->_setgen
	 */
	void MwSignalUpdateStepsTable();

protected:
	void changeEvent(QEvent *e);

private:

	/**
	 * @brief Minimal graph width
	 */
	const uint _minGraphWidth = 300;

	/**
	 * @brief Minimal graph height
	 */
	const uint _minGraphHeight = 300;

	/**
	 * @brief Graph's stretch factor
	 */
	const double _graphStretchFactor = 10;

	/**
	 * @brief Graph's horizontal size policy
	 */
	const QSizePolicy::Policy _graphHSizePolicy = QSizePolicy::Policy::MinimumExpanding;

	/**
	 * @brief Graph's vertical size policy
	 */
	const QSizePolicy::Policy _graphVSizePolicy = QSizePolicy::Policy::MinimumExpanding;

	Ui::MainWindow *ui;

	/**
	 * @brief Label with information about ADC->Temperature conversion
	 */
	QLabel *_mwConversionStatus = nullptr;

	/**
	 * @brief Label with current filename.
	 */
	QLabel *_mwFileName = nullptr;

	/**
	 * @brief Temperature convertor
	 */
	Interfaces::IAdcTemperatureConvertor *_tconv= nullptr;

	/**
	 * @brief _setgen Settings generator
	 */
	Interfaces::ISettingsGenerator *_setgen= nullptr;

	/**
	 * @brief List with pointers to steps table spinboxes for ADC Deltas. This is used for determining what spinbox was changed.
	 */
	QList<QSpinBox*> _ADCDeltaSpinboxes ;

	/**
	 * @brief As _ADCDetlaSpinboxes, but for RPM deltas
	 */
	QList<QSpinBox*> _RPMDeltaSpinboxes;

	/**
	 * @brief Graph to display RPMs-themperature relation
	 */
	Fossa::QSimpleGraph::QSimpleGraph* _graph = nullptr;

	/**
	 * @brief Call this to initialize given steps table column. Requires initialized _ADCDetlaSpinboxes and _RPMDeltaSpinboxes.
	 * @param col Column number (starting from 0)
	 */
	void InitializeStepsTableColumn(uint col);

	/**
	 * @brief Call this method to lock or unlock UI elements, related to loaded file.
	 * @param isUnlock if true, then elements will be unlocked.
	 */
	void LockUnlockInterface(bool isUnlock);

};

#endif // MAINWINDOW_HPP
