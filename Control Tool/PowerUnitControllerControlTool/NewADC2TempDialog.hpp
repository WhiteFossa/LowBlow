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

#ifndef NEWADC2TEMPDIALOG_HPP
#define NEWADC2TEMPDIALOG_HPP

/**
  * New ADC to Temperature settings dialog
  */

#include <QtWidgets/QDialog>
#include <QFileDialog>
#include <QDebug>
#include <Auxiliary.hpp>
#include <Interfaces/IAdcTemperatureConvertor.hpp>
#include <Implementations/AdcTemperatureConvertor.hpp>


namespace Ui {
class NewADC2TempDialog;
}

class NewADC2TempDialog : public QDialog
{
  Q_OBJECT

  public:
    explicit NewADC2TempDialog(QWidget *parent = 0);
    ~NewADC2TempDialog();

  /* Public slots */
  public slots:

    /**
     * @brief Call it to specify target file (this->filepath)
     */
    void SlotSelectFile();

    /**
     * @brief Call it to save settings into new this->filepath file
     */
    void SlotSaveSettings();

    /**
     * @brief Checks if we have filepath and description specified. If yes - enables Create button, otherwise - disables it.
     */
    void SlotCheckRequirements();

  protected:
    void changeEvent(QEvent *e);

  private:
    Ui::NewADC2TempDialog *ui;

    /**
     * @brief Full path to file, where settings will be saved
     */
    QString filepath;
};

#endif // NEWADC2TEMPDIALOG_HPP
