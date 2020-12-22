#ifndef KANIM_H
#define KANIM_H

/* Library Includes */
#include <qwidget.h>
#include <kprocess.h>

/* Class Includes */
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#undef TrueColor
#undef Above
#undef Below

class QString;
class QSize;
class QSizePolicy;

/**
* Widget class for video playing
*
* This widget is a xanim wrapper to easily allow the video playing on your
* applications.
*
* @short A xanim wrapper to allow video playing on your apps.
* @author Guillermo P. Marotte (g-marotte@usa.net)
* @version 0.3
*/
class KXAnim : public QWidget
{
	 Q_OBJECT
public:
        /**
        *  Constructor. The parent argumet is NOT optional.
        */
	KXAnim(QWidget *parent, const char *name=0);
        ~KXAnim();
        /**
        * Use this method to tell the player the name of the file
        * to play. The file will not start to play until you call the
        * play() slot.
        * You need to call stop() before you can set a new filename.
        *
        * After call this method, you should wait for an @ref #stopped signal.
        * Just then, the information about the file will be available.
        *
        * Also check for @ref #getErrorNumber and @ref #getErrorString to know
        * if there was some error.
        */
	void setFile(QString file);
        /**
        * If val==true, the widget will resize by himself after the setFile()
        * to fit to the size of the video. If val==false, the video size will
        * be adjusted to fit to the widget size.
        * 
        * The size of the widget is automatically updated by @ref setFile
        * and @ref play. But if you need to know the effective size of the
        * widget *before* you call play, you need to call @ref updateSize
        * manually.
        *
        * Default is true
        */
        void setAutoResize(bool val) { autoResize=val; };
        /**
        * Returns the auto-resize state (see @ref #setAutoResize ).
        */
        bool getAutoResize() { return autoResize; };
        /**
        * Updates the widget size to fit video size (taking care of the
        * scale factors that applies). See @ref setAutoResize .
        */
        void updateSize();
        /**
        * If val==true, the video will loop forever.
        *
        * Default is true
        */
        void setLoop(bool val) { loop=val; };
        /**
        * Returns the loop state ( see @ref #setLoop ).
        */
        bool getLoop() { return loop; };
        /**
        * Returns the current video file name (or an empty string, if
        * there's no video yet).
        */
        QString getVideoFileName() { return fileName; };
        /**
        * Returns the video height (in pixels).
        */
        int getVideoHeight() { return videoHeight; };
        /**
        * Returns the video height (in pixels).
        */
        int getVideoWidth() { return videoWidth; };
        /**
        * Returns the video playback speed (in frames per second).
        */
        float getVideoSpeed() { return videoSpeed; };
        /**
        * Returns the number of frames in the video.
        */
        int getVideoFrames() { return videoFrames; };
        /**
        * Returns the string that describes the video codec.
        */
        QString getVideoCodec() { return videoCodec; };
        /**
        * Returns the the error code number. You should check this
        * number after a @ref #setFile . If the code is zero, the there were
        * no errors. To get a string containing a description see @ref #getErrorCode .
        */
        int getErrorCode() { return errorCode; };
        /**
        * Returns the string that describes the error during @ref #setFile .
        */
        QString getErrorString() { return errorString; };
        /**
        * Returns true is the video is in play mode. Otherwise it reurns
        * false.
        */
        bool isPlaying() { return playing; };
        /**
        * Returns true is the video is active. Otherwise it reurns
        * false. The widget is NOT Active only when it's stopped. Therefore,
        * you can use this method to check if the video is REALLY stopped.
        */
        bool isActive() { return active; };


        /**
        * This method reset all the configuration parameters to their
        * default values.
        */
        void resetXAnimDefaults();
        /* ================== SOUND ====================== */
        /**
        * If val==true, enables sound output.
        *
        * Default is true
        */
        void setAudio(bool val) { audio=val; };
        /**
        * If val==true, allows xanim skip video frames for audio sync.
        *
        * Default is true
        */
        void setAudioSync(bool val) { audioSync=val; };
        /**
        * Sets the initial audio volume.
        *
        * Default is 40
        */
        void setInitialVolume(int val) { audioInitialVolume=val; };


        /* ================== COLOR ====================== */
        /**
        * See @ref setColorMapping .
        */  
        enum colorMap { static332, lookupTable, grayScale, none };
        /**
        * Tells KXAnim wich type of color mappint to use for non truecolor
        * displays.
        *
        * If val==KXAnim::static322, xanim will convert truecolor anims 
        * to static 322.
        *
        * If val==KXAnim::lookupTable, xanim will sample the color of 
        * truecolor animations ahead of time and forms a color lookup table.
        *
        * If val==KXAnim::grayScale, xanim will convert TrueColor
        * animations to gray scale.
        *
        * Those three option are ignored for TrueColor and DirectColor
        * displays.
        *
        * if val==KXAnim::none, xanim will use the default color mapping.
        *
        * Default is KXAnim::none
        */
        void setColorMapping(colorMap val) { colorMapping=val; };
        /**
        * This sets the number of frames ahead to use for the look up table
        * when using KXAnim::lookupTable in @ref #setColorMapping .
        *
        * Default is 5
        */
        void setColorAhead(int val) { colorAhead=val; };

        /* ================== GAMMA ====================== */
        /**
        * Set gamma of display. Gamma's greater than 1.0 typically brighten
        * the animation.
        *
        * Default is 1.0
        */
        void setGammaDisplay(float val) { gammaDisplay=val; };


        /* ================== SCALING ====================== */
        /**
        * If val==true, the video size can be changed "on the fly".
        *
        * Default is true
        */
        void setResizing(bool val) { resizing=val; };
        /**
        * Scale the size of anim by "val" before displaying.
        *
        * Default is 1.0 (no scaling)
        */
        void setScaleFactor(float val) { scaleFactor=val; };
        /**
        * Scale the horizontal size of anim by "val" before displaying.
        *
        * Default is 1.0 (no scaling)
        */
        void setScaleHFactor(float val) { scaleHFactor=val; };
        /**
        * Scale the vertical size of anim by "val" before displaying.
        *
        * Default is 1.0 (no scaling)
        */
        void setScaleVFactor(float val) { scaleVFactor=val; };
        /**
        * Scale the animation to have width "val" before displaying.
        *
        * Default is 0 (no scaling)
        */
        void setScaleWidth(int val) { scaleWidth=val; };
        /**
        * Scale the animation to have height "val" before displaying.
        *
        * Default is 0 (no scaling)
        */
        void setScaleHeight(int val) { scaleHeight=val; };
        /**
        * Copy display scaling factors to buffering scaling factors.
        *
        * Default is false.
        */
        void setScaleDisplayToBuffer(bool val) { scaleToBuffer=val; };
        /**
        * Scale the size of anim by "val" before buffering it.
        *
        * Default is 1.0 (no scaling)
        */
        void setScaleFactorB(float val) { scaleFactorB=val; };
        /**
        * Scale the horizontal size of anim by "val" before buffering it.
        *
        * Default is 1.0 (no scaling)
        */
        void setScaleHFactorB(float val) { scaleHFactorB=val; };
        /**
        * Scale the vertical size of anim by "val" before buffering it.
        *
        * Default is 1.0 (no scaling)
        */
        void setScaleVFactorB(float val) { scaleVFactorB=val; };
        /**
        * Scale the animation to have width "val" before buffering it.
        *
        * Default is 0 (no scaling)
        */
        void setScaleWidthB(int val) { scaleWidthB=val; };
        /**
        * Scale the animation to have height "val" before buffering it.
        *
        * Default is 0 (no scaling)
        */
        void setScaleHeightB(int val) { scaleHeightB=val; };
        /**
        * Copy buffering scaling factors to display scaling factors.
        *
        * Default is false.
        */
        void setScaleBufferToDisplay(bool val) { scaleToDisplay=val; };

        /* ================== OTHERS ====================== */
        /**
        * 0: KXAnim will load the entire video into memory.
        *
        * 1: KXAnim will load and decompress the animation
        * entirely into memory before displaying.
        *
        * 2: KXAnim will read each sector only when needed
        *
        * Default is 2.
        */
        void setLoading(int val) { loading=val; };
        /**
        * If val==true, KXAnim will use X11 shared memory (this is
        * mutually exclusive with @ref setPreload .
        *
        * Default is false.
        */
        void setX11Shared(bool val) { x11Shared=val; };
        /**
        * If val==true, KXAnim will use X11 Buffering (if present) to
        * smooth animations by double-buffering.
        *
        * Default is true.
        */
        void setMultiBuffer(bool val) { multiBuffer=val; };
        /**
        * If val==true, KXAnim will use Pixmap instead of Image
        * in X11shared. This option has no effect if the animation
        * is buffered.
        *
        * Default is false.
        */
        void setUsePixmap(bool val) { usePixmap=val; };
        /**
        * Select the best "X11 Visual of Class..." when displaying the
        * animation. val can be anyone of the following strings and is
        * case insensitive:
        *
        * default
        * staticgray
        * grayscale
        * staticcolor
        * pseudocolor
        * truecolor
        * directcolor
        *
        * Default is "default".
        */
        void setX11VisualClass( const char *val) { x11VisualClass=val; };
        /**
        * This will pause XAnim at frame "val". val=-1 disables this
        * option.
        *
        * Default is -1 (no pause).
        */
        void setPauseAt(int val) { pauseAt=val; };
        /**
        * Use this method to pass any extra parametrs you want to
        * xanim (separated by spaces).
        */
        void setExtraParameters(const char *val) { extras=val; };
        /**
        * Use this method to set the executable name for xanim.
        * Dafault is "xanim", but you can change it, for instance,
        * if you don't have xanim in the path.
        */
        void setExecutable(const char *val) { executable=val; };

	QSize sizeHint() const;
	QSize minimumSizeHint() const;
	QSizePolicy sizePolicy() const;

signals:
        /**
        * This signal is emitted after a call to the stop() slot, and
        * means that the player has REALLY stop the playing (it is: that
        * xanim is inactive) (see @ref #stop ).
        */
        void stopped();
        /**
        * This signal is emitted when the user clicks into the widget.
        */
        void mouseClick( QMouseEvent *);
public slots:
        /**
        * Start playing the video.
        */
	void play();
        /**
        * Pauses the video playing (xanim is still runnig).
        */
	void pause();
        /**
        * Stops the video and exit xanim (don't worry about it, it's
        * transparent for the class user).
        *
        * Note: calling this slot doesn't mean that the video will stop
        * inmediatly. Use the @ref #stopped signal to know exactly when
        * the player is set to the inactive mode (see @ref #isActive ).
        */
        void stop();
        /**
        * Single step forward one frame. Pauses the video playing.
        */
        void stepForward();
        /**
        * Single step back one frame. Pauses the video playing.
        */
        void stepBack();
        /**
        * Single step forward one frame staying with anim.
        * Pauses the video playing.
        */
        void stepForwardWA();
        /**
        * Single step back one frame staying with anim.
        * Pauses the video playing.
        */
        void stepBackWA();
        /**
        * Toggle sound between ON/OFF.
        */
        void toggleSound();
        /**
        * Increment volume by 1.
        */
        void volumeIncrement();
        /**
        * Decrement volume by 1.
        */
        void volumeDecrement();
        /**
        * Setup volume where v is a number from 0 to 100.
        */
        void setVolume(int v);
        /**
        * Increase animation playback speed.
        */
        void faster();
        /**
        * Decrease animation playback speed.
        */
        void slower();
        /**
        * Reset animation playback speed to original values.
        */
        void resetSpeed();
protected:
        /**
        * Don't use it!. This is internally used for the communication
        * with xanim.
        */
	void changeProperty( const char * c);
        virtual void closeEvent( QCloseEvent *);
        virtual void mousePressEvent( QMouseEvent *);
private:
	bool playing;
        bool active;
        bool autoResize;
        bool loop;
	QString fileName;
        QString outBuff;
        QString videoCodec;
        int errorCode;
        QString errorString;
        QString temp;
	Window window;
	Display *dpy;
	unsigned long window_atom;
        KProcess proc;
        void  setDefaults();
        int   videoHeight;
        int   videoWidth;
        float videoSpeed;
        int   videoFrames;
        int   getVideoInfo();
        void  setErrorString( int, QString );
        void  parseParameters();

        /* =========== SOUND ============ */
        bool audio;
        bool audioSync;
        int  audioInitialVolume;
        /* =========== COLOR ============ */
        colorMap colorMapping;
        bool colorLookUp;
        int  colorAhead;
        bool colorGray;
        /* =========== GAMMA ============ */
        float gammaDisplay;
        /* =========== SCALING ============ */
        bool  resizing;

        float scaleFactor;
        float scaleHFactor;
        float scaleVFactor;
        int   scaleWidth;
        int   scaleHeight;
        bool  scaleToBuffer;

        float scaleFactorB;
        float scaleHFactorB;
        float scaleVFactorB;
        int   scaleWidthB;
        int   scaleHeightB;
        bool  scaleToDisplay;
        /* =========== OTHERS ============ */
        int loading;
        // bool preload;
        bool x11Shared;
        bool multiBuffer;
        bool usePixmap;
        QString x11VisualClass;
        int pauseAt;
        QString executable;
        QString extras;
private slots:
        void getOutput( KProcess *, char *, int);
        void emitStopped( KProcess *);
        void checkOutput( KProcess *);
};
#endif
