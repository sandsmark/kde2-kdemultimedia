#ifndef PRINCIPAL_H
#define PRINCIPAL_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kmainwindow.h>
#include <capture.h>

class QToolButton;
class QFrame;
class QSlider;
class QTimer;
class QPopupMenu;
class QSlider;
class QLabel;
class QWidget;
class QStrList;

class KXAnim;

class KConfig;
class KIconLoader;
class KWin;

class aktionVm;
class capture;

#define NOTHING           0
#define CLOSE_THE_APP     1
#define NEW_VIDEO         2
#define CONTINUE_LOADING  3
#define CHANGE_EXECUTABLE 4

class Principal : public KMainWindow
{
	 Q_OBJECT
private:
	QToolButton *tocar;
	QToolButton *parar;
	QToolButton *abrir;
	QToolButton *avanzar;
	QToolButton *retroceder;
        QToolButton *ayuda;
        QToolButton *configB;
        QFrame *line;
        QLabel *logoWidget;
        KXAnim *video;
        QWidget *videoContainer;
        KIconLoader *loader;
        QString lastDir;
        QString fileName;
        QString saveCaption;
        int whatToDo;
        int menuItemFullScreen;
        KConfig *config;
        void changeSize(float);
        void changeInitialSize();
        void setParameters();
        void continueLoading();
        void executableChanged();
        void toggleButtons(bool);
        bool inFullScreen;
        bool parametersChanged;
        bool isNewVideo;
        QPopupMenu *menu;
        QSlider *volumeSlider;
        bool externalChange;
        QTimer *timer;
        QColor saveBackground;
    //        QRect oldGeometry;
        capture theCapturer;
#ifdef XF86_VM
        aktionVm *videoMode;
        bool inVmMode;
#endif
public:
	Principal( const char *name=0);
        virtual ~Principal();
        void loadFile(const char *);
protected:
        virtual bool queryExit();
        virtual void keyPressEvent( QKeyEvent *);
        virtual void resizeEvent( QResizeEvent *);
        virtual void dragEnterEvent( QDragEnterEvent *);
        virtual void dropEvent( QDropEvent *);
signals:
public slots:
        void click_open();
        void click_play();
        void click_stop();
        void click_forward();
        void click_backward();
        void click_ayuda();
        void click_config();
        void click_info();
        void click_original();
        void click_half();
        void click_double();
        void waitForKXanimExit();
        void receiveMouseClick( QMouseEvent *);
        void volumeSliderMoved(int);
        void maximize();
        void goFullScreen();
        void undoFullScreen();
        void checkStates();
};
#endif
