#include "qspectemu.h"
#include "spkey_p.h"

QSpectemu *qspectemu = NULL;            // instance
QSpectemu *fullScreenWidget = NULL;     // fullscreen instance (needed for Qtopia)
QSpectemu *normalScreenWidget = NULL;   // non-fullscreen instance
QImage scr;                             // bitmap with speccy screen
QImage scrR;                            // bitmap for rotated speccy screen
int scrTop;                             // used in qvga mode
bool rotated;                           // true for display rotated
bool fullScreen;                        // true to play in fullscreen
bool qvga;                              // true to display in qvga (320x240)
bool virtKeyb;                          // true to display virtual keyboard
QTime counter;

// On screen key info
struct oskey {
    int key;
    int x;
    int y;
};

// User defined on screen keys
static struct oskey oskeys[] = {
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
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
    case Qt::Key_Backslash: *ks = '\\';          *shks = '|';       break;
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
        : QWidget(parent)
{
#ifdef QTOPIA
    this->setWindowState(Qt::WindowMaximized);
    QtopiaApplication::setInputMethodHint(this, QtopiaApplication::AlwaysOn);
#else
    Q_UNUSED(f);
#endif

    // Initialize only minimum for fullscreen widget
    if(normalScreenWidget != NULL)
    {
        fullScreenWidget = this;
        return;
    }

    lw = new QListWidget(this);

    bOk = new QPushButton(tr(">"), this);
    connect(bOk, SIGNAL(clicked()), this, SLOT(okClicked()));

    bBinds = new QPushButton(tr("Bindings"), this);
    connect(bBinds, SIGNAL(clicked()), this, SLOT(bindsClicked()));

    bKbd = new QPushButton(tr("Keyboard"), this);
    connect(bKbd, SIGNAL(clicked()), this, SLOT(kbdClicked()));

    bQuit = new QPushButton(tr("Quit"), this);
    connect(bQuit, SIGNAL(clicked()), this, SLOT(quitClicked()));

    chkFullScreen = new QCheckBox(tr("Fullscreen"), this);
    chkRotate = new QCheckBox(tr("Rotate"), this);
    chkQvga = new QCheckBox(tr("Qvga (320x240)"), this);
    chkVirtKeyb = new QCheckBox(tr("Virtual keyboard"), this);

    layout = new QVBoxLayout(this);
    layout->addWidget(lw);
    layout->addWidget(chkFullScreen);
    layout->addWidget(chkRotate);
    layout->addWidget(chkQvga);
    layout->addWidget(chkVirtKeyb);
    layout->addWidget(bBinds);
    layout->addWidget(bKbd);
    layout->addWidget(bQuit);
    layout->addWidget(bOk);

    scrTop = 0;
    kbpix.load(":/qspectkey.png");

    //setAttribute(Qt::WA_NoSystemBackground);

    argc = 0;
    argv = 0;
    rotated = false;
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

void QSpectemu::showScreen(QSpectemu::Screen scr)
{
    this->screen = scr;

    if(scr == QSpectemu::ScreenProgRunning)
    {
        normalScreenWidget->hide();
        fullScreenWidget->screen = scr;
        fullScreenWidget->showInFullScreen();
        qspectemu = fullScreenWidget;
    }

    bBinds->setVisible(scr == QSpectemu::ScreenProgMenu);
    bKbd->setVisible(scr == QSpectemu::ScreenProgMenu);
    bOk->setVisible(scr == QSpectemu::ScreenProgList || scr == QSpectemu::ScreenProgMenu);
    bQuit->setVisible(scr == QSpectemu::ScreenProgList || scr == QSpectemu::ScreenProgMenu);
    chkFullScreen->setVisible(scr == QSpectemu::ScreenProgMenu);
    chkQvga->setVisible(scr == QSpectemu::ScreenProgMenu);
    chkRotate->setVisible(scr == QSpectemu::ScreenProgMenu);
    chkVirtKeyb->setVisible(scr == QSpectemu::ScreenProgMenu);
    lw->setVisible(scr == QSpectemu::ScreenProgList);

    qspectemu->update();
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

void QSpectemu::setRes(int xy)
{
#ifdef QTOPIA
    if(xy == 320240 || xy == 640480)
    {
        QFile f("/sys/bus/spi/devices/spi2.0/state");
        f.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
        if(xy == 320240)
        {
            QProcess p(this);
            p.start("fbset", QStringList("qvga"));
            p.waitForFinished(5000);
            f.write("qvga-normal");
        }
        else if(xy == 640480)
        {
            QProcess p(this);
            p.start("fbset", QStringList("vga"));
            p.waitForFinished(5000);
            f.write("normal");
        }
        f.close();
    }
#else
    Q_UNUSED(xy);
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
                p.drawImage(0, scrTop, scrR);
            }
            else
            {
                p.drawImage(0, scrTop, scr);
            }
        }
        break;
        case QSpectemu::ScreenKeyboardPng:
        case QSpectemu::ScreenKeyboardPngBind:
        {
            {
                p.drawPixmap(0, 0, kbpix);
                if(kbpix.width() > width())
                {
                    p.drawPixmap(width() - kbpix.width(), kbpix.height(), kbpix);
                }
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
        int minDist = 0xffff;
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
        // Press all keys in this distance
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
        pressKey(getKeyPng(e->x(), e->y()));
        showScreen(QSpectemu::ScreenSpectrum);
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
        showScreen(QSpectemu::ScreenBindings);
        if(QMessageBox::question(this, "ZX Spectrum", tr("Bind next key?"),
                                 QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
        {
            showScreen(QSpectemu::ScreenBindings);
        }
        else
        {
            qspectemu->saveCurrentProgCfg();
            showScreen(QSpectemu::ScreenSpectrum);
        }
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
    int minDist = 0xffff;
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
    QDir dir(QDir::homePath() + "/.qspectemu");
    if(!dir.exists())
    {
        dir.mkpath(dir.path());
    }
    QFile f(dir.filePath("qspectemu.xml"));
    if(!f.exists())
    {
        if(!f.open(QIODevice::WriteOnly))
        {
            return;
        }
        f.write("<qspectemu>\n  <prog file=\"program1.sna\" rotate=\"no\" fullscreen=\"no\" qvga=\"no\" virtKeyboard=\"no\"></prog>\n</qspectemu>");
        f.close();
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
        chkRotate->setChecked(rotated);

        QString fullscreenAttr = progElem.attribute("fullscreen");
        fullScreen = (fullscreenAttr == "yes");
        chkFullScreen->setChecked(fullScreen);

        QString qvgaAttr = progElem.attribute("qvga");
        qvga = (qvgaAttr == "yes");
        chkQvga->setChecked(qvga);

        QString vkAttr = progElem.attribute("virtKeyboard");
        virtKeyb = (vkAttr == "yes");
        chkVirtKeyb->setChecked(virtKeyb);

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

    f.close();
}

// Save global cfg (progFile==NULL) or cfg for given program
void QSpectemu::saveCfg(QString prog)
{
    QDomDocument doc;
    QString cfgFile = QDir::homePath() + "/.qspectemu/qspectemu.xml";
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
    for(QDomElement progElem = root.firstChildElement("prog"); !progElem.isNull(); progElem = progElem.nextSiblingElement("prog"))
    {
        QString file = progElem.attribute("file");
        if(file != prog)
        {
            continue;
        }
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

void QSpectemu::bindsClicked()
{
}

void QSpectemu::kbdClicked()
{
}

void QSpectemu::quitClicked()
{
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
        loadCfg(currentProg);
        showScreen(QSpectemu::ScreenProgMenu);
    }
    else if(screen == QSpectemu::ScreenProgMenu)
    {
        QStringList args = QApplication::arguments();
        argc = args.count();
        argv = (char **) malloc_err(sizeof(char *) * argc);
        for(int i = 0; i < argc; i++)
        {
            argv[i] = strdup((char *) args.at(i).toLocal8Bit().constData());
        }

        spma_init_privileged();
        spcf_pre_check_options(argc, argv);
        check_params(argc, argv);
        sp_init();

        load_snapshot_file_type(currentProg.toLatin1().data(), -1);

        showScreen(QSpectemu::ScreenProgRunning);
        start_spectemu();
    }
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

    if(rotated)
    {
        qspectemu->update(top + scrTop, 0, scrTop + bottom + 1, TV_WIDTH);
    }
    else
    {
        qspectemu->update(0, top + scrTop, TV_WIDTH, scrTop + bottom + 1);
    }
    //qspectemu->update();
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
