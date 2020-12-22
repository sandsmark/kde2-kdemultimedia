#ifndef __MENUMAKER_H__
#define __MENUMAKER_H__

#include <qpopupmenu.h>
#include <qobject.h>
#include <string>
#include <list>

class MenuCategory
{
protected:
	QPopupMenu *_menu;
	std::string _name;
	std::list<std::string> prefixList;
	bool _catchall;

public:
	MenuCategory(const char *name, const char *prefix,QPopupMenu *menu);

	void addPrefix(const char *prefix);
	const char *name();
	QPopupMenu *menu();

	bool catchall();
	bool matches(const char *item);
};

class MenuMaker :public QObject
{
	Q_OBJECT

	std::list<MenuCategory *> categories;
public:
	enum { CAT_MAGIC_ID = 10000 };

	MenuMaker(QPopupMenu *root);

	void addCategory(const char *name, const char *prefix);
	MenuCategory *lookupCategoryByName(const char *name);
	void addItem(const char *name, int i);
	std::string basename(const char *name);
	std::string catname(const char *name);

	void clear();

	const char *findID(int id);

protected slots:
	void menuactivated(int);

signals:
	void activated(int);
};
#endif
