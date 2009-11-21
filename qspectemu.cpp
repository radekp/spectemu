#include "qspectemu.h"
#include "spkey_p.h"

QSpectemu *qspectemu;
QImage scr(TV_WIDTH, TV_HEIGHT, QImage::Format_ARGB32);

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
}

void QSpectemu::mousePressEvent(QMouseEvent *e)
{

}

void QSpectemu::keyPressEvent(QKeyEvent *e)
{
    int key = e->key();
    spkb_last.keysym = key;
    spkb_last.shifted = key;
    spkb_last.modif = 0;
    spkb_last.index = key;

    spkb_kbstate[key].state = 1;
    spkb_kbstate[key].press = 1234;
    spkb_kbstate[key].frame = sp_int_ctr;

    spkb_state_changed = 1;
    process_keys();
}

void QSpectemu::keyReleaseEvent(QKeyEvent *e)
{
    int key = e->key();
    spkb_kbstate[key].state = 0;
    spkb_kbstate[key].press = 0;

    spkb_state_changed = 1;
    process_keys();
}

//void QSpectemu::okClicked()
//{
//    if(spkb_kbstate[97].state == 0)
//    {
//        spkb_last.keysym = 97;
//        spkb_last.shifted = 65;
//        spkb_last.modif = 0;
//        spkb_last.index = 97;
//
//        spkb_kbstate[97].state = 1;
//        spkb_kbstate[97].press = 1234;
//        spkb_kbstate[97].frame = sp_int_ctr;
//    }
//    else
//    {
//        spkb_kbstate[97].state = 0;
//        spkb_kbstate[97].press = 0;
//    }
//    spkb_state_changed = 1;
//    process_keys();
//}

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
    qspectemu->update();
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
