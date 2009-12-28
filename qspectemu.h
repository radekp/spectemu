#ifndef QSPECTEMU_H
#define QSPECTEMU_H

#include <QWidget>
#include <QTime>
#include <QTimer>
#include <QPaintEvent>
#include <QPainter>
#include <QApplication>
#include <QImage>
#include <QVBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QListWidget>
#include <QListWidgetItem>
#include <QProcess>
#include <QMessageBox>
#include <QtXml>
#include <QFile>
#ifdef QTOPIA
#include <QtopiaApplication>
#endif

#include <stdio.h>

#include "spmain.h"
#include "spperif.h"
#include "misc.h"
#include "spconf.h"
#include "spscr_p.h"
#include "snapshot.h"

#define TV_WIDTH   320
#define TV_HEIGHT  240

#define WIDTH      256
#define HEIGHT     192

#define X_OFF ((TV_WIDTH - WIDTH) / 2)
#define Y_OFF ((TV_HEIGHT - HEIGHT) / 2)

class QSpectemu : public QWidget
{
    Q_OBJECT

public:
    enum Screen
    {
        ScreenProgList,             // list of available programs
        ScreenProgMenu,             // menu for slected program options
        ScreenKeyboardPng,          // big on screen keyboard
        ScreenKeyboardPngBind,      // big on screen keyboard while binding key
        ScreenBindings,             // showing how keys are binded and allow edit/add keys
        ScreenProgRunning,          // program is running
    };

    QSpectemu(QWidget *parent = 0, Qt::WFlags f = 0);
    ~QSpectemu();

    void showScreen(QSpectemu::Screen);
    bool setRes(int xy);

private:
    int argc;
    char **argv;
    QString currentProg;
    Screen screen;
    QPixmap kbpix;
    int pressedKeyX;                // x and y of pressed on screen key
    int pressedKeyY;
    QListWidget *lw;
    QPushButton *bOk;
    QPushButton *bBack;
    QVBoxLayout *layout;
    QPushButton *bBind;
    QPushButton *bKbd;
    QPushButton *bSnap;
    QCheckBox *chkQvga;
    QCheckBox *chkRotate;
    QCheckBox *chkFullScreen;
    QCheckBox *chkVirtKeyb;
    int getKeyPng(int x, int y);
    void loadCfg(QString prog);
    void saveCfg(QString prog);
    void saveCurrentProgCfg();
    void showInFullScreen();

private slots:
    void okClicked();
    void backClicked();
    void bindClicked();
    void kbdClicked();
    void snapClicked();
    void showScreenKeyboardPngBind();

protected:
    bool event(QEvent *);
    void paintEvent(QPaintEvent *);
    void enterFullScreen();
    void keyPressEvent(QKeyEvent *);
    void keyReleaseEvent(QKeyEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);

};

#endif // QSPECTEMU_H
