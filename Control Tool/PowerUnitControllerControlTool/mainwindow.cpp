/*
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

#include "mainwindow.hpp"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	// Saver/loader
	_settingsSaverLoader = new SettingsSaverLoader();

	// Connecting signals to slots
	QObject::connect(ui->mw_actionExit, SIGNAL(triggered(bool)), this, SLOT(MwSlotExit()));
	QObject::connect(ui->actionNewADC2Temp, SIGNAL(triggered(bool)), this, SLOT(MwSlotCreateNewADC2Temp()));
	QObject::connect(this, SIGNAL(MwSignalUpdateStepsTable()), this, SLOT(MwSlotUpdateStepsTable()));
	QObject::connect(ui->actionNew, SIGNAL(triggered(bool)), this, SLOT(MwSlotCreateFile()));
	QObject::connect(ui->actionSave, SIGNAL(triggered(bool)), this, SLOT(MwSlotSaveFile()));
	QObject::connect(ui->actionSave_As, SIGNAL(triggered(bool)), this, SLOT(MwSlotSaveFileAs()));
	QObject::connect(ui->actionOpen, SIGNAL(triggered(bool)), this, SLOT(MwSlotLoadFile()));
	QObject::connect(this, SIGNAL(MwSignalFileModified()), this, SLOT(MwSlotMarkAsModified()));
	QObject::connect(ui->actionExport_to_EEPROM, SIGNAL(triggered(bool)), this, SLOT(MwSlotExportToEEPROM()));
	ConnectUISlots(true);

	// Initializing status bar
	// ADC->Temperature conversion info
	_mwConversionStatus = new QLabel(this);
	ui->mw_status->addPermanentWidget(this->_mwConversionStatus);

	// Current filename
	_mwFileName = new QLabel(this);
	ui->mw_status->addPermanentWidget(this->_mwFileName);

	// Initializing step table (UI)
	ui->mw_StepsTable->setColumnCount(Interfaces::ISettingsGenerator::TotalSteps);

	// Column names
	QStringList STColumnLabels;
	STColumnLabels.append(QObject::tr("Zero")); // Zero level
	STColumnLabels.append(QObject::tr("Base"));

	for (uint i = 0; i < Interfaces::ISettingsGenerator::StepsNumber; i++)
	{
		STColumnLabels.append(QString(QObject::tr("Step %1")).arg(i + 1));
	}

	ui->mw_StepsTable->setHorizontalHeaderLabels(STColumnLabels);

	// Row names
	QStringList STRowLabels;
	STRowLabels.append(QObject::tr("ADC level"));
	STRowLabels.append(QObject::tr("Temperature"));
	STRowLabels.append(QObject::tr("RPM level"));
	STRowLabels.append(QObject::tr("RPM percent"));
	STRowLabels.append(QObject::tr("ADC level increase"));
	STRowLabels.append(QObject::tr("Temperature increase"));
	STRowLabels.append(QObject::tr("RPM level increase"));
	STRowLabels.append(QObject::tr("RPM increase percent"));

	ui->mw_StepsTable->setVerticalHeaderLabels(STRowLabels);

	// Preparing tables of spinboxes
	for (uint i = 0; i < Interfaces::ISettingsGenerator::TotalSteps; i++)
	{
		_ADCDeltaSpinboxes.append(NULL);
		_RPMDeltaSpinboxes.append(NULL);
	}

	// Initializing each column (requires initialized _ADCDetlaSpinboxes and _RPMDeltaSpinboxes)
	for (uint i = 0; i < Interfaces::ISettingsGenerator::TotalSteps; i++)
	{
		InitializeStepsTableColumn(i);
	}

	// Initializing the graph
	_graph = new Fossa::QSimpleGraph::QSimpleGraph(this);
	_graph->setMinimumSize(MainWindow::_minGraphWidth, MainWindow::_minGraphHeight);
	_graph->setSizePolicy(MainWindow::_graphHSizePolicy, MainWindow::_graphVSizePolicy);

	ui->mwMainLayout->insertWidget(0, _graph, MainWindow::_graphStretchFactor);

	_graph->SetXAxisTitle(QObject::tr("Temperature"));
	_graph->SetYAxisTitle(QObject::tr("RPMs, %"));

	// Actualizing UI steps table
	emit MwSignalUpdateStepsTable();

	// Actualizing ADC->Temperature convertor settings
	UpdateConvertorInformation();

	// And filename
	UpdateDisplayedFileName();
	LockUnlockInterface(false);
}

void MainWindow::changeEvent(QEvent *e)
{
	QMainWindow::changeEvent(e);
	switch (e->type()) {
		case QEvent::LanguageChange:
			ui->retranslateUi(this);
			break;
		default:
			break;
		}
}

void MainWindow::closeEvent(QCloseEvent *e)
{
	// Do we have unsaved changes?
	if(_settingsSaverLoader->IsModified())
	{
		if (QMessageBox::Yes == QMessageBox::question(this, QObject::tr("Unsaved changes"), QObject::tr("There is unsaved changes. Are you really want to quit?")))
		{
			MwSlotSaveFile();
			e->accept();
		}
		else
		{
			e->ignore();
		}
	}
}

void MainWindow::MwSlotExit()
{
	close();
}

void MainWindow::UpdateConvertorInformation()
{
	Interfaces::IAdcTemperatureConvertor* conv = _settingsSaverLoader->GetADC2TempConvertorPtr();

	// Visualize new convertor
	double a, b;
	conv->GetADC2TempConversionFactors(&a, &b);

	QString format_str = QObject::tr("Sensor: %1: T = %2*ADC");
	if (b >= 0)
	{
		format_str += QObject::tr("+");
	}

	format_str += QObject::tr("%3");

	_mwConversionStatus->setText(QString(format_str).arg(conv->GetDescription()).arg(a).arg(b));
}

void MainWindow::MwSlotCreateNewADC2Temp()
{
		NewADC2TempDialog *dlg = new NewADC2TempDialog(this);
		dlg->exec();

		SafeDelete(dlg);
}

void MainWindow::MwSlotBaseTemperatureChanged(double tempC)
{
	UpdateBaseTemperature(tempC);

	// Marking file as modified
	emit MwSignalFileModified();
}

void MainWindow::UpdateBaseTemperature(double baseTemp)
{
	Interfaces::ISettingsGenerator* setgen = _settingsSaverLoader->GetSettingsGeneratorPtr();

	setgen->SetBaseTemperature(baseTemp);
	setgen->CalculateSteps(); // Updating steps values

	// Setting temperature back (it may not be equal to entered by user due ADC granularity)
	QObject::disconnect(ui->mw_basetemperature, SIGNAL(valueChanged(double)), this, SLOT(MwSlotBaseTemperatureChanged(double))); // Disabling signal-slot connection to avoid recursion
	ui->mw_basetemperature->setValue(setgen->GetBaseTemperature());
	QObject::connect(ui->mw_basetemperature, SIGNAL(valueChanged(double)), this, SLOT(MwSlotBaseTemperatureChanged(double))); // Re-enabling it

	ui->mw_basetemperatureADC->setNum((int)setgen->GetBaseTemperatureADC());

	// Actualizing UI steps table
	emit MwSignalUpdateStepsTable();
}

void MainWindow::MwSlotBaseRPMChanged(int RPM)
{
	UpdateBaseRPMs(RPM);

	// Marking file as modified
	emit MwSignalFileModified();
}

void MainWindow::UpdateBaseRPMs(int RPM)
{
	_settingsSaverLoader->GetSettingsGeneratorPtr()->SetBaseRPM(RPM);

	// Actualizing UI steps table
	emit MwSignalUpdateStepsTable();
}

void MainWindow::InitializeStepsTableColumn(uint col)
{
	QTableWidgetItem* labelItem;

	// ADC Level (label)
	labelItem = new QTableWidgetItem(QObject::tr("0"), QTableWidgetItem::Type);
	ui->mw_StepsTable->setItem(Ui::STEPS_TABLE_ROWS::ADC_LEVEL, col, labelItem);

	// Temperature (label)
	labelItem = new QTableWidgetItem(QObject::tr("0.0"), QTableWidgetItem::Type);
	ui->mw_StepsTable->setItem(Ui::STEPS_TABLE_ROWS::TEMPERATURE, col, labelItem);

	// RPM level (label)
	labelItem = new QTableWidgetItem(QObject::tr("0"), QTableWidgetItem::Type);
	ui->mw_StepsTable->setItem(Ui::STEPS_TABLE_ROWS::RPM_LEVEL, col, labelItem);

	// RPM percent (label)
	labelItem = new QTableWidgetItem(QObject::tr("0.0%"), QTableWidgetItem::Type);
	ui->mw_StepsTable->setItem(Ui::STEPS_TABLE_ROWS::RPM_PERCENT, col, labelItem);

	// ADC level increase (spinbox)
	_ADCDeltaSpinboxes[col] = new QSpinBox(ui->mw_StepsTable);
	ui->mw_StepsTable->setCellWidget(Ui::STEPS_TABLE_ROWS::ADC_DELTA, col, _ADCDeltaSpinboxes[col]);

	// Temperature increase (label)
	labelItem = new QTableWidgetItem(QObject::tr("0.0"), QTableWidgetItem::Type);
	ui->mw_StepsTable->setItem(Ui::STEPS_TABLE_ROWS::TEMPERATURE_DELTA, col, labelItem);

	// RPM increase (spinbox)
	_RPMDeltaSpinboxes[col] = new QSpinBox(ui->mw_StepsTable);
	ui->mw_StepsTable->setCellWidget(Ui::STEPS_TABLE_ROWS::RPM_DELTA, col, _RPMDeltaSpinboxes[col]);

	// RPM increase percent (label)
	labelItem = new QTableWidgetItem(QObject::tr("0.0%"), QTableWidgetItem::Type);
	ui->mw_StepsTable->setItem(Ui::STEPS_TABLE_ROWS::RPM_DELTA_PERCENT, col, labelItem);

	if (col < Interfaces::ISettingsGenerator::AdditionalSteps)
	{
		_ADCDeltaSpinboxes[col]->setReadOnly(true);
		_RPMDeltaSpinboxes[col]->setReadOnly(true);
	}

	// Overriding ranges
	if (SettingsGenerator::ZeroLevelsStepIndex == col)
	{
		// Zero step, always have zeros
		_ADCDeltaSpinboxes[col]->setRange(0, 0);
		_RPMDeltaSpinboxes[col]->setRange(0, 0);
	}
	else if (SettingsGenerator::BaseLevelsStepIndex == col)
	{
		// Base step, it have wide range
		_ADCDeltaSpinboxes[col]->setRange(0, AdcTemperatureConvertor::MaxADCValue);
		_RPMDeltaSpinboxes[col]->setRange(0, MAX_RPM);
	}
	else
	{
		_ADCDeltaSpinboxes[col]->setRange(MIN_ADC_DELTA, MAX_ADC_DELTA);
		_RPMDeltaSpinboxes[col]->setRange(0, MAX_RPM_DELTA);
	}

	// Connecting here to avoid calling slots during values/ranges initialization
	QObject::connect(_RPMDeltaSpinboxes[col], SIGNAL(valueChanged(int)), this, SLOT(MwSlotRPMDeltaChangedRaw(int)));
	QObject::connect(_ADCDeltaSpinboxes[col], SIGNAL(valueChanged(int)), this, SLOT(MwSlotADCDeltaChangedRaw(int)));
}

void MainWindow::MwSlotADCDeltaChangedRaw(int newDelta)
{
	// Detecting sender
	int senderColumn = _ADCDeltaSpinboxes.indexOf((QSpinBox*)QObject::sender());

	if (senderColumn < 0)
	{
		qFatal("Pointer of ADC Spinbox is not found in _ADCDeltaSpinboxes list. Wrong signal-slot connection?");
	}

	emit MwSignalADCDeltaChanged((uint)senderColumn, (uint)newDelta);
}

void MainWindow::MwSlotRPMDeltaChangedRaw(int newDelta)
{
	// Detecting sender
	int senderColumn = _RPMDeltaSpinboxes.indexOf((QSpinBox*)QObject::sender());

	if (senderColumn < 0)
	{
		qFatal("Pointer of RPM Spinbox is not found in _RPMDeltaSpinboxes list. Wrong signal-slot connection?");
	}

	emit MwSignalRPMDeltaChanged((uint)senderColumn, (uint)newDelta);
}

void MainWindow::MwSlotADCDeltaChanged(uint StepNumber, uint NewDelta)
{
	Interfaces::ISettingsGenerator* setgen = _settingsSaverLoader->GetSettingsGeneratorPtr();
	setgen->GetStepPtr(StepNumber)->SetADCDelta(NewDelta);
	setgen->CalculateSteps();

	// Marking file as modified
	emit MwSignalFileModified();

	// Actualizing UI steps table
	emit MwSignalUpdateStepsTable();
}

void MainWindow::MwSlotRPMDeltaChanged(uint StepNumber, uint NewDelta)
{
	Interfaces::ISettingsGenerator* setgen = _settingsSaverLoader->GetSettingsGeneratorPtr();
	setgen->GetStepPtr(StepNumber)->SetRPMDelta(NewDelta);
	setgen->CalculateSteps();

	// Marking file as modified
	emit MwSignalFileModified();

	// Actualizing UI steps table
	emit MwSignalUpdateStepsTable();
}

void MainWindow::MwSlotUpdateStepsTable()
{
	Interfaces::ISettingsGenerator* setgen = _settingsSaverLoader->GetSettingsGeneratorPtr();

	// We can do nothing is setgen or _graph is not ready yet
	if ((NULL == setgen) || (NULL == _graph))
	{
		return;
	}

	// Removing old points from graph
	_graph->ClearAllPoints();
	_graph->SetMinXValue(0); // TODO: Fix
	_graph->SetMaxXValue(0);
	_graph->SetMinYValue(0);
	_graph->SetMaxYValue(0);

	Interfaces::ISettingsStep *stepItem;

	for (uint step = 0; step < Interfaces::ISettingsGenerator::TotalSteps; step++)
	{
		// Getting step
		stepItem = setgen->GetStepPtr(step);

		// ADC level
		ui->mw_StepsTable->item(Ui::STEPS_TABLE_ROWS::ADC_LEVEL, step)->setText(QString("%1").arg(stepItem->GetCurrentADC()));

		// Temperature
		ui->mw_StepsTable->item(Ui::STEPS_TABLE_ROWS::TEMPERATURE, step)->setText(QString("%1 °C").arg(stepItem->GetCurrentTemperature()));

		// RPM level
		ui->mw_StepsTable->item(Ui::STEPS_TABLE_ROWS::RPM_LEVEL, step)->setText(QString("%1").arg(stepItem->GetCurrentRPM()));

		// RPM percent
		ui->mw_StepsTable->item(Ui::STEPS_TABLE_ROWS::RPM_PERCENT, step)->setText(stepItem->GetCurrentRPMPercentsString());

		// ADC level increase
		((QSpinBox*)ui->mw_StepsTable->cellWidget(Ui::STEPS_TABLE_ROWS::ADC_DELTA, step))->setValue(stepItem->GetADCDelta());

		// Temperature increase
		ui->mw_StepsTable->item(Ui::STEPS_TABLE_ROWS::TEMPERATURE_DELTA, step)->setText(QString("%1 °C").arg(stepItem->GetTempDelta()));

		// RPM level increase
		((QSpinBox*)ui->mw_StepsTable->cellWidget(Ui::STEPS_TABLE_ROWS::RPM_DELTA, step))->setValue(stepItem->GetRPMDelta());

		// RPM level increase (percents)
		ui->mw_StepsTable->item(Ui::STEPS_TABLE_ROWS::RPM_DELTA_PERCENT, step)->setText(QString("%1%").arg(stepItem->GetRPMDeltaPercents()));

		// Adding point to graph
		_graph->AddPoint(stepItem->GetCurrentTemperature(), stepItem->GetCurrentRPMPercents());
	}

	_graph->update();
}

void MainWindow::UpdateDisplayedFileName()
{
	QString filePath = _settingsSaverLoader->GetFilePath();

	QString windowTitle = "";
	if (!filePath.isEmpty())
	{
		QString modifiedSign = "";
		if (_settingsSaverLoader->IsModified())
		{
			modifiedSign = QObject::tr("*");
		}

		windowTitle = QObject::tr("%1%2 - ").arg(filePath).arg(modifiedSign);
		_mwFileName->setText(filePath);
	}
	else
	{
		_mwFileName->setText(QObject::tr("File not specified"));
	}

	windowTitle += QObject::tr("Project \"LowBlow\" Control Tool");

	setWindowTitle(windowTitle);
}

void MainWindow::LockUnlockInterface(bool isUnlock)
{
	ui->actionSave->setEnabled(isUnlock);
	ui->actionSave_As->setEnabled(isUnlock);
	ui->actionExport_to_EEPROM->setEnabled(isUnlock);
	ui->mw_basetemperature->setEnabled(isUnlock);
	ui->mw_baseRPM->setEnabled(isUnlock);
	_graph->setEnabled(isUnlock);
	ui->mw_StepsTable->setEnabled(isUnlock);
}

void MainWindow::MwSlotCreateFile()
{
	// Do current file saved?
	CheckDoSaveNeeded(QObject::tr("creating new settings file"));

	// Settings filename
	QString newFilePath = QFileDialog::getSaveFileName(this, QObject::tr("New settings file:"));

	if ("" == newFilePath)
	{
		return;
	}

	// ADC->Temperature settings file
	QString ADC2TempFilePath = QFileDialog::getOpenFileName(this, QObject::tr("Select ADC to temperature settings file:"));

	if ("" == ADC2TempFilePath)
	{
		return;
	}

	_settingsSaverLoader->Create(newFilePath, ADC2TempFilePath);

	InitializeAfterFileChanged();
}


void MainWindow::MwSlotLoadFile()
{
	// Do save needed?
	CheckDoSaveNeeded(QObject::tr("before loading new settings file"));

	ConnectUISlots(false); // To avoid marking file as modified

	// Load dialog
	QString loadFilePath = QFileDialog::getOpenFileName(this, QObject::tr("Select file to open:"));

	if ("" == loadFilePath)
	{
		ConnectUISlots(true);
		return;
	}

	if (!_settingsSaverLoader->Load(loadFilePath))
	{
		ConnectUISlots(true);
		QMessageBox::warning(this, QObject::tr("Failed to load selected file"), QObject::tr("Failed to load selected file"));
		return;
	}

	InitializeAfterFileChanged();
	ConnectUISlots(true);
}

void MainWindow::MwSlotSaveFile()
{
	_settingsSaverLoader->Save();

	UpdateDisplayedFileName();
}

void MainWindow::MwSlotSaveFileAs()
{
	// Showing save dialogue
	QString saveAsPath = QFileDialog::getSaveFileName(this, QObject::tr("Save settings as:"));

	if (saveAsPath == "")
	{
		return;
	}

	if (saveAsPath == _settingsSaverLoader->GetFilePath())
	{
		QMessageBox::warning(this, QObject::tr("Can't override opened file!"),
		QObject::tr("You are trying to override currently opened file. Use \"Save\", not \"Save As\" to do it."),
		QMessageBox::Ok
		);
	}

	_settingsSaverLoader->SaveAs(saveAsPath);
}


void MainWindow::InitializeAfterFileChanged()
{
	UpdateDisplayedFileName();
	UpdateConvertorInformation();

	// Setting base parameters
	Interfaces::ISettingsGenerator* setgen = _settingsSaverLoader->GetSettingsGeneratorPtr();
	ui->mw_basetemperature->setValue(setgen->GetBaseTemperature());
	UpdateBaseTemperature(ui->mw_basetemperature->value());

	ui->mw_baseRPM->setValue(setgen->GetBaseRPM());
	UpdateBaseRPMs(ui->mw_baseRPM->value());

	LockUnlockInterface(true);
}

void MainWindow::CheckDoSaveNeeded(QString action)
{
if (_settingsSaverLoader->IsModified())
	{
		// Saving if needed
		if (QMessageBox::Yes == QMessageBox::question(this, QObject::tr("Save changes?"), QObject::tr("Save changes before %1?").arg(action),
		QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes))
		{
			_settingsSaverLoader->Save();

			UpdateDisplayedFileName();
		}
	}
}

void MainWindow::MwSlotMarkAsModified()
{
	_settingsSaverLoader->MarkAsModified();
	UpdateDisplayedFileName();
}

void MainWindow::MwSlotExportToEEPROM()
{
	// Do save needed?
	CheckDoSaveNeeded(QObject::tr("exporting settings to EEPROM"));

	// Showing save dialogue
	QFileDialog* saveDialog = new QFileDialog(this, Qt::Dialog);
	QString extFilter = QObject::tr("Intel HEX (*.hex)");
	QString savePath = saveDialog->getSaveFileName(this, QObject::tr("Export EEPROM contents into file:"), saveDialog->directory().absolutePath(), extFilter, &extFilter);

	if (savePath == "")
	{
		return;
	}

	_settingsSaverLoader->ExportToEEPROM(savePath);
}

void MainWindow::ConnectUISlots(bool isConnect)
{
	if (isConnect)
	{
		QObject::connect(ui->mw_basetemperature, SIGNAL(valueChanged(double)), this, SLOT(MwSlotBaseTemperatureChanged(double)));
		QObject::connect(ui->mw_baseRPM, SIGNAL(valueChanged(int)), this, SLOT(MwSlotBaseRPMChanged(int)));
		QObject::connect(this, SIGNAL(MwSignalADCDeltaChanged(uint,uint)), this, SLOT(MwSlotADCDeltaChanged(uint, uint)));
		QObject::connect(this, SIGNAL(MwSignalRPMDeltaChanged(uint,uint)), this, SLOT(MwSlotRPMDeltaChanged(uint, uint)));
	}
	else
	{
		QObject::disconnect(ui->mw_basetemperature, SIGNAL(valueChanged(double)), this, SLOT(MwSlotBaseTemperatureChanged(double)));
		QObject::disconnect(ui->mw_baseRPM, SIGNAL(valueChanged(int)), this, SLOT(MwSlotBaseRPMChanged(int)));
		QObject::disconnect(this, SIGNAL(MwSignalADCDeltaChanged(uint,uint)), this, SLOT(MwSlotADCDeltaChanged(uint, uint)));
		QObject::disconnect(this, SIGNAL(MwSignalRPMDeltaChanged(uint,uint)), this, SLOT(MwSlotRPMDeltaChanged(uint, uint)));
	}
}

MainWindow::~MainWindow()
{
	SafeDelete(_mwConversionStatus);

	SafeDelete(_mwFileName);

	SafeDelete(_settingsSaverLoader);

	SafeDelete(ui);
}
