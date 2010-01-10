#include "qspectemu.h"
#include "spkey_p.h"

QSpectemu *qspectemu = NULL;            // instance
QSpectemu *fullScreenWidget = NULL;     // fullscreen instance (needed for Qtopia)
QSpectemu *normalScreenWidget = NULL;   // non-fullscreen instance
QSpectemuMainWindow *mainWin = NULL;    // main window
QImage scr;                             // bitmap with speccy screen
QImage scrR;                            // bitmap for rotated speccy screen
bool rotated;                           // true for display rotated
bool fullScreen;                        // true to play in fullscreen
bool qvga;                              // true to display in qvga (320x240)
QString url;                            // program url for download
QTime counter;
extern int endofsingle;

// On screen key info
struct oskey {
    int key;
    int x;
    int y;
};

// User defined on screen keys
static struct oskey oskeys[] = {
    {Qt::Key_F2, 32, 32},
    {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0},
    {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0},
    {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0},
    {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0},
    {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0},
    {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0},
    {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0},
    {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0},
    {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0},
    {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0},
    {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0},
    {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0},
};

// On screen coordinates for spectkey.png
static struct oskey oskeyspng[] = {
    {Qt::Key_1, 57, 103},
    {Qt::Key_2, 111, 103},
    {Qt::Key_3, 166, 103},
    {Qt::Key_4, 220, 103},
    {Qt::Key_5, 276, 103},
    {Qt::Key_6, 330, 103},
    {Qt::Key_7, 386, 103},
    {Qt::Key_8, 440, 103},
    {Qt::Key_9, 498, 103},
    {Qt::Key_0, 553, 103},
    {Qt::Key_Q, 85, 160},
    {Qt::Key_W, 139, 160},
    {Qt::Key_E, 190, 160},
    {Qt::Key_R, 248, 160},
    {Qt::Key_T, 303, 160},
    {Qt::Key_Y, 358, 160},
    {Qt::Key_U, 413, 160},
    {Qt::Key_I, 467, 160},
    {Qt::Key_O, 524, 160},
    {Qt::Key_P, 580, 160},
    {Qt::Key_A, 96, 216},
    {Qt::Key_S, 151, 216},
    {Qt::Key_D, 207, 216},
    {Qt::Key_F, 260, 216},
    {Qt::Key_G, 317, 216},
    {Qt::Key_H, 370, 216},
    {Qt::Key_J, 426, 216},
    {Qt::Key_K, 480, 216},
    {Qt::Key_L, 535, 216},
    {Qt::Key_Enter, 590, 216},
    {Qt::Key_Shift, 60, 272},
    {Qt::Key_Z, 123, 272},
    {Qt::Key_X, 177, 272},
    {Qt::Key_C, 232, 272},
    {Qt::Key_V, 287, 272},
    {Qt::Key_B, 342, 272},
    {Qt::Key_N, 398, 272},
    {Qt::Key_M, 452, 272},
    {Qt::Key_Control, 507, 272},
    {Qt::Key_Space, 578, 272},
    {Qt::Key_F1, 250, 28},
    {Qt::Key_F2, 330, 28},
};

#define OSKEYS_SIZE (int)(sizeof(oskeys) / sizeof(oskey))
#define OSKEYSPNG_SIZE (int)(sizeof(oskeyspng) / sizeof(oskey))

bool decodeKey(int key, int *ks, int *shks, int *ki)
{
    *shks = 0;
    switch(key)
    {
    case Qt::Key_Escape:    *ks = SK_Escape;                        break;
    case Qt::Key_1:         *ks = '1';          *shks = '!';        break;
    case Qt::Key_2:         *ks = '2';          *shks = '@';        break;
    case Qt::Key_3:         *ks = '3';          *shks = '#';        break;
    case Qt::Key_4:         *ks = '4';          *shks = '$';        break;
    case Qt::Key_5:         *ks = '5';          *shks = '%';        break;
    case Qt::Key_6:         *ks = '6';          *shks = '^';        break;
    case Qt::Key_7:         *ks = '7';          *shks = '&';        break;
    case Qt::Key_8:         *ks = '8';          *shks = '*';        break;
    case Qt::Key_9:         *ks = '9';          *shks = '(';        break;
    case Qt::Key_0:         *ks = '0';          *shks = ')';        break;
    case Qt::Key_Minus:     *ks = '-';          *shks = '_';        break;
    case Qt::Key_Equal:     *ks = '=';          *shks = '+';        break;
    case Qt::Key_Backspace: *ks = SK_BackSpace;                     break;
    case Qt::Key_Tab:       *ks = SK_Tab;                           break;
    case Qt::Key_Q:         *ks = 'q';          *shks = 'Q';        break;
    case Qt::Key_W:         *ks = 'w';          *shks = 'W';        break;
    case Qt::Key_E:         *ks = 'e';          *shks = 'E';        break;
    case Qt::Key_R:         *ks = 'r';          *shks = 'R';        break;
    case Qt::Key_T:         *ks = 't';          *shks = 'T';        break;
    case Qt::Key_Y:         *ks = 'y';          *shks = 'Y';        break;
    case Qt::Key_U:         *ks = 'u';          *shks = 'U';        break;
    case Qt::Key_I:         *ks = 'i';          *shks = 'I';        break;
    case Qt::Key_O:         *ks = 'o';          *shks = 'O';        break;
    case Qt::Key_P:         *ks = 'p';          *shks = 'P';        break;
    case Qt::Key_BracketLeft:   *ks = '[';      *shks = '{';        break;
    case Qt::Key_BracketRight:  *ks = ']';      *shks = '}';        break;
    case Qt::Key_Return:    *ks = SK_Return;                        break;
    case Qt::Key_Control:   *ks = SK_Control_L;                     break;
    case Qt::Key_A:         *ks = 'a';          *shks = 'A';        break;
    case Qt::Key_S:         *ks = 's';          *shks = 'S';        break;
    case Qt::Key_D:         *ks = 'd';          *shks = 'D';        break;
    case Qt::Key_F:         *ks = 'f';          *shks = 'F';        break;
    case Qt::Key_G:         *ks = 'g';          *shks = 'G';        break;
    case Qt::Key_H:         *ks = 'h';          *shks = 'H';        break;
    case Qt::Key_J:         *ks = 'j';          *shks = 'J';        break;
    case Qt::Key_K:         *ks = 'k';          *shks = 'K';        break;
    case Qt::Key_L:         *ks = 'l';          *shks = 'L';        break;
    case Qt::Key_Semicolon: *ks = ';';          *shks = ':';        break;
    case Qt::Key_Apostrophe:*ks = '\'';         *shks = '"';        break;
    case Qt::Key_QuoteLeft: *ks = '`';          *shks = '~';        break;
    case Qt::Key_Shift:     *ks = SK_Shift_L;                       break;
    case Qt::Key_Backslash: *ks = '\\';         *shks = '|';        break;
    case Qt::Key_Z:         *ks = 'z';          *shks = 'Z';        break;
    case Qt::Key_X:         *ks = 'x';          *shks = 'X';        break;
    case Qt::Key_C:         *ks = 'c';          *shks = 'C';        break;
    case Qt::Key_V:         *ks = 'v';          *shks = 'V';        break;
    case Qt::Key_B:         *ks = 'b';          *shks = 'B';        break;
    case Qt::Key_N:         *ks = 'n';          *shks = 'N';        break;
    case Qt::Key_M:         *ks = 'm';          *shks = 'M';        break;
    case Qt::Key_Comma:     *ks = ',';          *shks = '<';        break;
    case Qt::Key_Period:    *ks = '.';          *shks = '>';        break;
    case Qt::Key_Slash:     *ks = '/';          *shks = '?';        break;
    case Qt::Key_Space:     *ks = ' ';          *shks = ' ';        break;
    case Qt::Key_F1:        *ks = -1;                               break;
    case Qt::Key_F2:        *ks = -2;                               break;
    default:
        return false;
    }
    if(*shks == 0)
    {
        *shks = *ks;
    }
    *ki = KS_TO_KEY(*ks);
    return true;
}

static void pressKey(int key)
{
    int ks, shks, ki;
    if(!decodeKey(key, &ks, &shks, &ki))
    {
        printf("unknown key=%d\n", key);
        return;
    }

    // Special keys
    if(ks < 0)
    {
        sp_paused = 1;
        if(key == Qt::Key_F1)
        {
            qspectemu->showScreen(QSpectemu::ScreenKeyboardPng);
        }
        else if(key == Qt::Key_F2)
        {
            qspectemu->showScreen(QSpectemu::ScreenProgMenu);
        }
        return;
    }

    spkb_last.keysym = ks;
    spkb_last.shifted = shks;
    spkb_last.modif = 0;
    spkb_last.index = ki;

    spkb_kbstate[ki].state = 1;
    spkb_kbstate[ki].press = counter.elapsed();
    spkb_kbstate[ki].frame = sp_int_ctr;

    spkb_state_changed = 1;
    process_keys();
}

static void releaseKey(int key)
{
    int ks, shks, ki;
    if(!decodeKey(key, &ks, &shks, &ki))
    {
        printf("unknown key=%d\n", key);
        return;
    }

    spkb_kbstate[ki].state = 0;
    spkb_kbstate[ki].press = 0;

    spkb_state_changed = 1;
    process_keys();
}

static void releaseAllKeys()
{
    for(int i = 0; i < NR_SPKEYS; i++)
    {
        spkb_kbstate[i].state = 0;
        spkb_kbstate[i].press = 0;
    }

    spkb_state_changed = 1;
    process_keys();
}

static void showErr(QWidget *parent, QString err)
{
    qWarning() << err;
    QMessageBox::critical(parent, "qspectemu", err);
}

QSpectemu::QSpectemu(QWidget *parent, Qt::WFlags f)
        : QWidget(parent, f)
{
    Q_UNUSED(f);

#ifdef QTOPIA
    QtopiaApplication::setInputMethodHint(this, QtopiaApplication::AlwaysOn);
#endif

    setFocusPolicy(Qt::StrongFocus);

    // Initialize only minimum for fullscreen widget
    if(normalScreenWidget != NULL)
    {
        fullScreenWidget = this;
        kbpix = normalScreenWidget->kbpix;
        return;
    }

    lw = new QListWidget(this);

    label = new QLabel(this);
    label->setAlignment(Qt::AlignCenter);
    QFont font = label->font();
    font.setBold(true);
    label->setFont(font);

    progress = new QProgressBar(this);
    
    bOk = new QPushButton(tr(">"), this);
    connect(bOk, SIGNAL(clicked()), this, SLOT(okClicked()));

    bBack = new QPushButton(tr("<"), this);
    connect(bBack, SIGNAL(clicked()), this, SLOT(backClicked()));

    bBind = new QPushButton(tr("Bind key"), this);
    connect(bBind, SIGNAL(clicked()), this, SLOT(bindClicked()));

    bSnap = new QPushButton(tr("Save snapshot"), this);
    connect(bSnap, SIGNAL(clicked()), this, SLOT(snapClicked()));

    bKbd = new QPushButton(tr("Keyboard"), this);
    connect(bKbd, SIGNAL(clicked()), this, SLOT(kbdClicked()));

    chkFullScreen = new QCheckBox(tr("Fullscreen"), this);
    chkRotate = new QCheckBox(tr("Rotate"), this);
    chkQvga = new QCheckBox(tr("Qvga (320x240)"), this);

    layout = new QGridLayout(this);
    layout->addWidget(label, 0, 0, 1, 2);
    layout->addWidget(progress, 1, 0, 1, 2);
    layout->addWidget(chkFullScreen, 2, 0);
    layout->addWidget(chkRotate, 3, 0);
    layout->addWidget(chkQvga, 4, 0);
    layout->addWidget(lw, 5, 0, 1, 2);
    layout->addWidget(bBind, 6, 0);
    layout->addWidget(bKbd, 6, 1);
    layout->addWidget(bSnap, 7, 0);
    layout->addWidget(bBack, 8, 0);
    layout->addWidget(bOk, 8, 1);

    kbpix.load(":/qspectkey.png");

    //setAttribute(Qt::WA_NoSystemBackground);

    argc = 0;
    argv = NULL;
    rotated = false;
    qspectemuDir = QDir::homePath() + "/.qspectemu";
    counter.start();

    normalScreenWidget = qspectemu = this;
    fullScreenWidget = new QSpectemu();

    loadCfg(NULL);
    showScreen(QSpectemu::ScreenProgList);
}

QSpectemu::~QSpectemu()
{
    for(int i = 0; i < argc; )
    {
        if(argv[i])
        {
            free(argv[i]);
        }
        i++;
        if(i == argc)
        {
            free(argv);
        }
    }
}

QSpectemuMainWindow::QSpectemuMainWindow(QWidget *parent, Qt::WFlags f)
        : QMainWindow(parent, f)
{
#ifdef QTOPIA
    this->setWindowState(Qt::WindowMaximized);
    QMenu *m = QSoftMenuBar::menuFor(this);
    m->addAction("");
#else
    resize(640, 480);
#endif
    Q_UNUSED(f);

    normalScreenWidget = new QSpectemu(this);
    setCentralWidget(qspectemu);
    mainWin = this;
}

QSpectemuMainWindow::~QSpectemuMainWindow()
{

}

bool QSpectemu::isFullScreen(QSpectemu::Screen scr)
{
    return (scr == ScreenBindings ||
            scr == ScreenKeyboardPng ||
            scr == ScreenKeyboardPngBind ||
            scr == ScreenProgRunning);
}

bool QSpectemu::isQvga(QSpectemu::Screen scr)
{
    return (scr == ScreenProgRunning ||
            scr == ScreenKeyboardPng);
}

void QSpectemu::showScreen(QSpectemu::Screen scr)
{
    if(this == fullScreenWidget)
    {
        normalScreenWidget->showScreen(scr);
        this->screen = scr;
        update();
        return;
    }

    // Update settings GUI when entering/leaving settings screen
    if(scr == ScreenProgMenu)
    {
        chkRotate->setChecked(rotated);
        chkFullScreen->setChecked(fullScreen);
        chkQvga->setChecked(qvga);
    }
    if(screen == ScreenProgMenu)
    {
        rotated = chkRotate->isChecked();
        fullScreen = chkFullScreen->isChecked();
        qvga = chkQvga->isChecked();
    }
    
    bBind->setVisible(scr == ScreenProgMenu);
    bKbd->setVisible(scr == ScreenProgMenu && screen == ScreenProgRunning);
    bSnap->setVisible(scr == ScreenProgMenu && screen == ScreenProgRunning);
    bOk->setVisible(scr == ScreenProgList || scr == ScreenProgMenu);
    bBack->setVisible(scr == ScreenProgList || scr == ScreenProgMenu || scr == ScreenProgDownload);
    chkFullScreen->setVisible(scr == ScreenProgMenu);
    chkQvga->setVisible(scr == ScreenProgMenu);
    chkRotate->setVisible(scr == ScreenProgMenu);
    lw->setVisible(scr == ScreenProgList);
    label->setVisible(scr == ScreenProgMenu || scr == ScreenProgDownload);
    if(scr == ScreenProgMenu)
    {
        label->setText(currentProg);
    }
    progress->setVisible(scr == ScreenProgDownload);

    bool enterFullScreen = fullScreen && isFullScreen(scr) && !isFullScreen(screen);
    bool leaveFullScreen = fullScreen && isFullScreen(screen) && !isFullScreen(scr);
    if(enterFullScreen)
    {
        normalScreenWidget->hide();
        qspectemu = fullScreenWidget;
        fullScreenWidget->screen = scr;
        fullScreenWidget->showInFullScreen();
    }
    if(leaveFullScreen)
    {
        fullScreenWidget->hide();
        qspectemu = normalScreenWidget;
        normalScreenWidget->show();
    }

    bool enterQvga = qvga && isQvga(scr) && !isQvga(screen);
    bool leaveQvga = qvga && isQvga(screen) && !isQvga(scr);
    if(enterQvga)
    {
        setRes(320240);
    }
    if(leaveQvga)
    {
        setRes(640480);
    }

#if QTOPIA
    if(scr == ScreenProgRunning)
    {
        QtopiaApplication::setInputMethodHint(mainWin, QtopiaApplication::AlwaysOn);
        QtopiaApplication::instance()->showInputMethod();
    }
    if(screen == ScreenProgRunning)
    {
        QtopiaApplication::setInputMethodHint(mainWin, QtopiaApplication::AlwaysOff);
        QtopiaApplication::instance()->hideInputMethod();
    }
#endif

    this->screen = scr;
    update();
}

void QSpectemu::showScreenKeyboardPngBind()
{
    showScreen(QSpectemu::ScreenKeyboardPngBind);
}

void QSpectemu::showInFullScreen()
{
    showMaximized();
    enterFullScreen();
}

void QSpectemu::enterFullScreen()
{
#ifdef QTOPIA
    // Show editor view in full screen
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    setWindowState(Qt::WindowFullScreen);
    raise();
#endif
}

bool QSpectemu::event(QEvent *event)
{
    if(this == fullScreenWidget)
    {
        if(event->type() == QEvent::WindowDeactivate)
        {
            lower();
        }
        else if(event->type() == QEvent::WindowActivate)
        {
            QString title = windowTitle();
            setWindowTitle(QLatin1String("_allow_on_top_"));
            raise();
            setWindowTitle(title);
        }
    }
    return QWidget::event(event);
}

#ifdef QTOPIA
static bool setres(const char *sysfsPath, const char *sysfsVal, const char *fbsetMode)
{
    QFile f(sysfsPath);
    if(!f.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        return false;
    }
    QProcess p;
    p.start("fbset", QStringList(fbsetMode));
    p.waitForFinished(5000);
    f.write(sysfsVal);
    f.close();
    return true;
}
#endif

bool QSpectemu::setRes(int xy)
{
#ifdef QTOPIA
    if(xy == 320240)
    {
        return setres("/sys/bus/spi/devices/spi2.0/state", "qvga-normal", "qvga") ||
                setres("/sys/bus/spi/devices/spi2.0/resolution", "qvga", "qvga");
    }
    if(xy == 640480)
    {
        return setres("/sys/bus/spi/devices/spi2.0/state", "normal", "vga") ||
                setres("/sys/bus/spi/devices/spi2.0/resolution", "vga", "vga");
    }
    return false;
#else
    Q_UNUSED(xy);
    return true;
#endif
}


int QSpectemu::getKeyPng(int x, int y)
{
    // Small screen - pressed bottom part of keyboard?
    if(kbpix.width() > width() && y > kbpix.height())
    {
        y -= kbpix.height();
        x += kbpix.width() - width();
    }

    for(int i = 0; i < OSKEYSPNG_SIZE; i++)
    {
        oskey *ki = &(oskeyspng[i]);
        if(abs(x - ki->x) < 32 && abs(y - ki->y) < 32)
        {
            return ki->key;
        }
    }
    return -1;
}

void QSpectemu::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    switch(screen)
    {
        case QSpectemu::ScreenProgRunning:
        {
            if(sp_image == 0)
            {
                return;
            }
            //p.setCompositionMode(QPainter::CompositionMode_Source);
            if(rotated)
            {
                char *s = sp_image;
                char *rStart = (char *)scrR.bits();
                char *r = rStart;

                for(int y = 0; y < TV_HEIGHT; y++)
                {
                    r = rStart + y + (TV_WIDTH - 1) * TV_HEIGHT;
                    for(int x = 0; x < TV_WIDTH; x++)
                    {
                        *r = *s;
                        s++;
                        r -= TV_HEIGHT;
                    }
                }
                p.drawImage(0, 0, scrR);
            }
            else
            {
                p.drawImage(0, 0, scr);
            }
        }
        break;
        case QSpectemu::ScreenKeyboardPng:
        {
            if(qvga)
            {
                p.scale(0.5, 0.5);
            }
        }
        case QSpectemu::ScreenKeyboardPngBind:
        {
            p.drawPixmap(0, 0, kbpix);
            if(kbpix.width() > width())
            {
                p.drawPixmap(width() - kbpix.width(), kbpix.height(), kbpix);
            }
        }
        break;
        case QSpectemu::ScreenBindings:
        {
            for(int i = 0; i < OSKEYS_SIZE; i++)
            {
                oskey *ki = &(oskeys[i]);
                if(ki->x <= 0)
                {
                    continue;
                }
                QRect rect(ki->x - 16, ki->y - 16, 32, 32);
                QString key(QChar(ki->key));
                p.fillRect(rect, QColor(64, 64, 64, 64));
                p.setPen(QColor(255, 255, 255, 255));
                p.drawText(rect, key, QTextOption(Qt::AlignCenter));
            }
            p.drawText(this->rect(), tr("Click screen to place key"), QTextOption(Qt::AlignCenter));
        }
        break;
        default:
        {
        }
        break;
    }
}

void QSpectemu::keyPressEvent(QKeyEvent *e)
{
    pressKey(e->key());
}

void QSpectemu::keyReleaseEvent(QKeyEvent *e)
{
    releaseKey(e->key());
}

void QSpectemu::mousePressEvent(QMouseEvent *e)
{
    if(screen == QSpectemu::ScreenProgRunning)
    {
        // Find nearest distance
        int x = e->x();
        int y = e->y();
        int minDist = 0x7fffffff;
        for(int i = 0; i < OSKEYS_SIZE; i++)
        {
            oskey *ki = &(oskeys[i]);
            if(ki->key <= 0)
            {
                continue;
            }
            int dist = abs(x - ki->x) + abs(y - ki->y);
            if(dist < minDist)
            {
                minDist = dist;
                pressedKeyX = ki->x;
                pressedKeyY = ki->y;
            }
        }
        for(int i = 0; i < OSKEYS_SIZE; i++)
        {
            oskey *ki = &(oskeys[i]);
            if(ki->key <= 0)
            {
                continue;
            }
            if(ki->x == pressedKeyX && ki->y == pressedKeyY)
            {
                pressKey(ki->key);
            }
        }
    }
    else if(screen == QSpectemu::ScreenBindings)
    {
        int x = e->x();
        int y = e->y();

        for(int i = 0; i < OSKEYS_SIZE; i++)
        {
            oskey *ki = &(oskeys[i]);
            if(ki->key > 0)
            {
                if(abs(x - ki->x) < 16 && abs(y - ki->y) < 16)
                {
                    // Replace binding or assign one more key?
                    QString key(QChar(ki->key));
                    if(QMessageBox::question(this, "Bind key", tr("Add (YES) or replace (NO)") + " '" + key + "' ?",
                                             QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
                    {
                        ki->key = 0;    // replace binding
                    }
                    x = ki->x;
                    y = ki->y;
                }
                continue;
            }
            ki->x = x;
            ki->y = y;
            break;
        }
        update();
        QTimer::singleShot(500, this, SLOT(showScreenKeyboardPngBind()));
    }
    else if(screen == QSpectemu::ScreenKeyboardPng)
    {
        sp_paused = 0;
        pressKey(getKeyPng(e->x(), e->y()));
        showScreen(QSpectemu::ScreenProgRunning);
    }
    else if(screen == QSpectemu::ScreenKeyboardPngBind)
    {
        int key = getKeyPng(e->x(), e->y());
        for(int i = 0; i < OSKEYS_SIZE; i++)
        {
            oskey *ki = &(oskeys[i]);
            if(ki->key <= 0)
            {
                ki->key = key;
                break;
            }
        }
        showScreen(QSpectemu::ScreenProgMenu);
    }
}

void QSpectemu::mouseReleaseEvent(QMouseEvent *)
{
    releaseAllKeys();
}

void QSpectemu::mouseMoveEvent(QMouseEvent *e)
{
    int x = e->x();
    int y = e->y();

    // Check if we moved to another key
    int minDist = 0x7fffffff;
    int newPressedX = 0;
    int newPressedY = 0;
    for(int i = 0; i < OSKEYS_SIZE; i++)
    {
        oskey *ki = &(oskeys[i]);
        if(ki->key <= 0)
        {
            continue;
        }
        int dist = abs(x - ki->x) + abs(y - ki->y);
        if(dist < minDist)
        {
            minDist = dist;
            newPressedX = ki->x;
            newPressedY = ki->y;
        }
    }

    if(newPressedX == pressedKeyX && newPressedY == pressedKeyY)
    {
        return;
    }
    pressedKeyX = newPressedX;
    pressedKeyY = newPressedY;

    // Release old pressed keys and press new
    releaseAllKeys();
    for(int i = 0; i < OSKEYS_SIZE; i++)
    {
        oskey *ki = &(oskeys[i]);
        if(ki->key <= 0)
        {
            continue;
        }
        if(ki->x == newPressedX && ki->y == newPressedY)
        {
            pressKey(ki->key);
        }
    }
}

// Load global cfg (progFile==NULL) or cfg for given program
void QSpectemu::loadCfg(QString prog)
{
    bool list = (prog == NULL);
    if(list)
    {
        lw->clear();
    }

    QDomDocument doc;
    QDir dir(qspectemuDir);
    if(!dir.exists())
    {
        dir.mkpath(dir.path());
    }
    QFile f(dir.filePath("qspectemu.xml"));
    if(!f.exists())
    {
        QFile defXml(":/qspectemu.xml");
        if(!defXml.open(QIODevice::ReadOnly) || !f.open(QIODevice::WriteOnly))
        {
            return;
        }
        QByteArray content = defXml.readAll();
        f.write(content);
        f.close();
        defXml.close();
    }
    if(!f.open(QIODevice::ReadOnly))
    {
        return;
    }
    if (!doc.setContent(&f))
    {
        showErr(this, tr("Error while reading ") + f.fileName());
        f.close();
        return;
    }
    QDomElement root = doc.documentElement();
    for(QDomElement progElem = doc.firstChildElement("qspectemu").firstChildElement("prog"); !progElem.isNull(); progElem = progElem.nextSiblingElement("prog"))
    {
        QString file = progElem.attribute("file");
        if(list)
        {
            QListWidgetItem *item = new QListWidgetItem(file, lw, QListWidgetItem::UserType);
            lw->addItem(item);
            continue;
        }
        if(file != prog)
        {
            continue;
        }

        // Read settings for matching program
        QString rotateAttr = progElem.attribute("rotate");
        rotated = (rotateAttr == "yes");

        QString fullscreenAttr = progElem.attribute("fullscreen");
        fullScreen = (fullscreenAttr == "yes");

        QString qvgaAttr = progElem.attribute("qvga");
        qvga = (qvgaAttr == "yes");

        url = progElem.attribute("url");

        QDomElement bindsElem = progElem.firstChildElement("binds");
        if(bindsElem.isNull())
        {
            continue;
        }
        QDomElement bind = bindsElem.firstChildElement("bind");
        for(int j = 0; j < OSKEYS_SIZE; j++)
        {
            oskey *k = &(oskeys[j]);
            if(bind.isNull())
            {
                k->key = 0;
                continue;
            }
            k->key = bind.attribute("key").toInt();
            k->x = bind.attribute("x").toInt();
            k->y = bind.attribute("y").toInt();
            bind = bind.nextSiblingElement("bind");
        }
    }
    if(list)
    {
        lw->setCurrentRow(0);
    }
    f.close();
}

// Save global cfg (progFile==NULL) or cfg for given program
void QSpectemu::saveCfg(QString prog)
{
    QDomDocument doc;
    QString cfgFile = qspectemuDir + "/qspectemu.xml";
    QFile f(cfgFile);
    if(f.exists())
    {
        if(f.open(QIODevice::ReadOnly))
        {
            if(!doc.setContent(&f))
            {
                showErr(this, tr("Error while parsing xml parsing file ") + cfgFile);
            }
            f.close();
        }
        else
        {
            showErr(this, tr("Error while opening cfg file ") + cfgFile + " " + f.errorString());
        }
    }

    QDomElement root = doc.documentElement();
    QDomElement progElem;
    for(progElem = root.firstChildElement("prog"); !progElem.isNull(); progElem = progElem.nextSiblingElement("prog"))
    {
        QString file = progElem.attribute("file");
        if(file == prog)
        {
            break;
        }
    }

    if(progElem.isNull())
    {
        progElem = doc.createElement("prog");
        progElem.setAttribute("file", prog);
        root.appendChild(progElem);
    }

    // Save settings for current program
    progElem.setAttribute("rotate", rotated ? "yes" : "no");
    progElem.setAttribute("fullscreen", fullScreen ? "yes" : "no");
    progElem.setAttribute("qvga", qvga ? "yes" : "no");

    QDomElement bindsElem = progElem.firstChildElement("binds");
    if(bindsElem.isNull())
    {
        bindsElem = doc.createElement("binds");
        progElem.appendChild(bindsElem);
    }

    QDomNodeList binds = bindsElem.childNodes();
    while(binds.length() > 0)
    {
        bindsElem.removeChild(binds.at(0));
    }
    for(int j = 0; j < OSKEYS_SIZE; j++)
    {
        oskey *k = &(oskeys[j]);
        if(k->key <= 0)
        {
            break;
        }
        QDomElement bind = doc.createElement("bind");
        bind.setAttribute("key", QString::number(k->key));
        bind.setAttribute("x", QString::number(k->x));
        bind.setAttribute("y", QString::number(k->y));
        bindsElem.appendChild(bind);
    }

    if(!f.open(QIODevice::WriteOnly))
    {
        showErr(this, tr("Error while saving cfg file ") + cfgFile);
        return;
    }
    QTextStream ts(&f);
    ts << doc.toString();
    f.close();
}

void QSpectemu::saveCurrentProgCfg()
{
    saveCfg(currentProg);
}

void QSpectemu::setProgramFullPath()
{
    currentProgFullPath = qspectemuDir + "/" + currentProg;
}

bool QSpectemu::download(QString url, QString destPath, QString filename)
{
    label->setText(tr("Downloading") + " " + filename);
    showScreen(ScreenProgDownload);

    QString host = url;
    QString reqPath;
    int port = 80;

    if(url.startsWith("http://"))
    {
        host.remove(0, 7);
    }

    int colonIndex = host.indexOf(':');
    int slashIndex = host.indexOf('/');
    if(slashIndex < 0)
    {
        return false;
    }
    reqPath = host.right(host.length() - slashIndex).replace(" ", "%20");
    host = host.left(slashIndex);
    if(colonIndex > 0)
    {
        QString portStr = host.right(host.length() - colonIndex - 1);
        host = host.left(colonIndex);
        port = portStr.toInt(0, 10);
    }

connect:
    QTcpSocket sock(this);
    sock.setReadBufferSize(65535);
    sock.connectToHost(host, port);
    if(!sock.waitForConnected(5000))
    {
        QMessageBox::critical(this, tr("qspectemu"), sock.errorString());
        return false;
    }

    QByteArray req("GET ");
    req.append(reqPath);
    req.append(" HTTP/1.1\r\nHost: ");
    req.append(host);
    req.append(':');
    req.append(QByteArray::number(port));
    req.append("\r\n\r\n");

    sock.write(req);
    sock.flush();
    sock.waitForBytesWritten();

    int contentLen = 0;
    bool html = false;
    QByteArray line;
    for(;;)
    {
        line = sock.readLine();
        if(line.isEmpty())
        {
            if(sock.waitForReadyRead(5000))
            {
                continue;
            }
            break;
        }
        if(line.trimmed().isEmpty())
        {
            break;
        }
        html = html | (line.indexOf("Content-Type: text/html") == 0);
        if(line.indexOf("Content-Length: ") == 0)
        {
            contentLen = line.remove(0, 16).trimmed().toInt(0, 10);
        }
    }

    if(html)
    {
        QByteArray text = sock.readAll();
        sock.close();
        if(text.length() == 0)
        {
            QMessageBox::critical(this, tr("qspectemu"),
                                  tr("No response from ") + host);
            return false;
        }
        text.replace("</br>", "\n");
        if(QMessageBox::information(this, "qspectemu", text,
                                 QMessageBox::Ok | QMessageBox::Retry) == QMessageBox::Retry)
        {
            goto connect;
        }

        return false;
    }

    QFile f(destPath);
    if(!f.open(QFile::WriteOnly))
    {
        QMessageBox::critical(this, tr("qspectemu"),
                              tr("Unable to save file:\r\n\r\n") + f.errorString());
        sock.close();
        return false;
    }

#ifdef QTOPIA
     QtopiaApplication::setPowerConstraint(QtopiaApplication::DisableSuspend);
#endif

    if(contentLen <= 0)
    {
        QMessageBox::critical(this, tr("qspectemu"), tr("Couldnt read content length"));
        contentLen = 0x7fffffff;
    }
    progress->setMaximum(contentLen);
    progress->setValue(0);
    int remains = contentLen;

    char buf[65535];
    int count;
    abort = false;
    for(;;)
    {
        QApplication::processEvents();
        if(abort)
        {
            break;
        }
        count = sock.read(buf, 65535);
        if(count < 0)
        {
            break;
        }
        f.write(buf, count);
        f.flush();
        remains -= count;
        if(remains <= 0)
        {
            break;
        }
        progress->setValue(contentLen - remains);
    }
    f.close();
    sock.close();

#ifdef QTOPIA
    QtopiaApplication::setPowerConstraint(QtopiaApplication::Enable);
#endif

    return true;
}

void QSpectemu::okClicked()
{
    if(screen == QSpectemu::ScreenProgList)
    {
        QListWidgetItem *sel = lw->currentItem();
        if(sel == NULL)
        {
            return;
        }
        currentProg = sel->text();
        setProgramFullPath();
        loadCfg(currentProg);
        showScreen(QSpectemu::ScreenProgMenu);
    }
    else if(screen == QSpectemu::ScreenProgMenu)
    {
        if(sp_paused)
        {
            sp_paused = 0;
            showScreen(QSpectemu::ScreenProgRunning);
            return;
        }

        if(!QFile::exists(currentProgFullPath) && url.length() > 0)
        {
            if(QMessageBox::question(this, "Dowload program?", url, QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
            {
                return;
            }
            if(!download(url, currentProgFullPath, currentProg))
            {
                showScreen(ScreenProgMenu);
                return;
            }
        }

        if(argv == NULL)
        {
            QStringList args = QApplication::arguments();
            argc = args.count();
            argv = (char **) malloc_err(sizeof(char *) * argc);
            for(int i = 0; i < argc; i++)
            {
                argv[i] = strdup((char *) args.at(i).toLocal8Bit().constData());
            }
        }

        spma_init_privileged();
        spcf_pre_check_options(argc, argv);
        check_params(argc, argv);
        sp_init();

        if(QFile::exists(currentProgFullPath))
        {
            load_snapshot_file_type(currentProgFullPath.toLatin1().data(), -1);
        }

        showScreen(QSpectemu::ScreenProgRunning);
        saveCurrentProgCfg();

        start_spectemu();
    }
}

void QSpectemu::backClicked()
{
    if(screen == ScreenProgMenu)
    {
        sp_paused = 0;
        endofsingle = 1;
        showScreen(QSpectemu::ScreenProgList);
        saveCurrentProgCfg();
    }
    else if(screen == ScreenProgList)
    {
        mainWin->close();
    }
    else if(screen == ScreenProgDownload)
    {
        abort = true;
    }
}

void QSpectemu::bindClicked()
{
    showScreen(QSpectemu::ScreenBindings);
}

void QSpectemu::kbdClicked()
{
    showScreen(QSpectemu::ScreenKeyboardPng);
}

void QSpectemu::snapClicked()
{
    if(currentProg.length() <= 5)
    {
        showErr(this, tr("Invalid filename"));
        return;
    }
    QString ext = currentProg.right(4);
    int nameEnd = currentProg.length() - 5;
    int num = 0;
    int exp = 1;
    while(nameEnd >= 0 && currentProg.at(nameEnd).isDigit())
    {
        num += exp * currentProg.at(nameEnd).digitValue();
        exp *= 10;
        nameEnd--;
    }
    QString name = currentProg.left(nameEnd + 1);

    do
    {
        num++;
        currentProg = name + QString::number(num) + ext;
        setProgramFullPath();
    }
    while(QFile::exists(currentProgFullPath));

    save_snapshot_file(currentProgFullPath.toAscii().data());
    saveCurrentProgCfg();
    loadCfg(NULL);
    showScreen(ScreenProgMenu);
}

#ifdef	__cplusplus
extern	"C" {
#endif

int small_screen = 0;
int vga_pause_bg = 0;

int scrmul;           /* DUMMY */
int use_shm;          /* DUMMY */
int privatemap;       /* DUMMY */
int pause_on_iconify; /* DUMMY */
int fullscreen;       /* DUMMY */

spkeyboard kb_mkey;
const int need_switch_mode = 1;

void init_spect_scr(void)
{
    spscr_init_colors();
    for(int i = 0; i < 16; i++) sp_colors[i] = (unsigned char) i;
    spscr_init_mask_color();

    scr = QImage(TV_WIDTH, TV_HEIGHT, QImage::Format_Indexed8);
    scrR = QImage(TV_HEIGHT, TV_WIDTH, QImage::Format_Indexed8);
    scr.setNumColors(COLORNUM);
    scrR.setNumColors(COLORNUM);
    for(int i = 0; i < COLORNUM; i++)
    {
	rgb c = spscr_crgb[i];
	QRgb qc = qRgb(c.r * 4, c.g * 4, c.b * 4);
	scr.setColor(i, qc);
        scrR.setColor(i, qc);
    }
    sp_image = (char *) scr.bits();

    spscr_init_line_pointers(TV_HEIGHT);
}

void spkey_textmode(void)
{
}

void spkey_screenmode(void)
{
}

void spkb_process_events(int)
{
    QApplication::processEvents();
}

void init_spect_key(void)
{
    clear_keystates();
    init_basekeys();
}

int display_keyboard(void)
{
    return 0;
}

void update_screen(void)
{
    if(sp_border_update)
    {
        memset(sp_imag_mark, 0, HEIGHT);
        qspectemu->update();
        return;
    }

    int top = -1;
    int bottom = 0;
    for(int i = 0; i < HEIGHT; i++)
    {
	if(sp_imag_mark[i] == 0)
	{
	    continue;
	}
	if(top < 0)
	{
	    top = i;
	}
	bottom = i;
	sp_imag_mark[i] = 0;
    }
    top += Y_OFF;
    bottom += Y_OFF;

    if(rotated)
    {
        qspectemu->update(top, X_OFF, bottom + 1, WIDTH);
    }
    else
    {
        qspectemu->update(X_OFF, top, WIDTH, bottom + 1);
    }
}

void destroy_spect_scr(void)
{
}

void resize_spect_scr(int)
{
}

void spcf_read_xresources(void)
{
}

void spscr_refresh_colors(void)
{
}

void spscr_toggle_fullscreen(void)
{
}

#ifdef	__cplusplus
};
#endif

#ifdef QTOPIA

QTOPIA_ADD_APPLICATION(QTOPIA_TARGET,QSpectemuMainWindow)
QTOPIA_MAIN

#else

#include <QtGui/QApplication>
#include "qspectemu.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QSpectemuMainWindow w;
    w.show();
    return a.exec();
}

#endif
