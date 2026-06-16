#include "Menu.h"

MenuItem::MenuItem(const QString &iconPath, const QString &text, const QString &shortcut, bool hasSubMenu) : QStandardItem(text) {
    setData(shortcut, Qt::UserRole + 1);
    setData(hasSubMenu, Qt::UserRole + 2);
    setData(iconPath, Qt::UserRole + 3);
}

Menu::Menu(QWidget *parent) : QListView(parent), _maxVisibleItems(8) {
    setItemSize(QSize(150, 36));
    setMouseTracking(true);
    init();
}

void Menu::init() {
    // Popup
    popup = new RoundedBox(nullptr);
    popup->hide();

    // Delegate
    _delegate = new Delegate(itemSize());
    _delegate->setAsMenu(true);

    // Vertical Scroll Bar
    vScroll = new ScrollBar(Qt::Vertical);

    // List Properties
    setModel(&_model);
    setItemDelegate(_delegate);
    setAutoScroll(true);    
    setViewMode(QListView::ListMode);
    setSelectionMode(QAbstractItemView::NoSelection);
    setResizeMode(QListView::Adjust);
    setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    setUniformItemSizes(true);
    setFrameShape(QFrame::NoFrame);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    setVerticalScrollBar(vScroll);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setStyleSheet("QListView::item { margin: 0px; padding: 0px; } QListView { background: transparent; margin: 4px; padding: 0px; border:none; }");

    // Layout
    layout = new QVBoxLayout(popup);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addSpacing(0);
    layout->addWidget(this, 0, Qt::AlignVCenter);
  
    // Smooth Opacity
    smooth_opacity = new SmoothOpacity;
    popup->setGraphicsEffect(smooth_opacity);

    // Opacity Animation
    animation = new QPropertyAnimation(smooth_opacity, "opacity");
    animation->setEasingCurve(QEasingCurve::InOutQuad);
    animation->setDuration(300);

    // Installing Event Filter
    qApp->installEventFilter(this);

    // Item Clicked Signal Slot
    disconnect(this, &Menu::clicked, this, &Menu::onItemClicked);
    connect(this, &Menu::clicked, this, &Menu::onItemClicked);

    // Initial Theme
    setDarkMode(isDarkMode);
}

void Menu::fadeIn() {
    animation->stop();
    disconnect(animation, &QPropertyAnimation::finished, nullptr, nullptr);
    animation->setStartValue(0.0);
    animation->setEndValue(1.0);
    animation->start();
    if (popup) popup->show();
}

void Menu::fadeOut() {
    animation->stop();
    disconnect(animation, &QPropertyAnimation::finished, nullptr, nullptr);
    animation->setStartValue(1.0);
    animation->setEndValue(0.0);
    connect(animation, &QPropertyAnimation::finished, this, [this]() {
        if (popup) popup->hide();
    });
    animation->start();
}

bool Menu::eventFilter(QObject *o, QEvent *event) {
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        QPoint globalPos = mouseEvent->globalPosition().toPoint();
        QWidget *clickedWidget = QApplication::widgetAt(globalPos);

        if (popup->isVisible() && !popup->isAncestorOf(clickedWidget)) 
            fadeOut();
    }

    if (event->type() == QEvent::ApplicationDeactivate)
        if (popup->isVisible())  
            fadeOut();

    return QObject::eventFilter(o, event);
}

void Menu::addAction(const MenuAction &menuAction) {
    MenuItem *item = new MenuItem(isDarkMode ? menuAction.darkIcon : menuAction.lightIcon, 
                                 menuAction.text, menuAction.shortcut, menuAction.hasSubMenu);

    if (!menuAction.shortcut.isEmpty() && menuAction.hasSubMenu) {
        qCritical() << "Using both shortcut and submenu is not allowed.";
        return;
    }

    _model.appendRow(item);
}

bool Menu::hasActions() const {
    return _model.rowCount() != 0;
}

void Menu::removeAction(int index) {
    if (index != -1) {
        _model.removeRow(index);
        subMenus.remove(index);

        QMap<int, Menu*> updatedSubMenus;

        for (auto it = subMenus.begin(); it != subMenus.end(); ++it) {
            int key = it.key();

            Menu *menu = it.value();
            updatedSubMenus[key > index ? key - 1 : key] = menu;
        }

        subMenus = updatedSubMenus;

        updateMenu();
        update();
    }
}

void Menu::clearAll() {
    _model.clear();
    update();
}

void Menu::updateMenu() {
    int itemsHeight = sizeHintForRow(0);
    int totalItems = _model.rowCount(); 
    
    if (totalItems <= 0) 
        return;
    
    int visibleItems = std::min(totalItems, _maxVisibleItems);
    int height = (visibleItems * itemsHeight) + spacing() * (visibleItems - 1) + 8;
    
    setVerticalScrollBarPolicy(totalItems > visibleItems ? Qt::ScrollBarAsNeeded : Qt::ScrollBarAlwaysOff);
    setFixedHeight(height);
    popup->setFixedSize(itemSize().width(), height);
}

void Menu::setHoveredIndex(const QModelIndex &index) {
    hoveredIndex = index;
    
    if (_delegate) 
        _delegate->setHoveredIndex(index);
    
    update();
}

void Menu::addSubMenu(int index, Menu *submenu) {
    if (index != -1 && submenu)
        subMenus[index] = submenu;
    else
        qCritical() << "Invalid submenu index.";
}

void Menu::move(const QPoint &point) {
    if (!popup) 
        return;

    QScreen *screen = QApplication::screenAt(point);
    if (!screen) 
        screen = QGuiApplication::primaryScreen();

    popup->move(adjustXY(popup->size(), point, screen->availableGeometry(), false));
}

QPoint Menu::adjustXY(const QSize &s, const QPoint &p, const QRect &screenGeo, bool isSubMenu) {
    int x = p.x(), y = p.y();

    // Adjust X
    if (x + s.width() > screenGeo.right())
        x = x - s.width() + (isSubMenu ? this->popup->width() : 0) + 3;

    if (x < screenGeo.left())
        x = screenGeo.left(); 

    // Adjust Y
    if (y + s.height() > screenGeo.bottom()) 
        y = screenGeo.bottom() - s.height();
 
    if (y < screenGeo.top())  
        y = screenGeo.top();

    return QPoint(x,y);
}

void Menu::hide() { fadeOut(); }
void Menu::show() {
    updateMenu();
    fadeIn();
}

void Menu::showAt(QWidget *anchorWidget) {
    show();
    
    if (!anchorWidget) 
        return;
    
    QPoint globalPos(anchorWidget->mapToGlobal(QPoint(0,0)));

    QScreen *screen = QApplication::screenAt(globalPos);
    if (!screen) 
        screen = QApplication::primaryScreen();

    QRect screenRect(screen->availableGeometry());  
    QRect anchorRect(globalPos, anchorWidget->size());  
    QSize menuSize = popup->size();

    QPoint abovePos(anchorRect.left(), anchorRect.top() - menuSize.height() - 4);
    QPoint belowPos(anchorRect.left(), anchorRect.bottom() + 4);
    QPoint finalPos;

    if (screenRect.contains(QRect(belowPos, menuSize)))
       finalPos = belowPos;
    else if (screenRect.contains(QRect(abovePos, menuSize)))
       finalPos = abovePos;
    else 
       finalPos = belowPos;

    int x = std::clamp(finalPos.x(), screenRect.left(), screenRect.right() - menuSize.width());
    int y = std::clamp(finalPos.y(), screenRect.top(), screenRect.bottom() - menuSize.height());

    move(QPoint(x, y));
    popup->raise();
}

QPoint Menu::adjustSubMenuPosition(Menu* subMenu, const QPoint& intendedPos) {
    if (!subMenu || !subMenu->popup) 
        return intendedPos;

    QRect screenRect = QApplication::screenAt(intendedPos)->availableGeometry();

    return adjustXY(subMenu->popup->size(), intendedPos, screenRect, true);
}

QString Menu::clickedItemText() const { return _clickedItemText; }
QString Menu::clickedItemShortcut() const { return _clickedItemShortcut; }
int Menu::clickedItemIndex() const { return _clickedItemIndex; }

void Menu::setItemSize(QSize size) { _itemSize = size.expandedTo(QSize(150, 30)); }
QSize Menu::itemSize() { return _itemSize; }

void Menu::setIconic(bool value) { 
    isIconic = value; 
    _delegate->setIconic(isIconic);
}

void Menu::setDarkMode(bool value) { 
    isDarkMode = value; 
    popup->setDarkMode(isDarkMode);
    _delegate->setDarkMode(isDarkMode);
    vScroll->setDarkMode(isDarkMode);
}

void Menu::setParentMenu(Menu *parentMenu) { this->parentMenu = parentMenu; }

void Menu::setMaxVisibleItems(int items) { _maxVisibleItems = items; }
int Menu::maxVisibleItems() const { return _maxVisibleItems; }

Delegate *Menu::delegate() const { return _delegate; }

QModelIndex Menu::itemIndex(const QString &itemText) {
    for (int row = 0; row < _model.rowCount(); ++row) {
        QModelIndex idx = _model.index(row, 0);
        if (_model.data(idx).toString() == itemText)
            return idx;
    }

    qWarning() << "Item is not found in model of menu";
    return QModelIndex();
}

void Menu::mouseMoveEvent(QMouseEvent *event) {
    const QModelIndex index = indexAt(event->position().toPoint());
    
    if (!index.isValid()) 
        return;
    
    QPoint globalPos = mapToGlobal(visualRect(index).topRight()) - QPoint(4,0);

    setHoveredIndex(index);

    if (subMenuTimer) {
        subMenuTimer->stop();
        delete subMenuTimer;
        subMenuTimer = nullptr;
    }

    for (auto it = subMenus.begin(); it != subMenus.end(); ++it) {
        Menu *subMenu = it.value();
        int subMenuIndex = it.key();

        if (subMenuIndex == index.row()) {
            if (subMenu && !subMenu->isVisible() ) {
                subMenuTimer = new QTimer(this);
                subMenuTimer->setSingleShot(true);
                subMenuTimer->start(300);

                connect(subMenuTimer, &QTimer::timeout, [=](){
                    QModelIndex index = indexAt(mapFromGlobal(QCursor::pos()));
                    if (index.isValid() && index.row() == subMenuIndex) {
                        subMenu->show();
                        QPoint adjusted = adjustSubMenuPosition(subMenu, globalPos);
                        subMenu->move(adjusted);
                    } 
                });
            }
        } else {
            if (subMenu && subMenu->isVisible()) 
                subMenu->hide();
        }
    }

    update();
    QListView::mouseMoveEvent(event);
}

void Menu::leaveEvent(QEvent *event) {
   QTimer::singleShot(200, [this](){
     bool stillHovering = false;
        for (auto *submenu : subMenus) 
            if (submenu && submenu->popup && submenu->popup->underMouse()) 
                stillHovering = true;
        
        if (!stillHovering) {  
            setHoveredIndex(QModelIndex());
            update();
        }
   });

   QListView::leaveEvent(event);
}

void Menu::onItemClicked(const QModelIndex &index) {
    int idx = index.row();

    _delegate->setActiveIndex(index); // Passing it for dot or check indicator

    bool hasSubMenu = index.data(Qt::UserRole + 2).toBool();
    if (hasSubMenu) 
        return;

    _clickedItemText = index.data(Qt::DisplayRole).toString();
    _clickedItemIndex = idx;
    _clickedItemShortcut = index.data(Qt::UserRole + 1).toString();

    emit itemClicked();
    fadeOut();
}
