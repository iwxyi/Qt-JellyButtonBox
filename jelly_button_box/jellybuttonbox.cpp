#include "jellybuttonbox.h"

JellyButtonBox::JellyButtonBox(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    setWindowFlag(Qt::FramelessWindowHint, true);
    setFocusPolicy(Qt::StrongFocus);
}

void JellyButtonBox::exec(QPoint start_pos, QPoint end_pos)
{
    show();
    startAnimation1(start_pos, end_pos);
    setFocus();
}

/**
 * 按钮出现动画
 * @param start_pos 开始出现的位置
 * @param end_pos   结束出现的位置
 */
void JellyButtonBox::startAnimation1(QPoint start_pos, QPoint end_pos)
{
    QRect rect(end_pos.x()-outer_radius-border_size, end_pos.y()-outer_radius-border_size, outer_radius*2+border_size*2, outer_radius*2+border_size*2);

    QPropertyAnimation* geo_ani = new QPropertyAnimation(this, "geometry");
    geo_ani->setStartValue(QRect(start_pos, QSize(1, 1)));
    geo_ani->setEndValue(rect);
    geo_ani->setEasingCurve(QEasingCurve::OutBack);
    geo_ani->setDuration(350);
    geo_ani->start();
    connect(geo_ani, &QPropertyAnimation::finished, this, [=]{
        total_width = (btn_radius*2 + btn_spacing + outer_radius + border_size) * 2;
        show_prop = 100;
        QTimer::singleShot(150, [=]{
            startAnimation2();
        });
    });
}

/**
 * 整体扩展、按钮背景撕开 动画（反弹）
 */
void JellyButtonBox::startAnimation2()
{
    QRect rect(geometry().center().x() - total_width / 2, geometry().top(), total_width, height());
    int dur = 3500;

    QPropertyAnimation* geo_ani = new QPropertyAnimation(this, "geometry");
    geo_ani->setStartValue(geometry());
    geo_ani->setEndValue(rect);
    geo_ani->setEasingCurve(QEasingCurve::OutBack);
    geo_ani->setDuration(dur);
    geo_ani->start();

    QPropertyAnimation* step2_ani = new QPropertyAnimation(this, "step2");
    step2_ani->setStartValue(0);
    step2_ani->setEndValue(100);
    step2_ani->setDuration(dur);
    step2_ani->start();
    connect(step2_ani, &QPropertyAnimation::finished, this, [=]{
        expd_prop = 100;
        startAnimation3();
    });
}

void JellyButtonBox::startAnimation3()
{

}

void JellyButtonBox::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // 绘制背景
    if (show_prop < 100) // 画圆
    {
        QPainterPath bg_path;
        bg_path.addEllipse(border_size, border_size, width()-border_size*2, height() - border_size*2);
        painter.fillPath(bg_path, bg_color);

        QPainterPath fg_path;
        int real_radius = (width()-border_size*2) * btn_radius / outer_radius / 2;
        fg_path.addEllipse(width()/2-real_radius, height()/2-real_radius, real_radius*2, real_radius*2);
        painter.fillPath(fg_path, fg_color);
    }
    else if (expd_prop < 100) // 圆角矩形
    {
        QPainterPath bg_path;
        bg_path.addRoundedRect(border_size, border_size, width()-border_size*2, height() - border_size*2, outer_radius, outer_radius);
        painter.fillPath(bg_path, bg_color);

        QPainterPath fg_path;
//        fg_path.addRoundedRect(outer_radius-btn_radius+border_size, outer_radius-btn_radius+border_size, width()-border_size*2-(outer_radius-btn_radius)*2, height()-border_size*2-(outer_radius-btn_radius)*2, btn_radius, btn_radius);
        // 计算三个分开的曲线
        // 进度：expd_prop / 100
        // 总宽度：total_width
        // 最小宽度：(outer_radius + border_size)*2

        int ctry = height()/2;
        QPoint left(border_size + outer_radius, ctry),
                mid(width() / 2, ctry),
                right(width() - border_size - outer_radius, ctry);

        fg_path.moveTo(mid.x(), mid.y()-btn_radius);
        fg_path.quadTo(QPoint((left.x()+mid.x())/2, ctry), QPoint(left.x(), left.y()-btn_radius));
        fg_path.lineTo(left);
        fg_path.arcTo(QRect(left.x()-btn_radius, left.y()-btn_radius, btn_radius*2, btn_radius*2), 90, 180);
        fg_path.quadTo(QPoint((left.x()+mid.x())/2, ctry), QPoint(mid.x(), mid.y()+btn_radius));
        fg_path.quadTo(QPoint((mid.x()+right.x())/2, ctry), QPoint(right.x(), right.y()+btn_radius));
        fg_path.lineTo(right);
        fg_path.arcTo(QRect(right.x()-btn_radius, right.y()-btn_radius, btn_radius*2, btn_radius*2), 270, 180);
        fg_path.quadTo(QPoint((mid.x()+right.x())/2, ctry), QPoint(mid.x(), mid.y()-btn_radius));

        painter.fillPath(fg_path, fg_color);
    }
    else if (icon_prop < 100)
    {
        QPainterPath bg_path;
        bg_path.addRoundedRect(border_size, border_size, width()-border_size*2, height() - border_size*2, outer_radius, outer_radius);
        painter.fillPath(bg_path, bg_color);

        QPainterPath fg_path;
//        fg_path.addRoundedRect(outer_radius-btn_radius+border_size, outer_radius-btn_radius+border_size, width()-border_size*2-(outer_radius-btn_radius)*2, height()-border_size*2-(outer_radius-btn_radius)*2, btn_radius, btn_radius);
        fg_path.moveTo(width()/2, height()/2);
        fg_path.arcTo(QRect(width()/2-20, height()/2-20, 40, 40), 90, 180);
        painter.fillPath(fg_path, fg_color);


    }

    // 绘制前景


}

void JellyButtonBox::focusOutEvent(QFocusEvent *event)
{
    QWidget::focusOutEvent(event);

    close();
}

void JellyButtonBox::setStep1(int p)
{
    show_prop = p;
}

void JellyButtonBox::setStep2(int p)
{
    expd_prop = p;
}

void JellyButtonBox::setStep3(int p)
{
    icon_prop = p;
}

int JellyButtonBox::getStep1()
{
    return show_prop;
}

int JellyButtonBox::getStep2()
{
    return expd_prop;
}

int JellyButtonBox::getStep3()
{
    return icon_prop;
}
