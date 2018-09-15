#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::AboutDialog)
{
	ui->setupUi(this);

	// Close button
	QObject::connect(ui->adCloseButton, SIGNAL(clicked()), this, SLOT(reject()));

	// Setting about text from resources
	ui->adText->setText(QObject::tr(
		"<h1>Project \"LowBlow\"</h1>"
		"<h3>Advanced cooler controller</h3>"
		"<h2>Temperature-to-RPM EEPROM lookup table generation tool</h2>"
		"By White Fossa aka Artyom Vetrov, 2018.<br>"
		"<a href=\"mailto:whitefossa@gmail.com\">whitefossa@gmail.com</a><br><br>"
		"This contorl tool, firmware and hardware desing are being distributed under terms of GPLv3 license.<br>"
		"<a href=\"https://github.com/WhiteFossa/LowBlow\">https://github.com/WhiteFossa/LowBlow</a><br><br>"
		"Contains MIT-licensed Intel HEX format parser, written by Kimmo Kulovesi.<br>"
		"<a href=\"https://github.com/arkku/ihex\">https://github.com/arkku/ihex</a>"
							));
}

AboutDialog::~AboutDialog()
{
	delete ui;
}
