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
#include <QComboBox>
#include <QCompleter>
#include <QDateTime>
#include <QDateTimeEdit>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QFileSystemModel>
#include <QFormLayout>
#include <QGuiApplication>
#include <QKeySequenceEdit>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScreen>
#include <QScrollArea>
#include <QSettings>
#include <QSpinBox>
#include <QStringListModel>
#include <QTabWidget>
#include <QVector3D>

#include <array>

#include "InputManager.hpp"

class SettingsWidget : public QTabWidget
{
	Q_OBJECT
  public:
	explicit SettingsWidget(QWidget* parent = nullptr);
	~SettingsWidget() = default;

  protected:
	void addGroup(QString const& name, QString const& label);
	void insertGroup(QString const& name, QString const& label, int index);
	void editGroup(QString const& name);
	void addBoolSetting(QString const& name, bool defaultVal,
	                    QString const& label);
	void addUIntSetting(QString const& name, unsigned int defaultVal,
	                    QString const& label, unsigned int minVal = 0,
	                    unsigned int maxVal = 99);
	void addIntSetting(QString const& name, int defaultVal,
	                   QString const& label, int minVal = 0, int maxVal = 99);
	void addDoubleSetting(QString const& name, double defaultVal,
	                      QString const& label, double minVal = 0,
	                      double maxVal = 99, unsigned int decimals = 3);
	void addStringSetting(QString const& name, QString const& defaultVal,
	                      QString const& label);
	void addFilePathSetting(QString const& name, QString const& defaultVal,
	                        QString const& label);
	void addDirPathSetting(QString const& name, QString const& defaultVal,
	                       QString const& label);
	void addVector3DSetting(QString const& name, QVector3D const& defaultVal,
	                        QString const& label,
	                        std::array<QString, 3> componentLabels,
	                        float minVal = 0.f, float maxVal = 1.f);
	void addColorSetting(QString const& name, QColor const& defaultVal,
	                     QString const& label);
	void addDateTimeSetting(QString const& name, QDateTime const& defaultVal,
	                        QString const& label);
	void addKeySequenceSetting(QString const& name,
	                           QKeySequence const& defaultVal,
	                           QString const& label);
	void addLanguageSetting(QString const& name = "language",
	                        QString const& defaultVal
	                        = QLocale::system().name(),
	                        QString const& label
	                        = tr("Language (needs restart)"));
	void addScreenNameSetting(QString const& name       = "screenname",
	                          QString const& defaultVal = "",
	                          QString const& label      = tr("Screen"));

  private:
	QFormLayout* currentForm = nullptr;

	QSettings settings;
	QString currentGroup;
	QStringList orderedGroups;

	template <typename T>
	inline void updateValue(QString const& fullName, T newValue);
};

template <typename T>
void SettingsWidget::updateValue(QString const& fullName, T newValue)
{
	settings.setValue(fullName, newValue);
}

class ScreenSelector : public QDialog
{
	static QString& retValue();

  public:
	static QString selectScreen(QWidget* parent = nullptr);

  private:
	unsigned int w = 700;
	unsigned int h;
	QRect desktopGeometry = QGuiApplication::screens()[0]->virtualGeometry();

	explicit ScreenSelector(QWidget* parent = nullptr);

	// list of pairs of name and geometry
	QList<QPair<QString, QRect>> getScreens() const;
};

#endif // SETTINGSWIDGET_H
