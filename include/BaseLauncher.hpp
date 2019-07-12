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

#ifndef BASELAUNCHER_H
#define BASELAUNCHER_H

#include <QDebug>
#include <QDialog>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

#include "SettingsWidget.hpp"

/**
 * @brief Base class for the program launcher
 *
 * By default, will display a @ref SettingsWidget with a LAUNCH and QUIT button.
 * Inherit from it to customize the launcher, calling your class "Launcher".
 */
class BaseLauncher : public QDialog
{
	Q_OBJECT
  public:
	BaseLauncher() = default;
	/**
	 * @brief Main Launcher initialization. Override to customize.
	 */
	virtual void init();

  protected:
	/**
	 * @brief Creates a new @ref SettingsWidget.
	 *
	 * @warning Qt will take ownership of the returned pointer.
	 *
	 * If you inherited from @ref SettingsWidget to add custom settings to it
	 * (let's say you called your class MySettingsWidget), override this method
	 * to return <code>new MySettingsWidget(this)</code>.
	 */
	virtual SettingsWidget* newSettingsWidget();
	/**
	 * @brief Top level layout of the launcher.
	 *
	 * Contains first the @ref SettingsWidget, then a QHBoxLayout with both
	 * LAUNCH and QUIT buttons. Will get initialized when @ref init() is called.
	 * You can then add your own widgets to it if you want.
	 */
	QVBoxLayout* mainLayout = nullptr;

  protected slots:
	void resetSettings();

  private:
	SettingsWidget* settingsWidget;
};

#endif // BASELAUNCHER_H
