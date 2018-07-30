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





public slots:
    void setPen(const QPen &pen);
    void setBrush(const QBrush &brush);
    void setGridSize(const int step = 8);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QPen pen;
    QBrush brush;
    int grid_step;
};

#endif // BLOCKWIDGET_H
