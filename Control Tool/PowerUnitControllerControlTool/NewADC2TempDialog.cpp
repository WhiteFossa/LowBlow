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

#include "NewADC2TempDialog.hpp"
#include "ui_NewADC2TempDialog.h"

NewADC2TempDialog::NewADC2TempDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::NewADC2TempDialog)
{
  ui->setupUi(this);

  /* No path specified */
  this->filepath = "";
  this->SlotCheckRequirements();

  QObject::connect(this->ui->Cancel, SIGNAL(clicked()), this, SLOT(reject()));
  QObject::connect(this->ui->Create, SIGNAL(clicked()), this, SLOT(SlotSaveSettings()));
  QObject::connect(this->ui->tbSelectFile, SIGNAL(clicked()), this, SLOT(SlotSelectFile()));
  QObject::connect(this->ui->lnDescription, SIGNAL(textChanged(QString)), this, SLOT(SlotCheckRequirements()));
}

NewADC2TempDialog::~NewADC2TempDialog()
{
  delete ui;
}

void NewADC2TempDialog::changeEvent(QEvent *e)
{
  QDialog::changeEvent(e);
  switch (e->type()) {
  case QEvent::LanguageChange:
    ui->retranslateUi(this);
    break;
  default:
    break;
  }
}

/**
 * @brief Call it to specify target file (this->filepath)
 */
void NewADC2TempDialog::SlotSelectFile()
{
  this->filepath = QFileDialog::getSaveFileName(this, QObject::trUtf8("Save ADC to Temperature settings"));
  this->ui->lnFilePath->setText(this->filepath);

  this->SlotCheckRequirements();
}

/**
 * @brief Call it to save settings into new this->filepath file
 */
void NewADC2TempDialog::SlotSaveSettings()
{
  Interfaces::IAdcTemperatureConvertor *conv = new AdcTemperatureConvertor();
  conv->SetDescription(this->ui->lnDescription->text());
  conv->SetADC2TempConversionFactors(this->ui->sbMul->value(), this->ui->sbAdd->value());

  conv->SaveSettings(this->filepath);

  SafeDelete(conv);

  this->accept();
}

/**
 * @brief Checks if we have filepath and description specified. If yes - enables Create button, otherwise - disables it.
 */
void NewADC2TempDialog::SlotCheckRequirements()
{
  if ("" == this->filepath || "" == this->ui->lnDescription->text())
  {
    this->ui->Create->setEnabled(false);
  }
  else
  {
    this->ui->Create->setEnabled(true);
  }
}
