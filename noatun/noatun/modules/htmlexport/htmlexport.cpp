#include <klocale.h>

#include "htmlexport.h"

extern "C"
{
	Plugin *create_plugin()
	{
		return new HTMLExport();
	}
}


HTMLExport::HTMLExport(): QObject(0, "HTMLExport"), Plugin()
{
	NOATUNPLUGINC(HTMLExport);

	menuID = napp->pluginMenuAdd(i18n("&Export Playlist"), this, SLOT(slotExport()));
	new Prefs(this);
	config = KGlobal::config();
}

HTMLExport::~HTMLExport()
{
	napp->pluginMenuRemove(menuID);
}

void HTMLExport::slotExport()
{

	// init readConfig
	config->setGroup("HTMLExport");

	// get output target
	KURL url = KFileDialog::getSaveURL(QString::null,
	                                   i18n("*.html"),
	                                   0,
	                                   i18n("Export Playlist"));

	// write into tempfile
	KTempFile temp;
	temp.setAutoDelete(true);
	QFile file(temp.name());
	file.open(IO_WriteOnly);
	QTextStream str(&file);

	str << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\"" << endl;
	str << "					  \"http://www.w3.org/TR/REC-html40/loose.dtd\">" << endl;
	str << "<html>" << endl;
	str << "<head>" << endl;
	str << "\t<title>Noatun Playlist</title>" << endl;

	if ( config->readBoolEntry( "useCSS" ) == true )
	{
		str << "\t<style type=\"text/css\">" << endl;
		str << "\t<!--" << endl;
		str << "\tp,body,th,td { color:#"<< getColorByEntry("txtColor")<<" }" << endl;
		str << "\ta { color:#"<< getColorByEntry("lnkColor")<<" }" << endl;

		if ( config->readBoolEntry( "useHover" ) == true )
			str << "\ta:hover { color:#"<< getColorByEntry("hoverColor")<<" }"<< endl;

		str << "\t-->" << endl;
		str << "\t</style>" << endl;
	}

	str << "</head>" << endl;
	str << "<body bgcolor=\"" << getColorByEntry( "bgColor" )
		<< "\" background=\"" << config->readEntry( "bgImgPath" )
		<<"\">" << endl;
	str << "\t<h1 align=\"center\">Noatun Playlist</h1>" << endl;
	str << "\t<h2 align=\"center\">Experimental</h2>" << endl;
	str << "\t<table align=\"center\">" << endl;
	str << "\t\t<tr><th>Title</th></tr><tr><td><ol>" << endl;

	for (PlaylistItem *item = napp->playlist()->getFirst();
		item;
		item = napp->playlist()->getAfter(item))
		str << "\t\t<li><a href=\"" << item->file() << "\">" << item->title() << "</a>" << endl;

	str << "\t</ol></td></tr></table>" << endl;
	str << "</body>" << endl;
	str << "</html>" << endl;

	file.close();
	// tempfile -> userdefined file
	KIO::NetAccess::upload(temp.name(), url);
}

QString HTMLExport::getColorByEntry(QString s)
{
   QString res;
   QString tmp;
   QColor c;

   // init readConfig

   config->setGroup("HTMLExport");

   c = config->readColorEntry( s );
   tmp = QString::number( c.red(), 16);
   if (tmp.length()==1) tmp="0"+tmp;
   res = tmp;

   tmp = QString::number( c.green(), 16);
   if (tmp.length()==1) tmp="0"+tmp;
   res += tmp;

   tmp = QString::number( c.blue(), 16);
   if (tmp.length()==1) tmp="0"+tmp;
   res += tmp;

   return res;

}
//////////////////////////////////// Settings ////////////////////////////////////

Prefs::Prefs(QObject *parent)
	: CModule(i18n("Playlist Export"), i18n("Colors and settings for HTML Export"), parent)
{

	// Init Config
	KConfig *config = KGlobal::config();
	config->setGroup("HTMLExport");

	// Set default values
	if ( !config->hasKey( "lnkColor" ) )
	config->writeEntry( "lnkColor", QColor( blue ) ) ;

	if ( !config->hasKey( "hoverColor" ) )
	config->writeEntry( "hoverColor", QColor( red ) );

	if ( !config->hasKey( "bgColor" ) )
	config->writeEntry( "bgColor", QColor( white ) ) ;

	if ( !config->hasKey( "txtColor" ) )
	config->writeEntry( "txtColor", QColor( black ) );

	config->sync();

	// Draw Stuff and insert Settings
	QVBoxLayout *top = new QVBoxLayout(this, KDialog::marginHint(),
						KDialog::spacingHint() );

	colorBox = new QGroupBox( i18n( "HTML Color Settings"  ), this, "colorBox" );

	bgcolorLabel = new QGridLayout( colorBox, 5, 5,
					KDialog::marginHint(), KDialog::spacingHint() );

	lnkColorSelect = new KColorButton( colorBox, "lnkColorSelect" );
	lnkColorSelect->setColor(config->readColorEntry( "lnkColor" ) );

	hoverColorSelect = new KColorButton( colorBox, "hoverColorSelect" );
	hoverColorSelect->setColor( config->readColorEntry( "hoverColor" ) );

	bgColorSelect = new KColorButton( colorBox, "bgColorSelect" );
	bgColorSelect->setColor( config->readColorEntry( "bgColor" ) );

	txtColorSelect = new KColorButton( colorBox, "txtColorSelect" );
	txtColorSelect->setColor( config->readColorEntry( "txtColor" ) );

	txtColorLabel = new QLabel( colorBox, "txtColorLabel" );
	txtColorLabel->setText( i18n( "Text Color:"  ) );
	txtColorLabel->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

	bgColorLabel = new QLabel( colorBox, "bgColorLabel" );
	bgColorLabel->setText( i18n( "Background:"  ) );
	bgColorLabel->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

	lnkColorLabel = new QLabel( colorBox, "lnkColorLabel" );
	lnkColorLabel->setText( i18n( "Link Color:"  ) );
	lnkColorLabel->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

	hoverColorLabel = new QLabel( colorBox, "hoverColorLabel" );
	hoverColorLabel->setText( i18n( "Link Hover Color:" ) );
	hoverColorLabel->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

	// ### Is there any way to to this clean?
	// anyway, much cleaner than the previous way using
	// the proxy widget and hardcoded sizes...
	bgcolorLabel->setRowStretch(0, true);
	bgcolorLabel->addWidget( txtColorLabel, 0, 0 );
	bgcolorLabel->addWidget( txtColorSelect, 0, 1 );
	bgcolorLabel->addWidget( lnkColorLabel, 1, 0 );
	bgcolorLabel->addWidget( lnkColorSelect, 1, 1 );
	bgcolorLabel->addWidget( bgColorLabel, 0, 2 );
	bgcolorLabel->addWidget( bgColorSelect, 0, 3 );
	bgcolorLabel->addWidget( hoverColorLabel, 1, 2 );
	bgcolorLabel->addWidget( hoverColorSelect, 1, 3 );

	bgPicBox = new QHGroupBox( i18n( "Background Image"), this, "bgPicBox" );

	bgPicPath = new KURLRequester ( bgPicBox, "bgPicPath" );
	bgPicPath->setURL( config->readEntry( "bgImgPath" ) );

	useCSS = new QCheckBox( this, "useCSS" );
	useCSS->setText( i18n( "&Use CSS 1"  ) );
	useCSS->setChecked( config->readBoolEntry( "useCSS" ) );
	useCSS->setTristate( false );
	useCSS->hide(); // we will _always_ do CSS atm
	QWhatsThis::add( useCSS, i18n( "Use <b>CSS Level 1</b> description for color and style data rather then the deprecated <i>&lt;font&gt;</i> tag in HTML output." ) );

	useHover = new QCheckBox( this, "useCSS" );
	useHover->setText( i18n( "&Enable Hover"  ) );
	useHover->setChecked( config->readBoolEntry( "useHover" ) );
	useHover->setTristate( false );
	QWhatsThis::add( useCSS, i18n( "This color will appear if you move your mouse over a link in a CSS enabled browser." ) );

	top->addWidget( colorBox );
	top->addWidget( bgPicBox );
	top->addWidget( useCSS );
	top->addWidget( useHover );
	top->addStretch();

	connect( useCSS, SIGNAL( toggled( bool ) ), useHover, SLOT ( setDisabled( bool ) ) );
	connect( useCSS, SIGNAL( toggled( bool ) ), hoverColorSelect, SLOT ( setDisabled( bool ) ) );
	connect( useHover, SIGNAL( toggled( bool ) ), hoverColorSelect, SLOT ( setDisabled( bool ) ) );
}

void Prefs::save()
{

	KConfig *config = KGlobal::config();
	config->setGroup( "HTMLExport" );
	config->writeEntry( "bgColor", bgColorSelect->color() );
	config->writeEntry( "txtColor", txtColorSelect->color() );
	config->writeEntry( "lnkColor", lnkColorSelect->color() );
	config->writeEntry( "bgImgPath", bgPicPath->url() );
	config->writeEntry( "useCSS", useCSS->isChecked() );
	config->writeEntry( "useHover", useHover->isChecked() );
	config->sync();
}

#include "htmlexport.moc"

