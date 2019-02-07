/*
    Copyright (C) 2019 Florian Cabot <florian.cabot@epfl.ch>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "SettingsWidget.hpp"

SettingsWidget::SettingsWidget(QWidget* parent)
    : QTabWidget(parent)
{
	addGroup("window", tr("Window"));
	addUIntSetting("width", 1500, tr("Window Width"), 0, 10000);
	addUIntSetting("height", 800, tr("Window Height"), 0, 10000);
	addBoolSetting("fullscreen", false, tr("Window Fullscreen"));

	addGroup("vr", tr("Virtual Reality"));
	addBoolSetting("enabled", true, tr("Enable VR"));

	addGroup("debugcamera", tr("Debug Camera"));
	addBoolSetting("enabled", false, tr("Enable Debug Camera"));
	addBoolSetting("followhmd", false, tr("Follow HMD Movement"));
	addBoolSetting("debuginheadset", false, tr("Show Debug In HMD"));

	addGroup("data", tr("Data"));
	addFilePathSetting("gazfile", QString(""), tr("Gaz File"));
	addFilePathSetting("starsfile", QString(""), tr("Stars File"));
	addFilePathSetting("darkmatterfile", QString(""), tr("Dark Matter File"));
	addBoolSetting("loaddarkmatter", true, tr("Load Dark Matter"));
	addColorSetting("gazcolor", QColor::fromRgbF(0.427450f, 0.592156f, 0.709803f), tr("Gaz Color"));
	addColorSetting("starscolor", QColor::fromRgbF(1.f, 1.f, 0.f), tr("Stars Color"));
	addColorSetting("darkmattercolor", QColor::fromRgbF(1.f, 0.5f, 0.15f), tr("Dark Matter Color"));

	addGroup("misc", tr("Miscellaneous"));
	addBoolSetting("showcube", false, tr("Show Cube"));
	addColorSetting("cubecolor", QColor(255, 255, 255), tr("Cube Color"));
	addVector3DSetting("focuspoint", QVector3D(), tr("Focus Point"), {{tr("x"), tr("y"), tr("z")}}, -1000, 1000);
	// focuspoint=-0.352592, -0.062213, 0.144314
	addUIntSetting("maxvramusagemb", 500, tr("Max VRAM Usage (in Mb)"), 0, 1000000);
}

void SettingsWidget::addGroup(QString const& name, QString const& label)
{
	currentGroup = name;

	QWidget* newTab = new QWidget(this);
	QTabWidget::addTab(newTab, label);
	currentForm = new QFormLayout(newTab);
}

void SettingsWidget::addBoolSetting(QString const& name, bool defaultVal,
                                    QString const& label)
{
	QString fullName(currentGroup + '/' + name);

	if(!settings.contains(fullName))
		settings.setValue(fullName, defaultVal);

	QCheckBox* checkBox = new QCheckBox(this);
	checkBox->setCheckState(settings.value(fullName).toBool() ? Qt::Checked
	                                                          : Qt::Unchecked);
	connect(checkBox, &QCheckBox::stateChanged, this, [this, fullName](int s) {
		updateValue(fullName, s != Qt::Unchecked);
	});

	currentForm->addRow(label + " :", checkBox);
}

void SettingsWidget::addUIntSetting(QString const& name,
                                    unsigned int defaultVal,
                                    QString const& label, unsigned int minVal,
                                    unsigned int maxVal)
{
	QString fullName(currentGroup + '/' + name);

	if(!settings.contains(fullName))
		settings.setValue(fullName, defaultVal);

	QSpinBox* sbox = new QSpinBox(this);
	sbox->setRange(minVal, maxVal);
	sbox->setValue(settings.value(fullName).toUInt());

	connect(sbox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
	        this, [this, fullName](int v) { updateValue(fullName, v); });

	currentForm->addRow(label + " :", sbox);
}

void SettingsWidget::addStringSetting(QString const& name,
                                      QString const& defaultVal,
                                      QString const& label)
{
	QString fullName(currentGroup + '/' + name);

	if(!settings.contains(fullName))
		settings.setValue(fullName, defaultVal);

	QLineEdit* lineEdit = new QLineEdit(this);
	lineEdit->setText(settings.value(fullName).toString());

	connect(lineEdit, &QLineEdit::textChanged, this,
	        [this, fullName](QString const& t) { updateValue(fullName, t); });

	currentForm->addRow(label + " :", lineEdit);
}

void SettingsWidget::addFilePathSetting(QString const& name,
                                        QString const& defaultVal,
                                        QString const& label)
{
	QString fullName(currentGroup + '/' + name);

	if(!settings.contains(fullName))
		settings.setValue(fullName, defaultVal);

	QLineEdit* lineEdit = new QLineEdit(this);
	lineEdit->setText(settings.value(fullName).toString());

	QFileSystemModel* dirModel = new QFileSystemModel(this);
	dirModel->setRootPath(QDir::currentPath());
	QCompleter* completer = new QCompleter(dirModel, this);
	completer->setCaseSensitivity(Qt::CaseInsensitive);
	completer->setCompletionMode(QCompleter::PopupCompletion);
	lineEdit->setCompleter(completer);

	QPushButton* browsePb = new QPushButton(this);
	browsePb->setText(tr("..."));
	connect(browsePb, &QPushButton::clicked, this,
	        [this, label, lineEdit](bool) {
		        QString result(QFileDialog::getOpenFileName(this, label,
		                                                    lineEdit->text()));
		        if(result != "")
			        lineEdit->setText(result);
		    });

	QWidget* w          = new QWidget(this);
	QHBoxLayout* layout = new QHBoxLayout(w);
	layout->addWidget(lineEdit);
	layout->addWidget(browsePb);

	connect(lineEdit, &QLineEdit::textChanged, this,
	        [this, fullName](QString const& t) { updateValue(fullName, t); });

	currentForm->addRow(label + " :", w);
}

void SettingsWidget::addVector3DSetting(QString const& name,
                                        QVector3D const& defaultVal,
                                        QString const& label,
                                        std::array<QString, 3> componentLabels,
                                        float minVal, float maxVal)
{
	QString fullName(currentGroup + '/' + name);

	if(!settings.contains(fullName))
		settings.setValue(fullName, defaultVal);

	QVector3D stored(settings.value(fullName).value<QVector3D>());

	QWidget* w          = new QWidget(this);
	QHBoxLayout* layout = new QHBoxLayout(w);
	std::array<QDoubleSpinBox*, 3> sboxes;
	for(unsigned int i(0); i < 3; ++i)
	{
		sboxes[i] = new QDoubleSpinBox(this);
		sboxes[i]->setRange(minVal, maxVal);
		sboxes[i]->setSingleStep((maxVal - minVal) / 100.f);
		sboxes[i]->setValue(stored[i]);
		layout->addWidget(new QLabel(componentLabels[i] + " :", this));
		layout->addWidget(sboxes[i]);
	}
	for(unsigned int i(0); i < 3; ++i)
		connect(sboxes[i], static_cast<void (QDoubleSpinBox::*)(double)>(
		                       &QDoubleSpinBox::valueChanged),
		        this, [this, fullName, sboxes](double) {
			        updateValue(fullName, QVector3D(sboxes[0]->value(),
			                                        sboxes[1]->value(),
			                                        sboxes[2]->value()));
			    });

	currentForm->addRow(label + " :", w);
}

void SettingsWidget::addColorSetting(QString const& name,
                                     QColor const& defaultVal,
                                     QString const& label)
{
	QString fullName(currentGroup + '/' + name);

	if(!settings.contains(fullName))
		settings.setValue(fullName, defaultVal);

	QColor stored(settings.value(fullName).value<QColor>());

	QPushButton* button = new QPushButton(this);

	button->setStyleSheet("QPushButton{ \
    background-color: " + stored.name()
	                      + ";        \
    border-style: inset;                     \
    }");

	connect(button, &QPushButton::clicked, this,
	        [this, fullName, button, label](bool) {
		        QColor result(QColorDialog::getColor(
		            settings.value(fullName).value<QColor>(), this, label));
		        if(!result.isValid())
			        return;

		        button->setStyleSheet("QPushButton{ \
    background-color: " + result.name()
		                              + ";        \
    border-style: inset;                     \
    }");
		        updateValue(fullName, result);

		    });

	currentForm->addRow(label + " :", button);
}
