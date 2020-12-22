#ifndef EDITOR_H
#define EDITOR_H

#include <kdialog.h>

class KLineEdit;
class QLabel;
class NID3;
class QComboBox;

class Editor : public QWidget
{
Q_OBJECT
public:
	Editor(NID3 *id);
	QString name() const;
	QString artist() const;
	QString album() const;
	QString year() const;
	QString genre() const;
	QString comment() const;
	

signals:
	void saved();

public slots:
	void open(const QString &file);
	void show();

protected slots:
	void save();
	void textChanged(const QString&dummy);

protected:
	virtual void hideEvent(QHideEvent*);

private:
	KLineEdit *mName, *mArtist, *mAlbum, *mYear, *mComment;
	QComboBox *mGenre;
	QLabel *mFile;
	NID3 *mID3Loader;
	QPushButton *mSaveButton;
};

#endif

