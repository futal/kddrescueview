#ifndef BLOCK_DELEGATE_H
#define BLOCK_DELEGATE_H

#include <QItemDelegate>
#include <QModelIndex>
#include <QSize>

class BlockDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    BlockDelegate(QObject *parent = nullptr);

//    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index ) const override;

public slots:
    void setSquareSize(int size);

private:
    int m_square_size;
};

#endif // BLOCK_DELEGATE_H
