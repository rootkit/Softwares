#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSplitter>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QGridLayout>
#include <QPixmap>
#include "CLabelImageDrag.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
private:
    void initWidgets();
    void resetAllParameters();
    void resetPimplesParas();
    void resetBlackheadsParas();
    void resetFaceColoreParas();
    void resetPoreParas();
    void resetCoarseparas();
private:
    QSplitter *m_pSplitter;
    QWidget *m_pWidgetLeft;
    QWidget *m_pWidgetRight;
    QVBoxLayout *m_pVLayoutLeft;
    QHBoxLayout *m_pHLayoutBrowse;
    QVBoxLayout *m_pVLayoutRight;

    QPixmap m_pixmap;
    CLabelImageDrag *m_pLabelImage;
    QLineEdit *m_pEditPhoto;
    QPushButton *m_pButtonBrowse;

    QGroupBox *m_pGroupAll;
    QPushButton *m_pButtonResetAll;
    QPushButton *m_pButtonDetectAll;
    QHBoxLayout *m_pHLayoutAll;

    QGroupBox *m_pGroupPimples;
    QGridLayout *m_pGridPimples;
    QLabel *m_pLabelMinSizePimple;
    QLabel *m_pLabelMaxSizePimple;
    QLabel *m_pLabelMinColorPimple;
    QLabel *m_pLabelMaxColorDiffPimple;
    QLabel *m_pLabelMinRatioPimple;
    QLabel *m_pLabelMaxRatioPimple;
    QLineEdit *m_pEditMinSizePimple;
    QLineEdit *m_pEditMaxSizePimple;
    QLineEdit *m_pEditMinColorPimple;
    QLineEdit *m_pEditMaxColorPimple;
    QLineEdit *m_pEditMinRatioPimple;
    QLineEdit *m_pEditMaxRatioPimple;
    QPushButton *m_pButtonResetPimples;
    QPushButton *m_pButtonDetectPimples;

    QGroupBox *m_pGroupBlackheads;
    QGridLayout *m_pGridBlackheads;
    QLabel *m_pLabelMaxSizeBlackheads;
    QLabel *m_pLabelMaxColorBlackheads;
    QLabel *m_pLabelMinRatioBlackheads;
    QLabel *m_pLabelMaxRatioBlackheads;
    QLineEdit *m_pEditMaxSizeBlackheads;
    QLineEdit *m_pEditMaxColorBlackheads;
    QLineEdit *m_pEditMinRatioBlackheads;
    QLineEdit *m_pEditMaxRatioBlackheads;
    QPushButton *m_pButtonResetBlackheads;
    QPushButton *m_pButtonDetectBlackheads;

    QGroupBox *m_pGroupFaceColor;
    QGridLayout *m_pGridFaceColor;
    QLabel *m_pLabelTouBai;
    QLabel *m_pLabelBaiXi;
    QLabel *m_pLabelZiRan;
    QLabel *m_pLabelXiaoMai;
    QLabel *m_pLabelAnHei;
    QLabel *m_pLabelYouHei;
    QLineEdit *m_pEditTouBai;
    QLineEdit *m_pEditBaiXi;
    QLineEdit *m_pEditZiRan;
    QLineEdit *m_pEditXiaoMai;
    QLineEdit *m_pEditAnHei;
    QLineEdit *m_pEditYouHei;
    QPushButton *m_pButtonResetFaceColor;
    QPushButton *m_pButtonDetectFaceColor;

    QGroupBox *m_pGroupPore;
    QGridLayout *m_pGridPore;
    QLabel *m_pLabelRoughPore;
    QLabel *m_pLabelNormalPore;
    QLabel *m_pLabelSmoothPore;
    QLineEdit *m_pEditRoughPore;
    QLineEdit *m_pEditNormalPore;
    QPushButton *m_pButtonResetPore;
    QPushButton *m_pButtonDetectPore;

    QGroupBox *m_pGroupCoarseness;
    QGridLayout *m_pGridCoarse;
    QLabel *m_pLabelRoughCoarse;
    QLabel *m_pLabelNormalCoarse;
    QLabel *m_pLabelSmoothCoarse;
    QLineEdit *m_pEditRoughCoarse;
    QLineEdit *m_pEditNormalCoarse;
    QPushButton *m_pButtonResetCoarse;
    QPushButton *m_pButtonDetectCoarse;
};

#endif // MAINWINDOW_H