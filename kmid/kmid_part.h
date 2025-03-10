#ifndef __KMIDPART_H__
#define __KMIDPART_H__

#include <kparts/browserextension.h>
#include <klibloader.h>

class KAboutData;
class KInstance;
class KMidBrowserExtension;
class kmidClient;

class KMidFactory : public KLibFactory
{
  Q_OBJECT
  public:
    KMidFactory();
    virtual ~KMidFactory();

    virtual QObject* create(QObject* parent = 0, const char* name = 0,
                const char* classname = "QObject",
                const QStringList &args = QStringList());

    static KInstance *instance();

    static KAboutData *aboutData();
  private:
    static KInstance *s_instance;
};

class KMidPart: public KParts::ReadOnlyPart
{
  Q_OBJECT
  public:
    KMidPart(QWidget *parent, const char *name);
    virtual ~KMidPart();


    virtual bool closeURL();

  protected:
    virtual bool openFile();

  protected slots:
    void slotPlay();
    void slotStop();

  private:
    kmidClient *widget;
    KMidBrowserExtension *m_extension;
};

class KMidBrowserExtension : public KParts::BrowserExtension
{
  Q_OBJECT
    friend class KMidPart;
  public:
  KMidBrowserExtension(KMidPart *parent);
  virtual ~KMidBrowserExtension();
};

#endif
