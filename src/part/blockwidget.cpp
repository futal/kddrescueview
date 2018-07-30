#include "blockwidget.h"

#include <QPainter>

BlockWidget::BlockWidget(QWidget *parent)
    : QWidget(parent)
{
    pen = QPen(Qt::black, 1, Qt::SolidLine);
    brush = QBrush(Qt::green);
}

QSize BlockWidget::minimumSizeHint() const
{
    return QSize(100, 100);
}

QSize BlockWidget::sizeHint() const
{
    return QSize(400, 200);
}

void BlockWidget::setPen(const QPen &pen)
{
    this->pen = pen;
    update();
}

void BlockWidget::setBrush(const QBrush &brush)
{
    this->brush = brush;
    update();
}

void BlockWidget::paintEvent(QPaintEvent * /* event */)
{
    QRect rect(8, 8, 8, 8);
    QPainter painter(this);
    painter.setPen(pen);
    painter.setBrush(brush);
    for (int x = 0; x < width()-20; x += 8) {
        for (int y = 0; y < height()-20; y += 8) {
            painter.save();
            painter.translate(x, y);
            painter.drawRect(rect);
            painter.restore();
        }
    }

    //painter.setRenderHint(QPainter::Antialiasing, false);
    //painter.setPen(palette().dark().color());
    //painter.setBrush(Qt::NoBrush);
    // painter.drawRect(QRect(0, 0, width() - 1, height() - 1));
}
