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
	this->_settingsSaverLoader = new SettingsSaverLoader();

	// Connecting signals to slots
	QObject::connect(this->ui->mw_actionExit, SIGNAL(triggered(bool)), this, SLOT(MwSlotExit()));
	QObject::connect(this->ui->actionCreateNewADC2Temp, SIGNAL(triggered(bool)), this, SLOT(MwSlotCreateNewADC2Temp()));
	QObject::connect(this->ui->mw_basetemperature, SIGNAL(valueChanged(double)), this, SLOT(MwSlotBaseTemperatureChanged(double)));
	QObject::connect(this->ui->mw_baseRPM, SIGNAL(valueChanged(int)), this, SLOT(MwSlotBaseRPMChanged(int)));
	QObject::connect(this, SIGNAL(MwSignalADCDeltaChanged(uint,uint)), this, SLOT(MwSlotADCDeltaChanged(uint, uint)));
	QObject::connect(this, SIGNAL(MwSignalRPMDeltaChanged(uint,uint)), this, SLOT(MwSlotRPMDeltaChanged(uint, uint)));
	QObject::connect(this, SIGNAL(MwSignalUpdateStepsTable()), this, SLOT(MwSlotUpdateStepsTable()));
	QObject::connect(this->ui->actionNew, SIGNAL(triggered(bool)), this, SLOT(MwSlotCreateFile()));
	QObject::connect(this->ui->actionSave, SIGNAL(triggered(bool)), this, SLOT(MwSlotSaveFile()));
	QObject::connect(this->ui->actionSave_As, SIGNAL(triggered(bool)), this, SLOT(MwSlotSaveFileAs()));
	QObject::connect(this->ui->actionOpen, SIGNAL(triggered(bool)), this, SLOT(MwSlotLoadFile()));

	// Initializing status bar
	// ADC->Temperature conversion info
	this->_mwConversionStatus = new QLabel(this);
	this->ui->mw_status->addPermanentWidget(this->_mwConversionStatus);

	// Current filename
	this->_mwFileName = new QLabel(this);
	this->ui->mw_status->addPermanentWidget(this->_mwFileName);

	// Initializing step table (UI)
	this->ui->mw_StepsTable->setColumnCount(SettingsGenerator::TotalSteps);

	// Column names
	QStringList STColumnLabels;
	STColumnLabels.append(QObject::tr("Zero")); // Zero level
	STColumnLabels.append(QObject::tr("Base"));

	for (int i = 0; i < SettingsGenerator::StepsNumber; i++)
	{
		STColumnLabels.append(QString(QObject::tr("Step %1")).arg(i + 1));
	}

	this->ui->mw_StepsTable->setHorizontalHeaderLabels(STColumnLabels);

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

	this->ui->mw_StepsTable->setVerticalHeaderLabels(STRowLabels);

	// Preparing tables of spinboxes
	for (uint i = 0; i < SettingsGenerator::TotalSteps; i++)
	{
		this->_ADCDeltaSpinboxes.append(NULL);
		this->_RPMDeltaSpinboxes.append(NULL);
	}

	// Initializing each column (requires initialized _ADCDetlaSpinboxes and _RPMDeltaSpinboxes)
	for (uint i = 0; i < SettingsGenerator::TotalSteps; i++)
	{
		this->InitializeStepsTableColumn(i);
	}

	// Initializing the graph
	this->_graph = new Fossa::QSimpleGraph::QSimpleGraph(this);
	this->_graph->setMinimumSize(MainWindow::_minGraphWidth, MainWindow::_minGraphHeight);
	this->_graph->setSizePolicy(MainWindow::_graphHSizePolicy, MainWindow::_graphVSizePolicy);

	this->ui->mwMainLayout->insertWidget(0, this->_graph, MainWindow::_graphStretchFactor);

	// For debug
	this->_graph->SetMinXValue(0);
	this->_graph->SetMaxXValue(120);
	this->_graph->SetXAxisTitle(QObject::tr("Temperature"));

	this->_graph->SetMinYValue(0);
	this->_graph->SetMaxYValue(100);
	this->_graph->SetYAxisTitle(QObject::tr("RPMs, %"));

	// Actualizing UI steps table
	emit MwSignalUpdateStepsTable();

	// Actualizing ADC->Temperature convertor settings
	this->UpdateConvertorInformation();

	// And filename
	this->UpdateDisplayedFileName();
	this->LockUnlockInterface(false);
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
	if(this->_settingsSaverLoader->IsModified())
	{
		if (QMessageBox::Yes == QMessageBox::question(this, QObject::tr("Unsaved changes"), QObject::tr("There is unsaved changes. Are you really want to quit?")))
		{
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
	QCoreApplication::quit();
}

void MainWindow::UpdateConvertorInformation()
{
	Interfaces::IAdcTemperatureConvertor* conv = this->_settingsSaverLoader->GetADC2TempConvertorPtr();

	// Visualize new convertor
	double a, b;
	conv->GetADC2TempConversionFactors(&a, &b);

	QString format_str = QObject::tr("Sensor: %1: T = %2*ADC");
	if (b >= 0)
	{
		format_str += QObject::tr("+");
	}

	format_str += QObject::tr("%3");

	this->_mwConversionStatus->setText(QString(format_str).arg(conv->GetDescription()).arg(a).arg(b));
}

void MainWindow::MwSlotCreateNewADC2Temp()
{
		NewADC2TempDialog *dlg = new NewADC2TempDialog(this);
		dlg->exec();

		SafeDelete(dlg);
}

void MainWindow::MwSlotBaseTemperatureChanged(double tempC)
{
	Interfaces::ISettingsGenerator* setgen = this->_settingsSaverLoader->GetSettingsGeneratorPtr();

	setgen->SetBaseTemperature(tempC);
	setgen->CalculateSteps(); // Updating steps values

	// Setting temperature back (it may not be equal to entered by user due ADC granularity)
	QObject::disconnect(this->ui->mw_basetemperature, SIGNAL(valueChanged(double)), this, SLOT(MwSlotBaseTemperatureChanged(double))); // Disabling signal-slot connection to avoid recursion
	this->ui->mw_basetemperature->setValue(setgen->GetBaseTemperature());
	QObject::connect(this->ui->mw_basetemperature, SIGNAL(valueChanged(double)), this, SLOT(MwSlotBaseTemperatureChanged(double))); // Re-enabling it

	this->ui->mw_basetemperatureADC->setNum((int)setgen->GetBaseTemperatureADC());

	// Actualizing UI steps table
	emit MwSignalUpdateStepsTable();
}

void MainWindow::MwSlotBaseRPMChanged(int RPM)
{
	this->_settingsSaverLoader->GetSettingsGeneratorPtr()->SetBaseRPM(RPM);

	// Actualizing UI steps table
	emit MwSignalUpdateStepsTable();
}

void MainWindow::InitializeStepsTableColumn(uint col)
{
	QTableWidgetItem* labelItem;

	// ADC Level (label)
	labelItem = new QTableWidgetItem(QObject::tr("0"), QTableWidgetItem::Type);
	this->ui->mw_StepsTable->setItem(Ui::STEPS_TABLE_ROWS::ADC_LEVEL, col, labelItem);

	// Temperature (label)
	labelItem = new QTableWidgetItem(QObject::tr("0.0"), QTableWidgetItem::Type);
	this->ui->mw_StepsTable->setItem(Ui::STEPS_TABLE_ROWS::TEMPERATURE, col, labelItem);

	// RPM level (label)
	labelItem = new QTableWidgetItem(QObject::tr("0"), QTableWidgetItem::Type);
	this->ui->mw_StepsTable->setItem(Ui::STEPS_TABLE_ROWS::RPM_LEVEL, col, labelItem);

	// RPM percent (label)
	labelItem = new QTableWidgetItem(QObject::tr("0.0%"), QTableWidgetItem::Type);
	this->ui->mw_StepsTable->setItem(Ui::STEPS_TABLE_ROWS::RPM_PERCENT, col, labelItem);

	// ADC level increase (spinbox)
	this->_ADCDeltaSpinboxes[col] = new QSpinBox(this->ui->mw_StepsTable);
	this->ui->mw_StepsTable->setCellWidget(Ui::STEPS_TABLE_ROWS::ADC_DELTA, col, this->_ADCDeltaSpinboxes[col]);

	// Temperature increase (label)
	labelItem = new QTableWidgetItem(QObject::tr("0.0"), QTableWidgetItem::Type);
	this->ui->mw_StepsTable->setItem(Ui::STEPS_TABLE_ROWS::TEMPERATURE_DELTA, col, labelItem);

	// RPM increase (spinbox)
	this->_RPMDeltaSpinboxes[col] = new QSpinBox(this->ui->mw_StepsTable);
	this->ui->mw_StepsTable->setCellWidget(Ui::STEPS_TABLE_ROWS::RPM_DELTA, col, this->_RPMDeltaSpinboxes[col]);

	// RPM increase percent (label)
	labelItem = new QTableWidgetItem(QObject::tr("0.0%"), QTableWidgetItem::Type);
	this->ui->mw_StepsTable->setItem(Ui::STEPS_TABLE_ROWS::RPM_DELTA_PERCENT, col, labelItem);

	if (col < SettingsGenerator::AdditionalSteps)
	{
		this->_ADCDeltaSpinboxes[col]->setReadOnly(true);
		this->_RPMDeltaSpinboxes[col]->setReadOnly(true);
	}

	// Overriding ranges
	if (SettingsGenerator::BaseLevelsStepIndex == col)
	{
		// Base step, it have wide range
		this->_ADCDeltaSpinboxes[col]->setRange(0, ADC_MAX_VALUE);
		this->_RPMDeltaSpinboxes[col]->setRange(0, MAX_RPM);
	}
	else
	{
		this->_ADCDeltaSpinboxes[col]->setRange(MIN_ADC_DELTA, MAX_ADC_DELTA);
		this->_RPMDeltaSpinboxes[col]->setRange(0, MAX_RPM_DELTA);
	}

	// Connecting here to avoid calling slots during values/ranges initialization
	QObject::connect(this->_RPMDeltaSpinboxes[col], SIGNAL(valueChanged(int)), this, SLOT(MwSlotRPMDeltaChangedRaw(int)));
	QObject::connect(this->_ADCDeltaSpinboxes[col], SIGNAL(valueChanged(int)), this, SLOT(MwSlotADCDeltaChangedRaw(int)));
}

void MainWindow::MwSlotADCDeltaChangedRaw(int newDelta)
{
	// Detecting sender
	int senderColumn = this->_ADCDeltaSpinboxes.indexOf((QSpinBox*)QObject::sender());

	if (senderColumn < 0)
	{
		qFatal("Pointer of ADC Spinbox is not found in _ADCDeltaSpinboxes list. Wrong signal-slot connection?");
	}

	emit MwSignalADCDeltaChanged((uint)senderColumn, (uint)newDelta);
}

void MainWindow::MwSlotRPMDeltaChangedRaw(int newDelta)
{
	// Detecting sender
	int senderColumn = this->_RPMDeltaSpinboxes.indexOf((QSpinBox*)QObject::sender());

	if (senderColumn < 0)
	{
		qFatal("Pointer of RPM Spinbox is not found in _RPMDeltaSpinboxes list. Wrong signal-slot connection?");
	}

	emit MwSignalRPMDeltaChanged((uint)senderColumn, (uint)newDelta);
}

void MainWindow::MwSlotADCDeltaChanged(uint StepNumber, uint NewDelta)
{
	Interfaces::ISettingsGenerator* setgen = this->_settingsSaverLoader->GetSettingsGeneratorPtr();
	setgen->GetStepPtr(StepNumber)->SetADCDelta(NewDelta);
	setgen->CalculateSteps();

	// Actualizing UI steps table
	emit MwSignalUpdateStepsTable();
}

void MainWindow::MwSlotRPMDeltaChanged(uint StepNumber, uint NewDelta)
{
	Interfaces::ISettingsGenerator* setgen = this->_settingsSaverLoader->GetSettingsGeneratorPtr();
	setgen->GetStepPtr(StepNumber)->SetRPMDelta(NewDelta);
	setgen->CalculateSteps();

	// Actualizing UI steps table
	emit MwSignalUpdateStepsTable();
}

void MainWindow::MwSlotUpdateStepsTable()
{
	Interfaces::ISettingsGenerator* setgen = this->_settingsSaverLoader->GetSettingsGeneratorPtr();

	// We can do nothing is setgen or this->_graph is not ready yet
	if ((NULL == setgen) || (NULL == this->_graph))
	{
		return;
	}

	// Removing old points from graph
	this->_graph->ClearAllPoints();
	this->_graph->SetMinXValue(0); // TODO: Fix
	this->_graph->SetMaxXValue(0);
	this->_graph->SetMinYValue(0);
	this->_graph->SetMaxYValue(0);

	Interfaces::ISettingsStep *stepItem;

	for (uint step = 0; step < SettingsGenerator::TotalSteps; step++)
	{
		// Getting step
		stepItem = setgen->GetStepPtr(step);

		// ADC level
		this->ui->mw_StepsTable->item(Ui::STEPS_TABLE_ROWS::ADC_LEVEL, step)->setText(QString("%1").arg(stepItem->GetCurrentADC()));

		// Temperature
		this->ui->mw_StepsTable->item(Ui::STEPS_TABLE_ROWS::TEMPERATURE, step)->setText(QString("%1 °C").arg(stepItem->GetCurrentTemperature()));

		// RPM level
		this->ui->mw_StepsTable->item(Ui::STEPS_TABLE_ROWS::RPM_LEVEL, step)->setText(QString("%1").arg(stepItem->GetCurrentRPM()));

		// RPM percent
		this->ui->mw_StepsTable->item(Ui::STEPS_TABLE_ROWS::RPM_PERCENT, step)->setText(stepItem->GetCurrentRPMPercentsString());

		// ADC level increase
		((QSpinBox*)this->ui->mw_StepsTable->cellWidget(Ui::STEPS_TABLE_ROWS::ADC_DELTA, step))->setValue(stepItem->GetADCDelta());

		// Temperature increase
		this->ui->mw_StepsTable->item(Ui::STEPS_TABLE_ROWS::TEMPERATURE_DELTA, step)->setText(QString("%1 °C").arg(stepItem->GetTempDelta()));

		// RPM level increase
		((QSpinBox*)this->ui->mw_StepsTable->cellWidget(Ui::STEPS_TABLE_ROWS::RPM_DELTA, step))->setValue(stepItem->GetRPMDelta());

		// RPM level increase (percents)
		this->ui->mw_StepsTable->item(Ui::STEPS_TABLE_ROWS::RPM_DELTA_PERCENT, step)->setText(QString("%1%").arg(stepItem->GetRPMDeltaPercents()));

		// Adding point to graph
		this->_graph->AddPoint(stepItem->GetCurrentTemperature(), stepItem->GetCurrentRPMPercents());
	}

	this->_graph->update();
}

void MainWindow::UpdateDisplayedFileName()
{
	QString filePath = this->_settingsSaverLoader->GetFilePath();

	QString windowTitle = "";
	if (!filePath.isEmpty())
	{
		QString modifiedSign = "";
		if (this->_settingsSaverLoader->IsModified())
		{
			modifiedSign = QObject::tr("*");
		}

		windowTitle = QObject::tr("%1%2 - ").arg(filePath).arg(modifiedSign);
		this->_mwFileName->setText(filePath);
	}
	else
	{
		this->_mwFileName->setText(QObject::tr("File not specified"));
	}

	windowTitle += QObject::tr("Project \"LowBlow\" Control Tool");

	this->setWindowTitle(windowTitle);
}

void MainWindow::LockUnlockInterface(bool isUnlock)
{
	this->ui->actionSave->setEnabled(isUnlock);
	this->ui->actionSave_As->setEnabled(isUnlock);
	this->ui->mw_basetemperature->setEnabled(isUnlock);
	this->ui->mw_baseRPM->setEnabled(isUnlock);
	this->_graph->setEnabled(isUnlock);
	this->ui->mw_StepsTable->setEnabled(isUnlock);
}


void MainWindow::MwSlotCreateFile()
{
	// Do current file saved?
	CheckDoSaveNeeded();

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

	this->_settingsSaverLoader->Create(newFilePath, ADC2TempFilePath);

	this->InitializeAfterFileChanged();
}


void MainWindow::MwSlotLoadFile()
{
	// Do save needed?
	CheckDoSaveNeeded();

	// Load dialog
	QString loadFilePath = QFileDialog::getOpenFileName(this, QObject::tr("Select file to open:"));

	if ("" == loadFilePath)
	{
		return;
	}

	if (!this->_settingsSaverLoader->Load(loadFilePath))
	{
		QMessageBox::warning(this, QObject::tr("Failed to load selected file"), QObject::tr("Failed to load selected file"));
		return;
	}

	this->InitializeAfterFileChanged();
}

void MainWindow::MwSlotSaveFile()
{
	this->_settingsSaverLoader->Save();

	this->UpdateDisplayedFileName();
}

void MainWindow::MwSlotSaveFileAs()
{
	// Showing save dialogue
	QString saveAsPath = QFileDialog::getSaveFileName(this, QObject::tr("Save settings as:"));

	if (saveAsPath == "")
	{
		return;
	}

	if (saveAsPath == this->_settingsSaverLoader->GetFilePath())
	{
		QMessageBox::warning(this, QObject::tr("Can't override opened file!"),
		QObject::tr("You are trying to override currently opened file. Use \"Save\", not \"Save As\" to do it."),
		QMessageBox::Ok
		);
	}

	this->_settingsSaverLoader->SaveAs(saveAsPath);
}


void MainWindow::InitializeAfterFileChanged()
{
	this->UpdateDisplayedFileName();
	this->UpdateConvertorInformation();

	// Setting base parameters
	Interfaces::ISettingsGenerator* setgen = this->_settingsSaverLoader->GetSettingsGeneratorPtr();
	this->ui->mw_basetemperature->setValue(setgen->GetBaseTemperature());
	this->MwSlotBaseTemperatureChanged(this->ui->mw_basetemperature->value()); // Calling explicitly, because ValueChanged() signal may not be emitted
	// if new value is equal to old one.

	this->ui->mw_baseRPM->setValue(setgen->GetBaseRPM());
	this->MwSlotBaseRPMChanged(this->ui->mw_baseRPM->value());

	this->LockUnlockInterface(true);
}

void MainWindow::CheckDoSaveNeeded()
{
if (this->_settingsSaverLoader->IsModified())
	{
		// Saving if needed
		if (QMessageBox::Yes == QMessageBox::question(this, QObject::tr("Save changes?"), QObject::tr("Save changes before creating new settings file?"),
		QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes))
		{
			this->_settingsSaverLoader->Save();
		}
	}
}

MainWindow::~MainWindow()
{
	SafeDelete(this->_mwConversionStatus);

	SafeDelete(this->_mwFileName);

	SafeDelete(this->_settingsSaverLoader);

	SafeDelete(ui);
}
