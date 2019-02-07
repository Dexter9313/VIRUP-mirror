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

#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QCheckBox>
#include <QColorDialog>
#include <QCompleter>
#include <QFileDialog>
#include <QFileSystemModel>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSettings>
#include <QSpinBox>
#include <QStringListModel>
#include <QTabWidget>
#include <QVector3D>

#include <array>

class SettingsWidget : public QTabWidget
{
	Q_OBJECT
  public:
	SettingsWidget(QWidget* parent = nullptr);
	~SettingsWidget()              = default;

  private:
	void addGroup(QString const& name, QString const& label);
	void addBoolSetting(QString const& name, bool defaultVal,
	                    QString const& label);
	void addUIntSetting(QString const& name, unsigned int defaultVal,
	                    QString const& label, unsigned int minVal = 0,
	                    unsigned int maxVal = 99);
	void addStringSetting(QString const& name, QString const& defaultVal,
	                      QString const& label);
	void addFilePathSetting(QString const& name, QString const& defaultVal,
	                        QString const& label);
	void addVector3DSetting(QString const& name, QVector3D const& defaultVal,
	                        QString const& label,
	                        std::array<QString, 3> componentLabels,
	                        float minVal = 0.f, float maxVal = 1.f);
	void addColorSetting(QString const& name, QColor const& defaultVal,
	                     QString const& label);

	QFormLayout* currentForm = nullptr;

	QSettings settings;
	QString currentGroup;

	template <typename T>
	inline void updateValue(QString const& fullName, T newValue);
};

template <typename T>
void SettingsWidget::updateValue(QString const& fullName, T newValue)
{
	settings.setValue(fullName, newValue);
}

#endif // SETTINGSWIDGET_H
