/*  Copyright (C) 2008 e_k (e_k@users.sourceforge.net)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include <cstdlib>

#include <QApplication>
#include <QtDebug>
#include <QIcon>
#include <QFont>
#include <QKeySequence>
#include <QAction>
#include <QMainWindow>
#include <QRegularExpression>
#include <QDesktopServices>

#include <functional>

#include "qtermwidget.h"

const QFont DEFAULT_FONT = QFont("Fira Mono", 10);
const QString COLOR_SCHEME = "SolarizedLight";
const QRegularExpression FONT_CHANGE_REGEX("\\033\\]50;Font=([\\w\\s]+),?(\\d*)\\007");

void add_shortcut(QTermWidget *console,
                  QKeySequence const & key,
                  std::function<void()> cb) {
    QAction *action = new QAction(console);
    action->setShortcut(key);
    QObject::connect(action, &QAction::triggered, cb);
    console->addAction(action);
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    setenv("TERM", "xterm-256color", 1);

    QTermWidget *console = new QTermWidget(0);  // startnow = 0

    console->setTerminalSizeHint(false);
    console->setColorScheme(COLOR_SCHEME);
    console->setKeyBindings("default");
    console->setMotionAfterPasting(2);  // scroll to end
    console->setTerminalFont(DEFAULT_FONT);
    console->setBidiEnabled(false);
    console->startShellProgram();

    add_shortcut(console, QKeySequence("Ctrl+Shift+C"),
                 [=](){ console->copyClipboard(); });
    add_shortcut(console, QKeySequence("Ctrl+Shift+V"),
                 [=](){ console->pasteClipboard(); });
    add_shortcut(console, QKeySequence("Ctrl+Shift+F"),
                 [=](){ console->toggleShowSearchBar(); });
    add_shortcut(console, QKeySequence(Qt::CTRL, Qt::Key_Minus),
                 [=](){ console->zoomOut(); });
    add_shortcut(console, QKeySequence(Qt::CTRL, Qt::Key_Plus),
                 [=](){ console->zoomIn(); });

    QObject::connect(console, &QTermWidget::titleChanged, [=](){});
    QObject::connect(console, &QTermWidget::receivedData, [=](QString const & text) {
        auto match = FONT_CHANGE_REGEX.match(text);
        if (match.hasMatch()) {
            bool ok = true;
            QFont new_font = DEFAULT_FONT;
            new_font.setFamily(match.captured(1));
            if (match.captured(2).size() > 0)
                new_font.setPointSize(match.captured(2).toInt(&ok));
            if (ok) {
                qDebug() << "Setting font to " << new_font;
                console->setTerminalFont(new_font);
            } else {
                qDebug() << "Cannot parse font: " << match.captured(0);
            }
        }
    });
    QObject::connect(console, &QTermWidget::urlActivated, [](const QUrl& url, bool) {
        if (QApplication::keyboardModifiers() & (Qt::ControlModifier | Qt::MetaModifier))
            QDesktopServices::openUrl(url);
    });
    QObject::connect(console, &QTermWidget::finished, [&](){ app.quit(); });

    console->show();

    return app.exec();
}
