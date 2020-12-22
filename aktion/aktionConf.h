#ifndef AKTIONCONF
#define AKTIONCONF

#include <qtabdialog.h>

class QLineEdit;
class QCheckBox;
class QRadioButton;
class KConfig;
class QWidget;
class QLabel;
class QGroupBox;
class QButtonGroup;
class QPushButton;
class QSpinBox;
class QListBox;
class QComboBox;

class Setup : public QTabDialog
{
    Q_OBJECT
public:
    Setup( QWidget *parent=0, const char *name=0 );
    ~Setup();
private:
    QLineEdit *lined[6];
    QCheckBox *checkBox[11];
    QRadioButton *botonRadio[7];
    KConfig *config;
    /* the tabs' constructors */
    QWidget *audio();
    QWidget *color();
    QWidget *scaling();
    QWidget *capture();
    QWidget *others();

    QLabel *label[10];
    QGroupBox *grupo[1];
    QButtonGroup *botones[3];
    QComboBox *combo[2];
    QListBox *lista[1];
    QPushButton *dirButton[2];
    QPushButton *executableButton;
    QSpinBox *spin[1];
private slots:
    void quit();
    void save();
    void getDir();
    void getCaptureDir();
    void getXanimExecutable();
    void actualizeDialog(int);
    void actualizeDialog1(int);
};
#endif
