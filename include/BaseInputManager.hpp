/*
    Copyright (C) 2019 Florian Cabot <florian.cabot@hotmail.fr>

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

#ifndef BASEINPUTMANAGER_HPP
#define BASEINPUTMANAGER_HPP

#include <QHash>
#include <QKeySequence>
#include <QObject>
#include <QSettings>
#include <QtDebug>

class BaseInputManager : public QObject
{
	Q_OBJECT
  public:
	struct Action
	{
		// string to identify the action
		QString id;
		// string to show the user
		QString name;
	};
	BaseInputManager();
	std::vector<QString> const& getOrderedEngineKeys() const
	{
		return orderedEngineKeys;
	};
	std::vector<QString> const& getOrderedProgramKeys() const
	{
		return orderedProgramKeys;
	};
	QHash<QString, Action> const& getMapping() const { return mapping; };
	Action operator[](QKeySequence const& key) const;
	virtual ~BaseInputManager(){};

  protected:
	// put all addActions in constructor for launcher to see them !
	void addAction(QKeySequence const& defaultKey, Action action);

  private:
	void addAction(QKeySequence const& defaultKey, Action action, bool engine);

	// QKeySequence is a key (Qt::Key_A for example)
	QHash<QString, Action> mapping;

	// keep declaration order for interface
	std::vector<QString> orderedEngineKeys;
	std::vector<QString> orderedProgramKeys;
};

#endif // BASEINPUTMANAGER_HPP
