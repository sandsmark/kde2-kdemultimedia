#ifndef FIND_H
#define FIND_H

#include <kdialog.h>

class KLineEdit;
class QCheckBox;
class QPushButton;

class Finder : public KDialog
{
Q_OBJECT
public:
	Finder(QWidget *parent);

	bool regexp() const;
	bool isForward() const;

	QString string() const;
signals:
	void search(Finder *);

public slots:
	void clicked();
	
private:
	KLineEdit *mText;
	QCheckBox *mRegexp, *mBackwards;
	QPushButton *mStart;
};

#endif

