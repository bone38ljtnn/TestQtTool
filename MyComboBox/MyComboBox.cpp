#include "comboview.h"

ComboView::ComboView(QWidget *parent)
    : QListWidget(parent)
{

}

QRect ComboView::visualRect(const QModelIndex &index) const
{
    QRect rect = QListWidget::visualRect(index);
    int width = this->width();
    if(verticalScrollBar()->isVisible()){
        width -= verticalScrollBar()->width();
    }
    rect.setWidth(width);
    return rect;
}

ComboItem::ComboItem(const QString &text, QWidget *parent)
    : QWidget(parent),
      textValue(text)
{
    btn = new QPushButton(this);
    btn->setFixedSize(18,18);
    btn->setStyleSheet("QPushButton{background-color: transparent; border: none;border-image: url(style/icons/delete.png);}"
                       "QPushButton:hover{border-image: url(style/icons/delete_hover.png);}");
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addStretch();
    layout->addWidget(btn);
    layout->setContentsMargins(5,0,5,0);
    layout->setSpacing(0);
    this->setLayout(layout);
    connect(btn,&QPushButton::clicked,[this]{
        emit itemClicked(textValue);
    });
}

QString ComboItem::text() const
{
    return textValue;
}

ComboDelegate::ComboDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{

}

void ComboDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem opt(option);
    //去掉焦点带来的虚线框
    opt.state &= ~QStyle::State_HasFocus;
    initStyleOption(&opt, index);
    QStyledItemDelegate::paint(painter, opt, index);
}

MyComboBox::MyComboBox(QWidget *parent)
    : QComboBox(parent),
      itemList(new ComboView(this))
{
//    itemList->setTextElideMode(Qt::ElideNone);
    setModel(itemList->model());
    setView(itemList);
    setEditable(true);
    setItemDelegate(new ComboDelegate(this));
    setMaxVisibleItems(5);
}

void MyComboBox::setRemovableItems(const QStringList &items)
{
    //combox的additem insertitem不是虚函数
    //实现里时调用的model->insertRow，但是懒得再去重写listmodel-view，就新增一个接口
    itemList->clear();
    if(items.isEmpty())
        return;
    for(int i = 0;i < items.count();i++){
        MyAddItem(items.at(i));
    }
}

void MyComboBox::MyAddItem(const QString &text)
{
    QListWidgetItem* widget_item = new QListWidgetItem(itemList);
    ComboItem *item = new ComboItem(text,itemList);
    widget_item->setData(Qt::DisplayRole,text);
    itemList->setItemWidget(widget_item, item);
    connect(item,&ComboItem::itemClicked,this,[this,item,widget_item](){
        //take移除item后没有刷新弹框大小，干脆隐藏掉先
        hidePopup();
        itemList->takeItem(itemList->row(widget_item));
        delete widget_item;
        if(itemList->count() <= 5){
            for (int i = 0; i < itemList->count(); i++){
                QWidget *nwidget = itemList->itemWidget(itemList->item(i));
                QHBoxLayout *layout = (QHBoxLayout *)nwidget->layout();
                layout->setContentsMargins(5,0,5,0);
                nwidget->setLayout(layout);
            }
        }
        emit itemChange();
    });
    if(itemList->count() > 5){
        for (int i = 0; i < itemList->count(); i++){
            QWidget *nwidget = itemList->itemWidget(itemList->item(i));
            QHBoxLayout *layout = (QHBoxLayout *)nwidget->layout();
            layout->setContentsMargins(5,0,15,0);
            nwidget->setLayout(layout);
        }
    }
}

void MyComboBox::addCurruntText(QString text)
{
    for (int i = 0; i < count(); ++i) {
        ComboItem *m_widget = qobject_cast<ComboItem *>(itemList->itemWidget(itemList->item(i)));
        if(!m_widget) removeItem(i);
    }
    for (int i = 0; i < count(); ++i) {
        if(itemText(i) == text) return;
    }
    MyAddItem(text);
    setCurrentIndex(this->count()-1);
}

void MyComboBox::addKeyEnterText()
{
    QString text = currentText();
    addCurruntText(text);
}

void MyComboBox::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Enter|| event->key() == Qt::Key_Return){
        addKeyEnterText();
    }
    else {
        QComboBox::keyPressEvent(event);
    }
}

void MyComboBox::showPopup()
{
    QComboBox::showPopup();
}

void MyComboBox::hidePopup()
{
    QStyle * const style = this->style();
    QStyleOptionComboBox opt;
    initStyleOption(&opt);
    view()->scrollTo(view()->currentIndex(),
                     style->styleHint(QStyle::SH_ComboBox_Popup, &opt, this)
                     ? QAbstractItemView::PositionAtCenter
                     : QAbstractItemView::EnsureVisible);
    QComboBox::hidePopup();
}


