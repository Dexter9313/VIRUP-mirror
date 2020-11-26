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
	qDebug() << QString("Config file :") + QSettings().fileName();
	addGroup("window", tr("Window"));
	addUIntSetting("width", 1500, tr("Window Width"), 0, 17000);
	addUIntSetting("height", 800, tr("Window Height"), 0, 17000);
	addBoolSetting("fullscreen", false, tr("Window Fullscreen"));
	addBoolSetting("forcerenderresolution", false,
	               tr("Force Rendering Resolution"));
	addUIntSetting("forcewidth", 1500, tr("Forced Rendering Width"), 0, 17000);
	addUIntSetting("forceheight", 800, tr("Forced Rendering Height"), 0, 17000);
	addLanguageSetting();
	addDirPathSetting(
	    "viddir",
	    QFileInfo(QSettings().fileName()).absoluteDir().absolutePath()
	        + "/systems/",
	    tr("Video Frames Output Directory"));

	addGroup("graphics", tr("Graphics"));
	addUIntSetting("antialiasing", 0, tr("Anti-aliasing"), 0, 3);
	addUIntSetting("shadowsquality", 1, tr("Shadows Quality"), 1, 5);
	addUIntSetting("smoothshadows", 0, tr("Shadow Smoothing Quality"), 0, 5);
	addBoolSetting("dithering", true, tr("Enable Dithering"));
	addBoolSetting("bloom", true, tr("Bloom"));
	addDoubleSetting("vfov", 0.0, tr("Vertical field of view (0=auto)"), 0.0,
	                 360.0);
	addDoubleSetting("hfov", 0.0, tr("Horizontal field of view (0=auto)"), 0.0,
	                 360.0);

	InputManager inputManager;
	addGroup("controls", tr("Controls"));
	currentForm->addRow(tr("ENGINE"), new QWidget());
	for(auto const& key : inputManager.getOrderedEngineKeys())
	{
		addKeySequenceSetting(inputManager[key].id, key,
		                      inputManager[key].name);
	}
	if(!inputManager.getOrderedProgramKeys().empty())
	{
		currentForm->addRow(" ", new QWidget());
		currentForm->addRow(PROJECT_NAME, new QWidget());
		for(auto const& key : inputManager.getOrderedProgramKeys())
		{
			addKeySequenceSetting(inputManager[key].id, key,
			                      inputManager[key].name);
		}
	}

	addGroup("vr", tr("Virtual Reality"));
	addBoolSetting("enabled", true, tr("Enable VR"));
	addBoolSetting("mode", true, tr("OpenVR (Stereo Beamer Mode if no)"));
	addBoolSetting(
	    "thirdrender", false,
	    tr("Force 2D render on screen\n(will decrease performance !)"));
	addDoubleSetting("stereomultiplier", 1.0,
	                 tr("Stereo multiplier (if applicable)"), 0.0, 1000.0);

	addGroup("network", tr("Network"));
	addBoolSetting("server", true, tr("Server"));
	addStringSetting("ip", "127.0.0.1", tr("IP address of server (if client)"));
	addUIntSetting("port", 5000, tr("IP port"), 1025, 49999);
	addDoubleSetting("angleshift", 0.0,
	                 tr("Horizontal angle shift compared to server (degrees)"),
	                 -180.0, 180.0);
	addDoubleSetting("vangleshift", 0.0,
	                 tr("Vertical angle shift compared to server (degrees)"),
	                 -180.0, 180.0);

	addGroup("scripting", tr("Scripting"));
	addDirPathSetting(
	    "rootdir",
	    QFileInfo(settings.fileName()).absoluteDir().absolutePath()
	        + "/scripts",
	    tr("Scripts Root Directory"));

	addGroup("debugcamera", tr("Debug Camera"));
	addBoolSetting("enabled", false, tr("Enable Debug Camera"));
	addBoolSetting("followhmd", false, tr("Follow HMD Movement"));
	addBoolSetting("debuginheadset", false, tr("Show Debug In HMD"));
}

void SettingsWidget::addGroup(QString const& name, QString const& label)
{
	currentGroup = name;

	auto newScrollArea = new QScrollArea(this);
	auto newTab        = new QWidget(this);
	currentForm        = new QFormLayout(newTab);
	currentForm->setSizeConstraint(QLayout::SetMinimumSize);

	newScrollArea->setWidget(newTab);

	QTabWidget::addTab(newScrollArea, label);

	orderedGroups.append(name);
}

void SettingsWidget::insertGroup(QString const& name, QString const& label,
                                 int index)
{
	currentGroup = name;

	auto newScrollArea = new QScrollArea(this);
	auto newTab        = new QWidget(this);
	currentForm        = new QFormLayout(newTab);
	currentForm->setSizeConstraint(QLayout::SetMinimumSize);

	newScrollArea->setWidget(newTab);

	QTabWidget::insertTab(index, newScrollArea, label);

	orderedGroups.insert(index, name);
}

void SettingsWidget::editGroup(QString const& name)
{
	currentGroup = name;

	unsigned int index(orderedGroups.indexOf(name));

	currentForm = dynamic_cast<QFormLayout*>(
	    dynamic_cast<QScrollArea*>(QTabWidget::widget(index))
	        ->widget()
	        ->layout());

	currentForm->insertRow(0, tr("ENGINE"), new QWidget());
	currentForm->addRow(" ", new QWidget());
	currentForm->addRow(PROJECT_NAME, new QWidget());
}

void SettingsWidget::addBoolSetting(QString const& name, bool defaultVal,
                                    QString const& label)
{
	QString fullName(currentGroup + '/' + name);

	if(!settings.contains(fullName))
	{
		settings.setValue(fullName, defaultVal);
	}

	auto checkBox = new QCheckBox(this);
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
	{
		settings.setValue(fullName, defaultVal);
	}

	auto sbox = new QSpinBox(this);
	sbox->setRange(minVal, maxVal);
	sbox->setValue(settings.value(fullName).toUInt());

	connect(sbox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
	        this,
	        [this, fullName](unsigned int v) { updateValue(fullName, v); });

	currentForm->addRow(label + " :", sbox);
}

void SettingsWidget::addIntSetting(QString const& name, int defaultVal,
                                   QString const& label, int minVal, int maxVal)
{
	QString fullName(currentGroup + '/' + name);

	if(!settings.contains(fullName))
	{
		settings.setValue(fullName, defaultVal);
	}

	auto sbox = new QSpinBox(this);
	sbox->setRange(minVal, maxVal);
	sbox->setValue(settings.value(fullName).toInt());

	connect(sbox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
	        this, [this, fullName](int v) { updateValue(fullName, v); });

	currentForm->addRow(label + " :", sbox);
}

void SettingsWidget::addDoubleSetting(QString const& name, double defaultVal,
                                      QString const& label, double minVal,
                                      double maxVal, unsigned int decimals)
{
	QString fullName(currentGroup + '/' + name);

	if(!settings.contains(fullName))
	{
		settings.setValue(fullName, defaultVal);
	}

	auto sbox = new QDoubleSpinBox(this);
	sbox->setRange(minVal, maxVal);
	sbox->setDecimals(decimals);
	sbox->setValue(settings.value(fullName).toDouble());

	connect(sbox,
	        static_cast<void (QDoubleSpinBox::*)(double)>(
	            &QDoubleSpinBox::valueChanged),
	        this, [this, fullName](double v) { updateValue(fullName, v); });

	currentForm->addRow(label + " :", sbox);
}

void SettingsWidget::addStringSetting(QString const& name,
                                      QString const& defaultVal,
                                      QString const& label)
{
	QString fullName(currentGroup + '/' + name);

	if(!settings.contains(fullName))
	{
		settings.setValue(fullName, defaultVal);
	}

	auto lineEdit = new QLineEdit(this);
	lineEdit->setText(settings.value(fullName).toString());
	lineEdit->setMinimumWidth(400);

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
	{
		settings.setValue(fullName, defaultVal);
	}

	auto lineEdit = new QLineEdit(this);
	lineEdit->setText(settings.value(fullName).toString());
	lineEdit->setMinimumWidth(400);

	auto dirModel = new QFileSystemModel(this);
	dirModel->setRootPath(QDir::currentPath());
	auto completer = new QCompleter(dirModel, this);
	completer->setCaseSensitivity(Qt::CaseInsensitive);
	completer->setCompletionMode(QCompleter::PopupCompletion);
	lineEdit->setCompleter(completer);

	auto browsePb = new QPushButton(this);
	browsePb->setText("...");
	connect(browsePb, &QPushButton::clicked, this,
	        [this, label, lineEdit](bool) {
		        QString result(QFileDialog::getOpenFileName(this, label,
		                                                    lineEdit->text()));
		        if(result != "")
		        {
			        lineEdit->setText(result);
		        }
	        });

	auto w      = new QWidget(this);
	auto layout = new QHBoxLayout(w);
	layout->addWidget(lineEdit);
	layout->addWidget(browsePb);

	connect(lineEdit, &QLineEdit::textChanged, this,
	        [this, fullName](QString const& t) { updateValue(fullName, t); });

	currentForm->addRow(label + " :", w);
}

void SettingsWidget::addDirPathSetting(QString const& name,
                                       QString const& defaultVal,
                                       QString const& label)
{
	QString fullName(currentGroup + '/' + name);

	if(!settings.contains(fullName))
	{
		settings.setValue(fullName, defaultVal);
	}

	auto lineEdit = new QLineEdit(this);
	lineEdit->setText(settings.value(fullName).toString());
	lineEdit->setMinimumWidth(400);

	auto dirModel = new QFileSystemModel(this);
	dirModel->setRootPath(QDir::currentPath());
	auto completer = new QCompleter(dirModel, this);
	completer->setCaseSensitivity(Qt::CaseInsensitive);
	completer->setCompletionMode(QCompleter::PopupCompletion);
	lineEdit->setCompleter(completer);

	auto browsePb = new QPushButton(this);
	browsePb->setText("...");
	connect(browsePb, &QPushButton::clicked, this,
	        [this, label, lineEdit](bool) {
		        QString result(QFileDialog::getExistingDirectory(
		            this, label, lineEdit->text()));
		        if(result != "")
		        {
			        lineEdit->setText(result);
		        }
	        });

	auto w      = new QWidget(this);
	auto layout = new QHBoxLayout(w);
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
	{
		settings.setValue(fullName, defaultVal);
	}

	QVector3D stored(settings.value(fullName).value<QVector3D>());

	auto w                                = new QWidget(this);
	auto layout                           = new QHBoxLayout(w);
	std::array<QDoubleSpinBox*, 3> sboxes = {{nullptr, nullptr, nullptr}};
	unsigned int i(0);
	for(auto& sbox : sboxes)
	{
		sbox = new QDoubleSpinBox(this);
		sbox->setRange(minVal, maxVal);
		sbox->setSingleStep((maxVal - minVal) / 100.f);
		sbox->setValue(stored[i]);
		layout->addWidget(new QLabel(componentLabels.at(i) + " :", this));
		layout->addWidget(sbox);
		++i;
	}
	for(auto sbox : sboxes)
	{
		connect(sbox,
		        static_cast<void (QDoubleSpinBox::*)(double)>(
		            &QDoubleSpinBox::valueChanged),
		        this, [this, fullName, sboxes](double) {
			        updateValue(fullName, QVector3D(sboxes[0]->value(),
			                                        sboxes[1]->value(),
			                                        sboxes[2]->value()));
		        });
	}

	currentForm->addRow(label + " :", w);
}

void SettingsWidget::addColorSetting(QString const& name,
                                     QColor const& defaultVal,
                                     QString const& label)
{
	QString fullName(currentGroup + '/' + name);

	if(!settings.contains(fullName))
	{
		settings.setValue(fullName, defaultVal);
	}

	QColor stored(settings.value(fullName).value<QColor>());

	auto button = new QPushButton(this);

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
		        {
			        return;
		        }

		        button->setStyleSheet("QPushButton{ \
    background-color: " + result.name()
		                              + ";        \
    border-style: inset;                     \
    }");
		        updateValue(fullName, result);
	        });

	currentForm->addRow(label + " :", button);
}

void SettingsWidget::addDateTimeSetting(QString const& name,
                                        QDateTime const& defaultVal,
                                        QString const& label)
{
	QString fullName(currentGroup + '/' + name);

	if(!settings.contains(fullName))
	{
		settings.setValue(fullName, defaultVal);
	}

	QDateTime stored(settings.value(fullName).value<QDateTime>().toTimeSpec(
	    Qt::OffsetFromUTC));

	auto w      = new QWidget(this);
	auto layout = new QHBoxLayout(w);

	auto dtEdit = new QDateTimeEdit(stored, this);
	dtEdit->setCalendarPopup(true);
	dtEdit->setDisplayFormat("dd.MM.yyyy hh:mm:ss");

	connect(dtEdit, &QDateTimeEdit::dateTimeChanged, this,
	        [this, fullName](QDateTime dt) { updateValue(fullName, dt); });

	auto now = new QPushButton(tr("Now"), this);
	connect(now, &QPushButton::clicked, this, [dtEdit]() {
		dtEdit->setDateTime(QDateTime::currentDateTimeUtc());
	});

	layout->addWidget(dtEdit);
	layout->addWidget(now);
	currentForm->addRow(label + " :", w);
}

void SettingsWidget::addKeySequenceSetting(QString const& name,
                                           QKeySequence const& defaultVal,
                                           QString const& label)
{
	QString fullName(currentGroup + '/' + name);

	if(!settings.contains(fullName))
	{
		settings.setValue(fullName,
		                  defaultVal.toString(QKeySequence::PortableText));
	}

	auto keyseqEdit
	    = new QKeySequenceEdit(QKeySequence(settings.value(fullName).toString(),
	                                        QKeySequence::PortableText),
	                           this);
	keyseqEdit->setMinimumWidth(100);

	connect(
	    keyseqEdit, &QKeySequenceEdit::keySequenceChanged, this,
	    [this, fullName](QKeySequence const& t) { updateValue(fullName, t); });

	currentForm->addRow(label + " :", keyseqEdit);
}

void SettingsWidget::addLanguageSetting(QString const& name,
                                        QString const& defaultVal,
                                        QString const& label)
{
	QString fullName(currentGroup + '/' + name);
	if(!settings.contains(fullName))
	{
		settings.setValue(fullName, defaultVal);
	}

	auto stored = settings.value(fullName).toString();

	QList<QPair<QString, QString>> available(
	    {{"en", QLocale("en").nativeLanguageName()}});
	for(auto const& file :
	    QDir("data/translations").entryList({"HydrogenVR_*.qm"}, QDir::Files))
	{
		QString name(file.split('_')[1].split(".")[0]);
		QPair<QString, QString> elem(
		    {name, QLocale(name).nativeLanguageName()});
		elem.second[0] = elem.second[0].toUpper();
		available.push_back(elem);
	}

	auto comboBox = new QComboBox(this);
	for(auto const& pair : available)
	{
		comboBox->addItem(pair.second, pair.first);

		if(QLocale(stored).language() == QLocale(pair.first).language())
		{
			comboBox->setCurrentText(pair.second);
		}
	}

	void (QComboBox::*indexChangedSignal)(int)
	    = &QComboBox::currentIndexChanged;
	connect(comboBox, indexChangedSignal, this,
	        [this, fullName, available](int index) {
		        updateValue(fullName, available[index].first);
	        });

	currentForm->addRow(label + " :", comboBox);
}
