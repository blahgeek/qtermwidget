#include <cstdlib>

#include <QApplication>
#include <QtDebug>
#include <QIcon>
#include <QFont>
#include <QFontDatabase>
#include <QKeySequence>
#include <QAction>
#include <QMainWindow>
#include <QRegularExpression>
#include <QDesktopServices>
#include <QCoreApplication>
#include <QStyle>
#include <QProxyStyle>

#include <atomic>
#include <functional>

#include "qtermwidget.h"

#if __APPLE__
extern "C" void macos_hide_titlebar(long winid);
#endif

#if __APPLE__
const QFont DEFAULT_FONT = QFont("Fira Mono", 14);
#else
const QFont DEFAULT_FONT = QFont("Fira Mono", 10);
#endif
const QString COLOR_SCHEME = "SolarizedLight";
const QRegularExpression FONT_CHANGE_REGEX("\\033\\]50;Font=([\\w\\s]+),?(\\d*)\\007");

class BQStyle: public QProxyStyle {
public:
    int styleHint(StyleHint sh, const QStyleOption *opt, const QWidget *w, QStyleHintReturn *hret) const override {
        if (sh == SH_ScrollBar_Transient)
            return true;
        return QProxyStyle::styleHint(sh, opt, w, hret);
    }
};

void add_shortcut(QWidget *parent,
                  QKeySequence const & key,
                  std::function<void()> cb) {
    QAction *action = new QAction(parent);
    action->setShortcut(key);
    QObject::connect(action, &QAction::triggered, cb);
    parent->addAction(action);
}


static std::atomic<int> _console_cnt;

void new_console(QApplication *app) {
    QTermWidget *console = new QTermWidget(0);  // startnow = 0
    _console_cnt += 1;

    console->setTerminalSizeHint(false);
    console->setColorScheme(COLOR_SCHEME);
    console->setKeyBindings("default");
    console->setMotionAfterPasting(2);  // scroll to end
    console->setTerminalFont(DEFAULT_FONT);
    console->setBidiEnabled(false);
    console->startShellProgram();

#if __APPLE__
    macos_hide_titlebar(console->winId());
    console->setScrollBarPosition(QTermWidget::ScrollBarPosition::ScrollBarRight);

    add_shortcut(console, QKeySequence("Meta+C"),
                 [=](){ console->copyClipboard(); });
    add_shortcut(console, QKeySequence("Meta+V"),
                 [=](){ console->pasteClipboard(); });
    add_shortcut(console, QKeySequence("Meta+F"),
                 [=](){ console->toggleShowSearchBar(); });
    add_shortcut(console, QKeySequence(Qt::META, Qt::Key_Minus),
                 [=](){ console->zoomOut(); });
    add_shortcut(console, QKeySequence(Qt::META, Qt::Key_Plus),
                 [=](){ console->zoomIn(); });
#else
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
#endif
    add_shortcut(console, QKeySequence("Meta+n"),
                 [=](){ new_console(app); });

    QObject::connect(console, &QTermWidget::titleChanged, [=](){});
    QObject::connect(console, &QTermWidget::receivedData, [=](QString const & text) {
        auto match = FONT_CHANGE_REGEX.match(text);
        if (match.hasMatch()) {
            QFont new_font = DEFAULT_FONT;
            new_font.setFamily(match.captured(1));
            qDebug() << "Setting font to " << new_font;
            console->setTerminalFont(new_font);
        }
    });
    QObject::connect(console, &QTermWidget::urlActivated, [](const QUrl& url, bool) {
        if (QApplication::keyboardModifiers() & (Qt::ControlModifier | Qt::MetaModifier))
            QDesktopServices::openUrl(url);
    });
    QObject::connect(console, &QTermWidget::finished, [=](){
        console->close();
        console->deleteLater();
        if (_console_cnt.fetch_sub(1) == 1)
            app->quit();
    });

    console->show();
}

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_MacDontSwapCtrlAndMeta, true);
    QApplication app(argc, argv);
    setenv("TERM", "xterm-256color", 1);
    setenv("LANG", "en_US.UTF-8", 1);

#if __APPLE__
    BQStyle *style = new BQStyle;
    style->setBaseStyle(app.style());
    app.setStyle(style);
#endif

    new_console(&app);

    return app.exec();
}
