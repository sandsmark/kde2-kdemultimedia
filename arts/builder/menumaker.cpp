#include "menumaker.h"
#include <stdio.h>
#include <assert.h>
#include <arts/debug.h>

using namespace std;

MenuCategory::MenuCategory(const char *name, const char *prefix,QPopupMenu *menu)
{
	_menu = menu;
	_name = name;
	_catchall = (strcmp(prefix,"*") == 0);
	addPrefix(prefix);
}

const char *MenuCategory::name()
{
	return _name.c_str();
}

bool MenuCategory::catchall()
{
	return _catchall;
}

void MenuCategory::addPrefix(const char *prefix)
{
	prefixList.push_back(prefix);
}

bool MenuCategory::matches(const char *item)
{
	string pattern = string(item)+"$";
	size_t patternlen = strlen(pattern.c_str());

	list<string>::iterator i;

	for(i=prefixList.begin();i != prefixList.end(); i++)
	{
		const char *prefix = (*i).c_str();

		if(patternlen >= strlen(prefix))
		{
			if(strncmp(prefix,pattern.c_str(),strlen(prefix)) == 0) return true;
		}
	}
	return false;
}

QPopupMenu *MenuCategory::menu()
{
	return _menu;
}

MenuMaker::MenuMaker(QPopupMenu *root)
{
	categories.push_back(new MenuCategory("","",root));
}

// Add more specific categories later, more general categories first.
//
// for instance use
//  addCategory("&Synthesis", "Synth_");
//  addCategory("&Synthesis/&Waveforms", "Synth_WAVE");

void MenuMaker::addCategory(const char *name, const char *prefix)
{
	MenuCategory *mc = 0,*pc = 0;
	
	mc = lookupCategoryByName(name);
	if(mc)
	{
		mc->addPrefix(prefix);
		return; // already exists
	}

	pc = lookupCategoryByName(basename(name).c_str());
	if(pc)
	{
		QPopupMenu *newMenu = new QPopupMenu;
		connect(newMenu,SIGNAL(activated(int)),this,SLOT(menuactivated(int)));
		pc->menu()->insertItem(catname(name).c_str(), newMenu, CAT_MAGIC_ID);
		arts_debug("inserting a menu called '%s' in the parent menu '%s'",catname(name).c_str(),pc->name());
		categories.push_back(new MenuCategory(name,prefix,newMenu));
	}
	else
	{
		arts_debug("Parent category '%s' for '%s' is missing.",basename(name).c_str(),name);
	}
}

MenuCategory *MenuMaker::lookupCategoryByName(const char *name)
{
	MenuCategory *mc = 0;
	list<MenuCategory *>::iterator i;
	for(i=categories.begin();i != categories.end();i++)
	{
		if(strcmp((*i)->name(),name) == 0) mc = (*i);
	}
	return mc;
}

void MenuMaker::addItem(const char *name, int index)
{
	MenuCategory *mc = 0;

	list<MenuCategory *>::iterator i;
	for(i=categories.begin();i != categories.end();i++)
		if((*i)->matches(name)) mc = (*i);
	assert(mc);		// root category should always match

	if(!strlen(mc->name()))
	{
		// if we hit the root category, it may be better to move the thing
		// into the catchall category (looks cleaner)
		for(i=categories.begin();i != categories.end();i++)
			if((*i)->catchall()) mc = (*i);
	}
	mc->menu()->insertItem(name,index);
}

string MenuMaker::basename(const char *name)
{
	char *x = strdup(name);
	int i;

	i=strlen(x)-1;
	while(i>=0)
	{
		if(x[i] == '/')
		{
			x[i]=0;
			i = -2;
		}
		else i--;
	}
	if(i != -2) x[0] = 0; // no slash means empty parent category
	string result(x);
	free(x);
	arts_debug("basename(%s) => %s",name,result.c_str());
	return result;
}

string MenuMaker::catname(const char *name)
{
	int i;
	for(i=strlen(name)-1;i>=0;i--)
		if(name[i] == '/') return string(&name[i+1]);

	return string(name);
}

void MenuMaker::clear()
{
	list<MenuCategory *>::iterator i;
	for(i=categories.begin();i != categories.end();i++)
	{
		QPopupMenu *m = (*i)->menu();
		unsigned int k;

		k = 0;
		while(k<m->count())
		{
			if(m->idAt(k) != CAT_MAGIC_ID)
			{
				m->removeItemAt(k);
				k = 0;
			}
			else
			{
				k++;
			}
		}
	}
}

const char *MenuMaker::findID(int id)
{
	list<MenuCategory *>::iterator i;
	for(i=categories.begin();i != categories.end();i++)
	{
		const char *name = (*i)->menu()->text(id);
		if(name) return(name);
	}
	return 0;
}

void MenuMaker::menuactivated(int i)
{
	emit activated(i);
}

#include "menumaker.moc"
