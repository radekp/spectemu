#ifndef QSPECTEMU_H
#define QSPECTEMU_H

#include <QWidget>
#include <QTimer>
#include <QPaintEvent>
#include <QPainter>
#include <QApplication>
#include <QImage>
#include <QPushButton>
#ifdef QTOPIA
#include <QtopiaApplication>
#endif

#include <stdio.h>

#include "spmain.h"
#include "spperif.h"
#include "misc.h"
#include "xscr.h"
#include "spconf.h"
#include "spscr_p.h"

#include "ax.h"

#define TV_WIDTH   320
#define TV_HEIGHT  256

#define WIDTH      256
#define HEIGHT     192

class QSpectemu : public QWidget
{
    Q_OBJECT

public:
    QSpectemu(QWidget *parent = 0, Qt::WFlags f = 0);
    ~QSpectemu();

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void keyPressEvent(QKeyEvent *);
    void keyReleaseEvent(QKeyEvent *);

private:
    int argc;
    char **argv;

private slots:
    void startSpectemu();

};

#endif // QSPECTEMU_H
