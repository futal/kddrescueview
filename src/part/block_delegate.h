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
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index ) const override;
};

#endif // BLOCK_DELEGATE_H
