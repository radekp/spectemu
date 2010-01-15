#ifndef QSPECTEMU_H
#define QSPECTEMU_H

#include <QMainWindow>
#include <QWidget>
#include <QTime>
#include <QTimer>
#include <QPaintEvent>
#include <QPainter>
#include <QApplication>
#include <QImage>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QCheckBox>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMenu>
#include <QAction>
#include <QMenuBar>
#include <QProcess>
#include <QMessageBox>
#include <QProgressBar>
#include <QtXml>
#include <QFile>
#include <QTcpSocket>
#ifdef QTOPIA
#include <QSoftMenuBar>
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
        ScreenNone,                 // when application is uninitialized
        ScreenProgList,             // list of available programs
        ScreenProgMenu,             // menu for slected program options
        ScreenProgDownload,         // downloading program from internet
        ScreenKeyboardPng,          // big on screen keyboard
        ScreenKeyboardPngBind,      // big on screen keyboard while binding key
        ScreenBindings,             // showing how keys are binded and allow edit/add keys
        ScreenProgRunning,          // program is running
    };

    QSpectemu(QWidget *parent = 0, Qt::WFlags f = 0);
    ~QSpectemu();

    void showScreen(QSpectemu::Screen);
    bool setRes(int xy);
    void releaseKeysLater();

private:
    int argc;
    char **argv;
    QString qspectemuDir;
    QString currentProg;
    QString currentProgFullPath;
    Screen screen;
    QPixmap kbpix;
    QPixmap kbpix320;
    int pressedKeyX;                // x and y of pressed on screen key
    int pressedKeyY;
    bool abort;
    bool pngKeyDown;                // true if key from png keyboard is down
    bool paintKeyLocations;         // paint will also show on screen key locations
    int showScreenProgMenuCancel;
    QListWidget *lw;
    QPushButton *bOk;
    QPushButton *bBack;
    QPushButton *bBind;
    QPushButton *bKbd;
    QPushButton *bSnap;
    QLabel *label;
    QProgressBar *progress;
    QCheckBox *chkQvga;
    QCheckBox *chkRotate;
    QCheckBox *chkFullScreen;
    QCheckBox *chkFeedback;
    QCheckBox *chkAutocorrect;
    QGridLayout *layout;
    bool isFullScreen(Screen scr);
    bool isQvga(Screen scr);
    int getKeyPng(int x, int y, bool nearest);
    void loadCfg(QString prog);
    void saveCfg(QString prog);
    void saveCurrentProgCfg();
    void showInFullScreen();
    bool download(QString url, QString destPath, QString filename);
    void setProgramFullPath();

private slots:
    void okClicked();
    void backClicked();
    void bindClicked();
    void kbdClicked();
    void snapClicked();
    void showScreenProgMenu();
    void releaseKeysLaterImpl();

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

class QSpectemuMainWindow : public QMainWindow
{
public:
    QSpectemuMainWindow(QWidget *parent = 0, Qt::WFlags f = 0);
    ~QSpectemuMainWindow();
};

#endif // QSPECTEMU_H
