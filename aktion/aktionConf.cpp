#include "aktionConf.h"

#include <klocale.h>
#include <kconfig.h>
#include <kapp.h>
#include <kfiledialog.h>
#include <kmessagebox.h>

#include <qimage.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qwidget.h>
#include <qgroupbox.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qlistbox.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qlabel.h>

Setup::Setup( QWidget *parent, const char *name )
    : QTabDialog( parent, name, true )
{
    setCaption( i18n("aKtion! Setup" ));

    config = kapp->config();

    addTab( audio(), i18n("Audio" ));
    addTab( color(), i18n("Color") );
    addTab( scaling(), i18n("Scaling" ));
    addTab( capture(), i18n("Capture" ));
    addTab( others(), i18n("Others" ));

    setCancelButton( i18n("&Cancel") );
    connect( this, SIGNAL(cancelButtonPressed()), SLOT(quit()) );
    
    setOkButton( i18n("&OK" ));
    connect( this, SIGNAL(applyButtonPressed()), SLOT(save()) );
}

QWidget *Setup::audio()
{
    QWidget *base = new QWidget( this, "Audio" );
    config->setGroup("audio");

    QBoxLayout *l = new QVBoxLayout(base, 10, 0);

    checkBox[0] = new QCheckBox( base );
    checkBox[0]->setText( i18n("Audio enable") );
    checkBox[0]->setMinimumSize( checkBox[0]->sizeHint() );
    checkBox[0]->setChecked(config->readBoolEntry("enable"));
    l->addWidget( checkBox[0] );

    checkBox[1] = new QCheckBox( base );
    checkBox[1]->setText( i18n("Skip frames for audio sync") );
    checkBox[1]->setMinimumSize( checkBox[1]->sizeHint() );
    checkBox[1]->setChecked(config->readBoolEntry("audioSync"));
    l->addWidget( checkBox[1] );

    checkBox[8] = new QCheckBox( base );
    checkBox[8]->setText( i18n("Show volume slider") );
    checkBox[8]->setMinimumSize( checkBox[8]->sizeHint() );
    checkBox[8]->setChecked(config->readBoolEntry("showVolumeSlider"));
    l->addWidget( checkBox[8] );

    label[0] = new QLabel( base );
    label[0]->setText( i18n("Initial volume (0-100):" ));
    label[0]->setMinimumSize( label[0]->sizeHint() );
    l->addWidget( label[0] );

    spin[0] = new QSpinBox( base );
    spin[0]->setRange(0,100);
    spin[0]->setValue(config->readNumEntry("audioInitialVolume"));
    spin[0]->setMinimumSize( spin[0]->sizeHint() );
    spin[0]->setFixedWidth(50);
    QBoxLayout *l1 = new QHBoxLayout();
    l->addLayout(l1);
    l1->addWidget( spin[0] );
    l1->addStretch(1);

    l->addStretch(1);

    l->activate();
    return base;
}

QWidget *Setup::color()
{
    QWidget *base = new QWidget( this, "Color" );
    config->setGroup("color");

    QBoxLayout *l = new QVBoxLayout(base,10,0);

    grupo[0] = new QGroupBox(base);
    grupo[0]->setTitle( i18n("Color mapping (non TrueColor displays)"));

    QBoxLayout *l1 = new QVBoxLayout(grupo[0],10,0);

    botonRadio[0] = new QRadioButton( grupo[0] );
    botonRadio[0]->setText( i18n("TrueColor to 322 (static color)") );
    botonRadio[0]->setMinimumSize( botonRadio[0]->sizeHint() );
    l1->addSpacing(10);
    l1->addWidget(botonRadio[0]);

    botonRadio[1] = new QRadioButton( grupo[0] );
    botonRadio[1]->setText( i18n("Color lookup table") );
    botonRadio[1]->setMinimumSize( botonRadio[1]->sizeHint() );
    l1->addWidget(botonRadio[1]);

    label[1] = new QLabel( grupo[0] );
    label[1]->setText( i18n("Numer of frames to look ahead:" ));
    label[1]->setMinimumSize(label[1]->sizeHint());

    lined[0] = new QLineEdit( grupo[0] );
    lined[0]->setText(config->readEntry("colorAhead"));
    lined[0]->setMinimumSize( lined[0]->sizeHint() );
    lined[0]->setFixedWidth( 40 );

    QBoxLayout *l2 = new QHBoxLayout();
    l1->addLayout(l2);
    l2->addSpacing(20);
    l2->addWidget( label[1] );
    l2->addSpacing(10);
    l2->addWidget( lined[0] );
    l2->addStretch(1);

    botonRadio[2] = new QRadioButton( grupo[0] );
    botonRadio[2]->setText( i18n("TrueColor to grayscale") );
    botonRadio[2]->setMinimumSize(botonRadio[2]->sizeHint());
    l1->addWidget(botonRadio[2]);

    botonRadio[3] = new QRadioButton( grupo[0] );
    botonRadio[3]->setText( i18n("None") );
    botonRadio[3]->setMinimumSize(botonRadio[3]->sizeHint());
    l1->addWidget(botonRadio[3]);

    botones[0] = new QButtonGroup( base );
    botones[0]->hide();
    botones[0]->insert(botonRadio[0]);
    botones[0]->insert(botonRadio[1]);
    botones[0]->insert(botonRadio[2]);
    botones[0]->insert(botonRadio[3]);
//    botones[0]->setButton(config->readNumEntry("colorMapping"));
    /* patch for Qt 1.32 */
    botonRadio[config->readNumEntry("colorMapping")]->setChecked(true);
    connect(botones[0], SIGNAL(clicked(int)), SLOT(actualizeDialog1(int)) );
    if (config->readNumEntry("colorMapping")!=1)
    {
       label[1]->setEnabled(false);
       lined[0]->setEnabled(false);
    }

    l->addWidget(grupo[0]);
    l->addSpacing(10);

    label[2] = new QLabel( base );
    label[2]->setText( i18n("Gamma of display:" ));
    label[2]->setMinimumSize(label[2]->sizeHint());
    l->addWidget(label[2]);

    lined[1] = new QLineEdit( base );
    lined[1]->setText(config->readEntry("gammaDisp"));
    lined[1]->setMinimumSize(lined[1]->sizeHint());
    l->addWidget(lined[1]);

    l->addStretch(1);

    l->activate();
    return base;
}

QWidget *Setup::scaling()
{
    QWidget *base = new QWidget( this, "Scaling" );
    config->setGroup("scaling");

    QBoxLayout *l = new QVBoxLayout(base, 10);

    label[3] = new QLabel( base );
    label[3]->setText( i18n("Display scale" ));
    label[3]->setMinimumSize( label[3]->sizeHint() );
    l->addWidget(label[3]);

    combo[0] = new QComboBox( base );
    combo[0]->insertItem( i18n("Original size") );
    combo[0]->insertItem( i18n("Half size") );
    combo[0]->insertItem( i18n("Double size") );
    combo[0]->insertItem( i18n("Maximized") );
    combo[0]->insertItem( i18n("Full screen") );
    combo[0]->setCurrentItem(config->readNumEntry("scale"));
    combo[0]->setMinimumSize( combo[0]->sizeHint() );
    l->addWidget(combo[0]);

    botones[2] = new QButtonGroup( base );
    botones[2]->setTitle( i18n("Full screen") );

    QBoxLayout *l1 = new QVBoxLayout(botones[2], 10, 0);
    l1->addSpacing(10);

    checkBox[9] = new QCheckBox( botones[2] );
    checkBox[9]->setText( i18n("Use XFree86-VidModeExtensions (if available)") );
    checkBox[9]->setMinimumSize( checkBox[9]->sizeHint() );
    checkBox[9]->setChecked(config->readBoolEntry("usevm"));
    l1->addWidget( checkBox[9] );

    checkBox[10] = new QCheckBox( botones[2] );
    checkBox[10]->setText( i18n("Grab the mouse") );
    checkBox[10]->setMinimumSize( checkBox[10]->sizeHint() );
    checkBox[10]->setChecked(config->readBoolEntry("grabmouse"));
    l1->addWidget( checkBox[10] );

    l->addWidget( botones[2] );

    l->addStretch(1);

    l->activate();
    return base;
}

QWidget *Setup::capture()
{
    QWidget *base = new QWidget( this, "Capture" );
    config->setGroup("capture");

    QBoxLayout *l = new QVBoxLayout(base, 10);

    label[7] = new QLabel( base );
    label[7]->setText( i18n("Output file format:" ));
    label[7]->setMinimumSize( label[7]->sizeHint() );
    l->addWidget(label[7]);

    QStrList formats=QImage::outputFormats();
    combo[1] = new QComboBox( base );
    combo[1]->insertStrList( &formats );
    QString configFormat;
    int c=0;
    configFormat=config->readEntry("outputFormat");
    combo[1]->setCurrentItem(c);
    while ( combo[1]->currentText() != configFormat )
    {
       c++;
       combo[1]->setCurrentItem(c);
    }
    combo[1]->setMinimumSize( combo[1]->sizeHint() );
    l->addWidget(combo[1]);

    label[8] = new QLabel( base );
    label[8]->setText( i18n("Output file directory:" ));
    label[8]->setMinimumSize( label[8]->sizeHint() );
    l->addWidget(label[8]);

    QBoxLayout *l2 = new QHBoxLayout();
    l->addLayout(l2);

    lined[5] = new QLineEdit( base );
    lined[5]->setText( config->readEntry("outputDir"));
    lined[5]->setMinimumSize( lined[5]->sizeHint() );
    l2->addWidget( lined[5] );

    dirButton[1] = new QPushButton( base );
    dirButton[1]->setText("...");
    dirButton[1]->setFixedSize( dirButton[1]->sizeHint() );
    l2->addWidget( dirButton[1] );
    connect( dirButton[1], SIGNAL(clicked()), SLOT(getCaptureDir()) );

    label[9] = new QLabel( base );
    label[9]->setText( i18n("Press 'c' during playback to capture a single frame." ));
    label[9]->setMinimumSize( label[9]->sizeHint() );
    l->addWidget(label[9]);

    l->addStretch(1);

    l->activate();
    return base;
}

QWidget *Setup::others()
{
    QWidget *base = new QWidget( this, "Others" );
    config->setGroup("others");

    QBoxLayout *l = new QVBoxLayout(base, 10, 0);

    botones[1] = new QButtonGroup( base );
    botones[1]->setTitle( i18n("Video loading") );
    //botones[1]->setMinimumSize(10,10,max,90);

    QBoxLayout *l1 = new QVBoxLayout(botones[1], 10, 0);
    l1->addSpacing(10);

    botonRadio[4] = new QRadioButton( botones[1] );
    botonRadio[4]->setText( i18n("Preload animation into memory") );
    botonRadio[4]->setMinimumSize( botonRadio[4]->sizeHint() );
    l1->addWidget( botonRadio[4] );

    botonRadio[5] = new QRadioButton( botones[1] );
    botonRadio[5]->setText( i18n("Preload and uncompress animation") );
    botonRadio[5]->setMinimumSize( botonRadio[5]->sizeHint() );
    l1->addWidget( botonRadio[5] );

    botonRadio[6] = new QRadioButton( botones[1] );
    botonRadio[6]->setText( i18n("Read each sector only when needed") );
    botonRadio[6]->setMinimumSize( botonRadio[6]->sizeHint() );
    l1->addWidget( botonRadio[6] );

//  botones[1]->setButton( config->readNumEntry("loading") );
    /* patch for Qt 1.32 */
    botonRadio[config->readNumEntry("loading")+4]->setChecked(true);
    connect(botones[1], SIGNAL(clicked(int)), SLOT(actualizeDialog(int)) );

    l->addWidget( botones[1] );
    l->addSpacing(10);

    checkBox[3] = new QCheckBox( base );
    checkBox[3]->setText( i18n("Use X11 shared memory") );
    checkBox[3]->setMinimumSize( checkBox[3]->sizeHint() );
    checkBox[3]->setChecked(config->readBoolEntry("x11shared"));
    if (config->readNumEntry("loading")==1)
       checkBox[3]->setEnabled(false);
    l->addWidget( checkBox[3] );

    checkBox[4] = new QCheckBox( base );
    checkBox[4]->setText( i18n("Use X11 multi buffering") );
    checkBox[4]->setMinimumSize( checkBox[4]->sizeHint() );
    checkBox[4]->setChecked(config->readBoolEntry("multiBuffer"));
    l->addWidget( checkBox[4] );

    checkBox[5] = new QCheckBox( base );
    checkBox[5]->setText( i18n("Use Pixmap instead of Image in X11") );
    checkBox[5]->setMinimumSize( checkBox[5]->sizeHint() );
    checkBox[5]->setChecked(config->readBoolEntry("pixmap"));
    l->addWidget( checkBox[5] );
/*
    label[7] = new QLabel( base );
    label[7]->setMinimumSize(10,90,max,20);
    label[7]->setText( i18n("X11 Visual of class:" ));
    combo[0] = new QComboBox( base );
    combo[0]->setMinimumSize(10,110,150,20);
    combo[0]->insertItem("(default)");
    combo[0]->insertItem("Static gray");
    combo[0]->insertItem("Gray scale");
    combo[0]->insertItem("Static color");
    combo[0]->insertItem("Pseudo color");
    combo[0]->insertItem("True color");
    combo[0]->insertItem("Direct color");
    combo[0]->setCurrentItem(config->readNumEntry("visualClass"));
*/
    label[4] = new QLabel( base );
    label[4]->setText( i18n("Initial directory:" ));
    label[4]->setMinimumSize( label[4]->sizeHint() );
    l->addWidget( label[4] );

    QBoxLayout *l2 = new QHBoxLayout();
    l->addLayout(l2);

    lined[3] = new QLineEdit( base );
    lined[3]->setText( config->readEntry("initialDirectory"));
    lined[3]->setMinimumSize( lined[3]->sizeHint() );
    l2->addWidget( lined[3] );

    dirButton[0] = new QPushButton( base );
    dirButton[0]->setText("...");
    dirButton[0]->setFixedSize( dirButton[0]->sizeHint() );
    l2->addWidget( dirButton[0] );
    connect( dirButton[0], SIGNAL(clicked()), SLOT(getDir()) );

    checkBox[6] = new QCheckBox( base );
    checkBox[6]->setText( i18n("Loop animation") );
    checkBox[6]->setMinimumSize( checkBox[6]->sizeHint() );
    checkBox[6]->setChecked(config->readBoolEntry("loop"));
    l->addWidget( checkBox[6] );

    checkBox[7] = new QCheckBox( base );
    checkBox[7]->setText( i18n("Automatically start playing video") );
    checkBox[7]->setMinimumSize( checkBox[7]->sizeHint() );
    checkBox[7]->setChecked(config->readBoolEntry("autoplay"));
    l->addWidget( checkBox[7] );

    label[5] = new QLabel( base );
    label[5]->setText( i18n("Extra XAnim parameters:" ));
    label[5]->setMinimumSize( label[5]->sizeHint() );
    l->addWidget( label[5] );

    lined[2] = new QLineEdit( base );
    lined[2]->setText( config->readEntry("extras"));
    lined[2]->setMinimumSize( lined[2]->sizeHint() );
    l->addWidget( lined[2] );

    label[6] = new QLabel( base );
    label[6]->setText( i18n("xanim executable:" ));
    label[6]->setMinimumSize( label[6]->sizeHint() );
    l->addWidget( label[6] );

    QBoxLayout *l3 = new QHBoxLayout();
    l->addLayout(l3);

    lined[4] = new QLineEdit( base );
    lined[4]->setText( config->readEntry("executable"));
    lined[4]->setMinimumSize( lined[4]->sizeHint() );
    l3->addWidget( lined[4] );

    executableButton = new QPushButton( base );
    executableButton->setText("...");
    executableButton->setFixedSize( executableButton->sizeHint() );
    l3->addWidget( executableButton );
    connect( executableButton, SIGNAL(clicked()), SLOT(getXanimExecutable()) );

    l->activate();
    return base;
}
void Setup::quit()
{
    done(0);
}

void Setup::save()
{
    config->setGroup("audio");
    config->writeEntry("enable",checkBox[0]->isChecked() );
    config->writeEntry("audioSync",checkBox[1]->isChecked() );
    config->writeEntry("showVolumeSlider",checkBox[8]->isChecked() );
    config->writeEntry("audioInitialVolume",spin[0]->value() );

    config->setGroup("color");
    config->writeEntry("colorAhead", lined[0]->text() );
    if (botonRadio[0]->isChecked()) config->writeEntry("colorMapping", 0);
    if (botonRadio[1]->isChecked()) config->writeEntry("colorMapping", 1);
    if (botonRadio[2]->isChecked()) config->writeEntry("colorMapping", 2);
    if (botonRadio[3]->isChecked()) config->writeEntry("colorMapping", 3);
    config->writeEntry("gammaDisp",lined[1]->text() );

    config->setGroup("scaling");
    config->writeEntry("scale",combo[0]->currentItem() );
    config->writeEntry("usevm",checkBox[9]->isChecked() );
    config->writeEntry("grabmouse",checkBox[10]->isChecked() );

    config->setGroup("capture");
    config->writeEntry("outputFormat",combo[1]->currentText() );
    config->writeEntry("outputDir",lined[5]->text() );

    config->setGroup("others");
    if (botonRadio[4]->isChecked()) config->writeEntry("loading", 0);
    if (botonRadio[5]->isChecked()) config->writeEntry("loading", 1);
    if (botonRadio[6]->isChecked()) config->writeEntry("loading", 2);
    config->writeEntry("x11shared",checkBox[3]->isChecked() );
    config->writeEntry("multiBuffer",checkBox[4]->isChecked() );
    config->writeEntry("pixmap",checkBox[5]->isChecked() );
//    config->writeEntry("visualClass",combo[0]->currentItem() );
    config->writeEntry("initialDirectory", lined[3]->text() );
    config->writeEntry("loop",checkBox[6]->isChecked() );
    config->writeEntry("extras",lined[2]->text());
    config->writeEntry("executable",lined[4]->text());
    config->writeEntry("autoplay",checkBox[7]->isChecked() );

    config->sync();
    done(1);
}

Setup::~Setup()
{
}

void Setup::getDir()
{
   QString s;
   
   s=KFileDialog::getExistingDirectory("",0L);
   if (!s.isEmpty())
      lined[3]->setText(s);
}

void Setup::getCaptureDir()
{
   QString s;
   
   s=KFileDialog::getExistingDirectory("",0L);
   if (!s.isEmpty())
      lined[5]->setText(s);
}

void Setup::getXanimExecutable()
{
   KURL url = KFileDialog::getOpenURL("","*|All files (*)",0L,i18n("xanim executable:" ));
   
   if( url.isEmpty() )
     return;
   
   if( !url.isLocalFile() )
   {
     KMessageBox::sorry( 0L, i18n( "Only local executables allowed." ) );
     return;
   }  
   
   lined[4]->setText(url.path());
}

void Setup::actualizeDialog(int rButton)
{
    if (rButton==1)
       checkBox[3]->setEnabled(false);
    else
       checkBox[3]->setEnabled(true);
}

void Setup::actualizeDialog1(int rButton)
{
    if (rButton==1)
    {
       label[1]->setEnabled(true);
       lined[0]->setEnabled(true);
    }
    else
    {
       label[1]->setEnabled(false);
       lined[0]->setEnabled(false);
    }
}

#include "aktionConf.moc"
