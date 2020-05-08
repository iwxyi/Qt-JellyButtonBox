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
    int dur = 600;

    QPropertyAnimation* geo_ani = new QPropertyAnimation(this, "geometry");
    geo_ani->setStartValue(geometry());
    geo_ani->setEndValue(rect);
    geo_ani->setEasingCurve(QEasingCurve::OutBack);
    geo_ani->setDuration(dur);
    geo_ani->start();

    /*QPropertyAnimation* angle_ani = new QPropertyAnimation(this, "qie_angle");
    angle_ani->setStartValue(90);
    angle_ani->setEndValue(0);
    angle_ani->setDuration(dur);
    angle_ani->start();*/

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
    else if (expd_prop < 100 || true) // 圆角矩形
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

        // 切线贝塞尔算法
        int delta_ol_om = mid.x()-left.x(); // 圆心之间的距离
        double prop = delta_ol_om/(double)(btn_spacing+btn_radius*2);
        if (prop > 1) // 反弹效果时会变成负的
            prop = 1;
        double angle = PI/2 - (PI/2) * prop * prop; // 切线角度

        int qie_delta_x = btn_radius * cos(angle);
        int qie_delta_y = btn_radius * sin(angle);
        QPoint qie_ulm(left.x() + qie_delta_x,left.y() - qie_delta_y);
        QPoint qie_uml(mid.x() - qie_delta_x, mid.y() - qie_delta_y);
        QPoint qie_dlm(left.x() + qie_delta_x,left.y() + qie_delta_y);
        QPoint qie_dml(mid.x() - qie_delta_x, mid.y() + qie_delta_y);
        QPoint qie_umr(mid.x() + qie_delta_x, mid.y() - qie_delta_y);
        QPoint qie_urm(right.x() - qie_delta_x,right.y() - qie_delta_y);
        QPoint qie_dmr(mid.x() + qie_delta_x, mid.y() + qie_delta_y);
        QPoint qie_drm(right.x() - qie_delta_x,right.y() + qie_delta_y);

        int dis = qie_uml.x() - qie_ulm.x();
//        int dis_calc = dis*sqrt(prop) * sin(angle) * cos(angle);
        int dis_calc = dis*prop*prop;
        int ctrl_delta_x = dis_calc * cos(angle);
        int ctrl_delta_y = dis_calc * sin(angle);
        QPoint ctrl_uml = qie_uml + QPoint(-ctrl_delta_x, ctrl_delta_y);
        QPoint ctrl_ulm = qie_ulm + QPoint(ctrl_delta_x, ctrl_delta_y);
        QPoint ctrl_dlm = qie_dlm + QPoint(ctrl_delta_x, -ctrl_delta_y);
        QPoint ctrl_dml = qie_dml + QPoint(-ctrl_delta_x, -ctrl_delta_y);
        QPoint ctrl_umr = qie_umr + QPoint(ctrl_delta_x, ctrl_delta_y);
        QPoint ctrl_urm = qie_urm + QPoint(-ctrl_delta_x, ctrl_delta_y);
        QPoint ctrl_drm = qie_drm + QPoint(-ctrl_delta_x, -ctrl_delta_y);
        QPoint ctrl_dmr = qie_dmr + QPoint(ctrl_delta_x, -ctrl_delta_y);

        angle = angle * 180 / PI; // 切线弧度制转角度制，用来 arcTo
        double degle = 90 - angle; // 90-切线角度

        fg_path.moveTo(mid.x(), mid.y()-btn_radius);
        fg_path.arcTo(mid.x()-btn_radius, mid.y()-btn_radius, btn_radius*2, btn_radius*2, 90, degle);
        fg_path.cubicTo(ctrl_uml, ctrl_ulm, qie_ulm);
        fg_path.arcTo(QRect(left.x()-btn_radius, left.y()-btn_radius, btn_radius*2, btn_radius*2), angle, 180+degle*2);
        fg_path.cubicTo(ctrl_dlm, ctrl_dml, qie_dml);
        fg_path.arcTo(mid.x()-btn_radius, mid.y()-btn_radius, btn_radius*2, btn_radius*2, 180+angle, degle*2);
        fg_path.cubicTo(ctrl_dmr, ctrl_drm, qie_drm);
        fg_path.arcTo(right.x()-btn_radius, right.y()-btn_radius, btn_radius*2, btn_radius*2, 180+angle, 180+degle*2);
        fg_path.cubicTo(ctrl_urm, ctrl_umr, qie_umr);
        fg_path.arcTo(mid.x()-btn_radius, mid.y()-btn_radius, btn_radius*2, btn_radius*2, angle, degle);
        fg_path.lineTo(mid.x(), mid.y()-btn_radius);

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

void JellyButtonBox::setQieAngle(int a)
{
    qie_angle = a;
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

int JellyButtonBox::getQieAngle()
{
    return qie_angle;
}

int JellyButtonBox::getStep3()
{
    return icon_prop;
}
