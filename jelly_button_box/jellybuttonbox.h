#ifndef JELLYBUTTONBOX_H
#define JELLYBUTTONBOX_H

#include <QObject>
#include <QWidget>
#include <QPropertyAnimation>
#include <cmath>
#include <QGraphicsDropShadowEffect>
#include "jellybutton.h"

class JellyButtonBox : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int step1 READ getStep1 WRITE setStep1)
    Q_PROPERTY(int step2 READ getStep2 WRITE setStep2)
    Q_PROPERTY(int qie_angle READ getQieAngle WRITE setQieAngle)
    Q_PROPERTY(int step3 READ getStep3 WRITE setStep3)
public:
    JellyButtonBox(QWidget *parent = nullptr);

    void setColors(QColor bg, QColor fg);
    void setSize(int outer, int inner, int spacing);
    void setButtonIcons(QString icon1, QString icon2, QString icon3);
    void setButtonPixmaps(QString icon1, QString icon2, QString icon3);
    void setButtons(QList<QPixmap> icons, QList<QString> texts = {});
    void setButtons(QList<QIcon> icons, QList<QString> texts = {});
    void setHideAfterClick(bool h = true);
    void exec(QPoint start_pos, QPoint end_pos);
    void toHide();

private:
    void startAnimation1(QPoint start_pos, QPoint end_pos);
    void startAnimation2();
    void startAnimation3();
    void endAnimation3();
    void endAnimation2();
    void endAnimation1();

protected:
    void paintEvent(QPaintEvent *) override;
    void focusOutEvent(QFocusEvent *event) override;

signals:
    void signalButtonClicked(int index);

public slots:

private:
    void setStep1(int p);
    void setStep2(int p);
    void setQieAngle(int a);
    void setStep3(int p);
    int getStep1();
    int getStep2();
    int getQieAngle();
    int getStep3();

private:
    QList<JellyButton*> buttons;
    int btn_radius = 16; // 按钮最大半径
    int outer_radius = 24; // 大圆半径
    int btn_spacing = 32; // 两个按钮之间的间距
    QColor fg_color = Qt::white;
    QColor bg_color = QColor(58, 54, 94);
    int border_size = 4; // 边界阴影最大处宽度
    int total_width = -1; // 控件真正的总宽度
    int qie_angle = 90; // 切线
    QGraphicsDropShadowEffect* effect;

    int show_prop = 0; // 按钮出现 动画
    int expd_prop = 0; // 整体扩展、按钮背景撕开 动画（反弹）
    int icon_prop = 0; // 背景隐藏+按钮出现 动画
    bool hiding = false; // 是否正在隐藏（无法点击）

    bool hide_after_click = true; // 按钮点击后隐藏
};

#endif // JELLYBUTTONBOX_H
