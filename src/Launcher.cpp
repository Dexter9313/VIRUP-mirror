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

#include "Launcher.hpp"

Launcher::Launcher()
{
	this->setWindowTitle(QString(PROJECT_NAME) + " Launcher");

	QVBoxLayout* mainLayout = new QVBoxLayout(this);

	// SETTINGS TAB WIDGET
	SettingsWidget* settingsWidget = new SettingsWidget(this);
	mainLayout->addWidget(settingsWidget);

	// LAUNCH AND QUIT BUTTONS
	QWidget* w = new QWidget(this);
	mainLayout->addWidget(w);
	QHBoxLayout* l = new QHBoxLayout(w);
	QPushButton* pbl = new QPushButton(this);
	l->addWidget(pbl);
	pbl->setText(tr("LAUNCH"));
	connect(pbl, SIGNAL(pressed()), this, SLOT(accept()));
	QPushButton* pbq = new QPushButton(this);
	l->addWidget(pbq);
	pbq->setText(tr("QUIT"));
	connect(pbq, SIGNAL(pressed()), this, SLOT(reject()));
}
