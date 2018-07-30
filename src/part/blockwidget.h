#ifndef BLOCKWIDGET_H
#define BLOCKWIDGET_H

#include <QWidget>



#include <QBrush>
#include <QPen>
#include <QPixmap>


class BlockWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BlockWidget(QWidget *parent = nullptr);

signals:

public slots:





public:
    enum Shape { Line, Points, Polyline, Polygon, Rect, RoundedRect, Ellipse, Arc,
                 Chord, Pie, Path, Text, Pixmap };

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

public slots:
    void setPen(const QPen &pen);
    void setBrush(const QBrush &brush);


protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QPen pen;
    QBrush brush;
};

#endif // BLOCKWIDGET_H
