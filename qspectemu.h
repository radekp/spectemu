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
#include <QListWidget>
#include <QListWidgetItem>
#include <QProcess>
#include <QMessageBox>
#ifdef QTOPIA
#include <QtopiaApplication>
#endif

#include <stdio.h>

#include "spmain.h"
#include "spperif.h"
#include "misc.h"
#include "spconf.h"
#include "spscr_p.h"

#define TV_WIDTH   320
#define TV_HEIGHT  192

#define WIDTH      256
#define HEIGHT     192

class RunScreen : public QWidget
{
    Q_OBJECT

public:
    RunScreen();
    void showScreen();
    void setRes(int xy);

protected:
    bool event(QEvent *);
    void paintEvent(QPaintEvent *);
    void enterFullScreen();
    void keyPressEvent(QKeyEvent *);
    void keyReleaseEvent(QKeyEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);

private:
    enum Screen
    {
        ScreenSpectrumUnbinded,     // running spectrum, no keys binded
        ScreenSpectrum,             // running spectrum
        ScreenKeyboardPng,          // big on screen keyboard
        ScreenKeyboardPngBind,      // big on screen keyboard while binding key
        ScreenBindings,             // showing how keys are binded and allow edit/add keys
    };

    Screen screen;
    int pressedKeyX;                // x and y of pressed on screen key
    int pressedKeyY;
    void showScreen(Screen screen);

private slots:
    void showScreenKeyboardPngBind();
};


class QSpectemu : public QWidget
{
    Q_OBJECT

public:
    QSpectemu(QWidget *parent = 0, Qt::WFlags f = 0);
    ~QSpectemu();

private:
    int argc;
    char **argv;
    QListWidget *lw;
    QPushButton *bOk;
    QVBoxLayout *layout;
    void fillLw();

private slots:
    void okClicked();

};

#endif // QSPECTEMU_H
