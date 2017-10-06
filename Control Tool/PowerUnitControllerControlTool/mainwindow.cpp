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
	QObject::connect(this, SIGNAL(MwSignalConvertorChanged()), this, SLOT(MwSlotConvertorChanged()));
	QObject::connect(this->ui->mw_basetemperature, SIGNAL(valueChanged(double)), this, SLOT(MwSlotBaseTemperatureChanged(double)));
	QObject::connect(this->ui->mw_baseRPM, SIGNAL(valueChanged(int)), this, SLOT(MwSlotBaseRPMChanged(int)));
	QObject::connect(this, SIGNAL(MwSignalADCDeltaChanged(uint,uint)), this, SLOT(MwSlotADCDeltaChanged(uint, uint)));
	QObject::connect(this, SIGNAL(MwSignalRPMDeltaChanged(uint,uint)), this, SLOT(MwSlotRPMDeltaChanged(uint, uint)));
	QObject::connect(this, SIGNAL(MwSignalUpdateStepsTable()), this, SLOT(MwSlotUpdateStepsTable()));

	// Initializing status bar
	// ADC->Temperature conversion info
	this->_mwConversionStatus = new QLabel(this);
	this->ui->mw_status->addPermanentWidget(this->_mwConversionStatus);

	// Current filename
	this->_mwFileName = new QLabel(this);
	this->ui->mw_status->addPermanentWidget(this->_mwFileName);

	// Temperature convertor
	this->_tconv = new AdcTemperatureConvertor();
	emit MwSignalConvertorChanged();

	this->_setgen = new SettingsGenerator(this->_tconv);
	this->_setgen->InitializeStepsList(STEPS_NUMBER);

	// Initializing step table (UI)
	this->ui->mw_StepsTable->setColumnCount(NUMBER_OF_STEPS_TABLE_RECORDS);

	// Column names
	QStringList STColumnLabels;
	STColumnLabels.append(QObject::trUtf8("Zero")); // Zero level
	STColumnLabels.append(QObject::trUtf8("Base"));

	for (int i = 0; i < STEPS_NUMBER; i++)
	{
		STColumnLabels.append(QString(QObject::trUtf8("Step %1")).arg(i + 1));
	}

	this->ui->mw_StepsTable->setHorizontalHeaderLabels(STColumnLabels);

	// Row names
	QStringList STRowLabels;
	STRowLabels.append(QObject::trUtf8("ADC level"));
	STRowLabels.append(QObject::trUtf8("Temperature"));
	STRowLabels.append(QObject::trUtf8("RPM level"));
	STRowLabels.append(QObject::trUtf8("RPM percent"));
	STRowLabels.append(QObject::trUtf8("ADC level increase"));
	STRowLabels.append(QObject::trUtf8("Temperature increase"));
	STRowLabels.append(QObject::trUtf8("RPM level increase"));
	STRowLabels.append(QObject::trUtf8("RPM increase percent"));

	this->ui->mw_StepsTable->setVerticalHeaderLabels(STRowLabels);

	// Preparing tables of spinboxes
	for (uint i = 0; i < NUMBER_OF_STEPS_TABLE_RECORDS; i++)
	{
		this->_ADCDeltaSpinboxes.append(NULL);
		this->_RPMDeltaSpinboxes.append(NULL);
	}

	// Initializing each column (requires initialized _ADCDetlaSpinboxes and _RPMDeltaSpinboxes)
	for (uint i = 0; i < NUMBER_OF_STEPS_TABLE_RECORDS; i++)
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
	this->_graph->SetXAxisTitle(QObject::trUtf8("Temperature"));

	this->_graph->SetMinYValue(0);
	this->_graph->SetMaxYValue(100);
	this->_graph->SetYAxisTitle(QObject::trUtf8("RPMs, %"));

	// Actualizing UI steps table
	emit MwSignalUpdateStepsTable();

	this->MwSlotSetFileName("yiff.xml");
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

void MainWindow::MwSlotExit()
{
		QCoreApplication::quit();
}

void MainWindow::MwSlotConvertorChanged()
{
	// Visualize new convertor
	double a, b;
	this->_tconv->GetADC2TempConversionFactors(&a, &b);

	QString format_str = QObject::trUtf8("Sensor: %1: T = %2*ADC");
	if (b >= 0)
	{
		format_str += QObject::trUtf8("+");
	}

	format_str += QObject::trUtf8("%3");

	this->_mwConversionStatus->setText(QString(format_str).arg(this->_tconv->GetDescription()).arg(a).arg(b));

	// Actualizing UI steps table
	emit MwSignalUpdateStepsTable();
}

void MainWindow::MwSlotCreateNewADC2Temp()
{
		NewADC2TempDialog *dlg = new NewADC2TempDialog(this);
		dlg->exec();

		SafeDelete(dlg);
}

void MainWindow::MwSlotBaseTemperatureChanged(double tempC)
{
	this->_setgen->SetBaseTemperature(tempC);
	this->_setgen->CalculateSteps(); // Updating steps values

	// Setting temperature back (it may not be equal to entered by user due ADC granularity)
	QObject::disconnect(this->ui->mw_basetemperature, SIGNAL(valueChanged(double)), this, SLOT(MwSlotBaseTemperatureChanged(double))); // Disabling signal-slot connection to avoid recursion
	this->ui->mw_basetemperature->setValue(this->_setgen->GetBaseTemperature());
	QObject::connect(this->ui->mw_basetemperature, SIGNAL(valueChanged(double)), this, SLOT(MwSlotBaseTemperatureChanged(double))); // Re-enabling it

	this->ui->mw_basetemperatureADC->setNum((int)this->_setgen->GetBaseTemperatureADC());

	// Actualizing UI steps table
	emit MwSignalUpdateStepsTable();
}

void MainWindow::MwSlotBaseRPMChanged(int RPM)
{
	this->_setgen->SetBaseRPM(RPM);

	// Actualizing UI steps table
	emit MwSignalUpdateStepsTable();
}

void MainWindow::InitializeStepsTableColumn(uint col)
{
	QTableWidgetItem* labelItem;

	// ADC Level (label)
	labelItem = new QTableWidgetItem(QObject::trUtf8("0"), QTableWidgetItem::Type);
	this->ui->mw_StepsTable->setItem(Ui::STEPS_TABLE_ROWS::ADC_LEVEL, col, labelItem);

	// Temperature (label)
	labelItem = new QTableWidgetItem(QObject::trUtf8("0.0"), QTableWidgetItem::Type);
	this->ui->mw_StepsTable->setItem(Ui::STEPS_TABLE_ROWS::TEMPERATURE, col, labelItem);

	// RPM level (label)
	labelItem = new QTableWidgetItem(QObject::trUtf8("0"), QTableWidgetItem::Type);
	this->ui->mw_StepsTable->setItem(Ui::STEPS_TABLE_ROWS::RPM_LEVEL, col, labelItem);

	// RPM percent (label)
	labelItem = new QTableWidgetItem(QObject::trUtf8("0.0%"), QTableWidgetItem::Type);
	this->ui->mw_StepsTable->setItem(Ui::STEPS_TABLE_ROWS::RPM_PERCENT, col, labelItem);

	// ADC level increase (spinbox)
	this->_ADCDeltaSpinboxes[col] = new QSpinBox(this->ui->mw_StepsTable);
	this->ui->mw_StepsTable->setCellWidget(Ui::STEPS_TABLE_ROWS::ADC_DELTA, col, this->_ADCDeltaSpinboxes[col]);

	// Temperature increase (label)
	labelItem = new QTableWidgetItem(QObject::trUtf8("0.0"), QTableWidgetItem::Type);
	this->ui->mw_StepsTable->setItem(Ui::STEPS_TABLE_ROWS::TEMPERATURE_DELTA, col, labelItem);

	// RPM increase (spinbox)
	this->_RPMDeltaSpinboxes[col] = new QSpinBox(this->ui->mw_StepsTable);
	this->ui->mw_StepsTable->setCellWidget(Ui::STEPS_TABLE_ROWS::RPM_DELTA, col, this->_RPMDeltaSpinboxes[col]);

	// RPM increase percent (label)
	labelItem = new QTableWidgetItem(QObject::trUtf8("0.0%"), QTableWidgetItem::Type);
	this->ui->mw_StepsTable->setItem(Ui::STEPS_TABLE_ROWS::RPM_DELTA_PERCENT, col, labelItem);

	if (col < ADDITIONAL_STEPS)
	{
		this->_ADCDeltaSpinboxes[col]->setReadOnly(true);
		this->_RPMDeltaSpinboxes[col]->setReadOnly(true);
	}

	// Overriding ranges
	if (BASE_STEP_INDEX == col)
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
	this->_setgen->GetStepPtr(StepNumber)->SetADCDelta(NewDelta);
	this->_setgen->CalculateSteps();

	// Actualizing UI steps table
	emit MwSignalUpdateStepsTable();
}

void MainWindow::MwSlotRPMDeltaChanged(uint StepNumber, uint NewDelta)
{
	this->_setgen->GetStepPtr(StepNumber)->SetRPMDelta(NewDelta);
	this->_setgen->CalculateSteps();

	// Actualizing UI steps table
	emit MwSignalUpdateStepsTable();
}

void MainWindow::MwSlotUpdateStepsTable()
{
	// We can do nothing is this->_setgen or this->_graph is not ready yet
	if ((NULL == this->_setgen) || (NULL == this->_graph))
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

	for (uint step = 0; step < NUMBER_OF_STEPS_TABLE_RECORDS; step++)
	{
		// Getting step
		stepItem = this->_setgen->GetStepPtr(step);

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

void MainWindow::MwSlotSetFileName(QString filename)
{
	QString windowTitle = QObject::trUtf8("");
	if (!filename.isEmpty())
	{
		windowTitle = QObject::trUtf8("%1 - ").arg(filename);
		this->_mwFileName->setText(filename);
	}
	else
	{
		this->_mwFileName->setText(QObject::trUtf8("File is not specified"));
	}

	windowTitle += QObject::trUtf8("Project \"LowBlow\" Control Tool");

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

MainWindow::~MainWindow()
{
	SafeDelete(this->_setgen);

	SafeDelete(this->_tconv);

	SafeDelete(this->_mwConversionStatus);

	SafeDelete(this->_mwFileName);

	SafeDelete(this->_settingsSaverLoader);

	SafeDelete(ui);
}
