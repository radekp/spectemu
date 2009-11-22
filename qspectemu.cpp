#include "qspectemu.h"
#include "spkey_p.h"

QSpectemu *qspectemu;
QImage scr;
bool updating;

QSpectemu::QSpectemu(QWidget *parent, Qt::WFlags f)
    : QWidget(parent)
{
#ifdef QTOPIA
    this->setWindowState(Qt::WindowMaximized);
    QtopiaApplication::setInputMethodHint(this, QtopiaApplication::AlwaysOn);
#else
    Q_UNUSED(f);
#endif

    argc = 0;
    argv = 0;
    updating = false;
    qspectemu = this;
    QTimer::singleShot(1, this, SLOT(startSpectemu()));
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

void QSpectemu::startSpectemu()
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

    start_spectemu();
}

void QSpectemu::paintEvent(QPaintEvent *e)
{
    if(sp_image == 0)
    {
        return;
    }
    QPainter p(this);
    p.drawImage(0, 0, scr);
    updating = false;
}

void QSpectemu::mousePressEvent(QMouseEvent *e)
{

}

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
        case Qt::Key_Control:   *ks = SK_Control_L;                        break;
        case Qt::Key_A:  *ks = 'a';      *shks = 'A';        break;
        case Qt::Key_S:  *ks = 's';      *shks = 'S';        break;
        case Qt::Key_D:  *ks = 'd';      *shks = 'D';        break;
        case Qt::Key_F:  *ks = 'f';      *shks = 'F';        break;
        case Qt::Key_G:  *ks = 'g';      *shks = 'G';        break;
        case Qt::Key_H:  *ks = 'h';      *shks = 'H';        break;
        case Qt::Key_J:  *ks = 'j';      *shks = 'J';        break;
        case Qt::Key_K:  *ks = 'k';      *shks = 'K';        break;
        case Qt::Key_L:  *ks = 'l';      *shks = 'L';        break;
        case Qt::Key_Semicolon:  *ks = ';';      *shks = ':';        break;
        case Qt::Key_Apostrophe: *ks = '\'';     *shks = '"';        break;
        case Qt::Key_QuoteLeft:  *ks = '`';      *shks = '~';        break;
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

void QSpectemu::keyPressEvent(QKeyEvent *e)
{
    int ks, shks, ki;
    if(!decodeKey(e->key(), &ks, &shks, &ki))
    {
        printf("unknown key=%d\n", e->key());
        return;
    }

    spkb_last.keysym = ks;
    spkb_last.shifted = shks;
    spkb_last.modif = 0;
    spkb_last.index = ki;

    spkb_kbstate[ki].state = 1;
    spkb_kbstate[ki].press = 1234;
    spkb_kbstate[ki].frame = sp_int_ctr;

    spkb_state_changed = 1;
    process_keys();
}

void QSpectemu::keyReleaseEvent(QKeyEvent *e)
{
    int ks, shks, ki;
    if(!decodeKey(e->key(), &ks, &shks, &ki))
    {
        printf("unknown key=%d\n", e->key());
        return;
    }

    spkb_kbstate[ki].state = 0;
    spkb_kbstate[ki].press = 0;

    spkb_state_changed = 1;
    process_keys();
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
    scr.setNumColors(COLORNUM);
    for(int i = 0; i < COLORNUM; i++)
    {
        rgb c = spscr_crgb[i];
        QRgb qc = qRgb(c.r * 4, c.g * 4, c.b * 4);
        scr.setColor(i, qc);
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

void spkb_process_events(int evenframe)
{
    QApplication::processEvents();
}

void init_spect_key(void)
{
}

int display_keyboard(void)
{
    return 0;
}

void update_screen(void)
{
    if(updating)
    {
        return;
    }
    updating = true;
    QTimer::singleShot(100, qspectemu, SLOT(update()));
    //qspectemu->update();
}

void destroy_spect_scr(void)
{
}

void resize_spect_scr(int s)
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
