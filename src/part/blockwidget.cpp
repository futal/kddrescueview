#include "blockwidget.h"

#include <QPainter>

BlockWidget::BlockWidget(QWidget *parent)
    : QWidget(parent)
{
    pen = QPen(Qt::black, 1, Qt::SolidLine);
    brush = QBrush(Qt::green);
    grid_step = 8;
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

void BlockWidget::setGridSize(const int step){
    this->grid_step = step;
    update();
}

void BlockWidget::paintEvent(QPaintEvent * /* event */)
{
    QRect rect(grid_step, grid_step, grid_step, grid_step);
    
    // the qwidget width and height appears to continue under the window border for 8 pixels
    int window_border = 8;
    
    int columns = (width() - 1 - window_border)/grid_step;
    int rows = (height() - 1 - window_border)/grid_step;
    
    QPainter painter(this);
    painter.setPen(pen);
    painter.setBrush(brush);
    
    for (int column = 0; column < columns; ++ column) {
        for (int row = 0; row < rows; ++row) {
            painter.save();
            painter.translate(column*grid_step, row*grid_step);
            painter.drawRect(rect);
            painter.restore();
        }
    }
    painter.drawText(30,30, QString("width: %1, columns: %2, w:%3").arg(width()).arg(columns).arg(columns*grid_step+1));
    painter.drawText(30,60, QString("height: %1, rows: %2, h:%3").arg(height()).arg(rows).arg(rows*grid_step+1));
    
}
