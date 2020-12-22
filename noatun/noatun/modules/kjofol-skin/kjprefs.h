#ifndef KJPREFS_H
#define KJPREFS_H

#include <qwidget.h>
#include <noatunpref.h>

class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QComboBox;
class QLabel;
class QPushButton;
class KJLoader;

class KJPrefs : public CModule
{ 
Q_OBJECT
public:
	KJPrefs(QObject* parent);
	virtual void save();
	virtual void reopen();

public slots:
	void add();
	void showPreview(const QString &skin);	

private:
	QComboBox *mSkins;
	QPushButton *mAdd;
	QLabel *mPreview;
	QPixmap mPixmap;
};

/**
 * resolve a filename to it's correct case.
 * badNodes is the amount of directories/files (at the end)
 * that aren't known)
 **/
QString filenameNoCase(const QString &filename, int badNodes=1);

#endif // FORM1_H


