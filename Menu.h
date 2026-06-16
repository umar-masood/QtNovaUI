#pragma once

#include "ScrollBar.h"
#include "RoundedBox.h"
#include "Delegate.h"
#include "SmoothOpacity.h"

#include <QStandardItem>
#include <QListView>
#include <QStandardItemModel>
#include <QMouseEvent>
#include <QApplication>
#include <QGuiApplication>
#include <QScreen>
#include <QVBoxLayout>
#include <QAbstractItemModel>
#include <QAbstractItemView>
#include <QAbstractScrollArea>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QFrame>
#include <QTimer>
#include <QCursor>
#include <QMap>
#include <QEvent>
#include <algorithm>

class MenuItem : public QStandardItem {
   public:
   explicit MenuItem(const QString &iconPath, const QString &text, const QString &shortcut, bool hasSubMenu);
};

class Menu : public QListView {
   Q_OBJECT

   public:
   struct MenuAction {
      const QString text = QString();
      const bool hasSubMenu = false;
      const QString shortcut = QString(); 
      const QString lightIcon = QString();
      const QString darkIcon = QString();

      MenuAction(const QString &actionText, const QString &actionShortcut) 
         : text(actionText), shortcut(actionShortcut) {}
      
      MenuAction(const QString &actionText) : text(actionText){}

      MenuAction(const QString &actionText, bool hasSubmenu, const QString &shortcutText, const QString &iconLight, const QString &iconDark) : text(actionText), hasSubMenu(hasSubmenu), shortcut(shortcutText), lightIcon(iconLight), darkIcon(iconDark) {}
      
   }; 

   explicit Menu(QWidget *parent = nullptr);

   void fadeIn();
   void fadeOut();

   /**
    * @param text QString
    * @param hasSubmenu bool
    * @param shortcut QString
    * @param lightIcon QString
    * @param darkIcon QString
    * @warning Using both Submenu and Shortcut at same time is not allowed.
    */
   void addAction(const MenuAction &menuAction);
   bool hasActions() const;

   /**
    * @brief Specify the index of the item which you want to remove.
    */
   void removeAction(int index);
   void clearAll();

   /**
    * @attention Must pass the parent menu to setParentMenu() whenever you add a submenu
    */
   void addSubMenu(int index, Menu *submenu);
   
   /**
    * @attention Must call show() before using move() in order to setup the Menu properly
    */
   void move(const QPoint &point);
  
   void hide();
   void show();
   void showAt(QWidget *anchorWidget);

   QString clickedItemText() const;
   QString clickedItemShortcut() const;
   int clickedItemIndex() const;

   void setParentMenu(Menu *parentMenu);
   
   void setItemSize(QSize size);
   QSize itemSize();

   void setIconic(bool value);
   void setDarkMode(bool value);

   void setMaxVisibleItems(int items);
   int maxVisibleItems() const;

   Delegate* delegate() const;
   QModelIndex itemIndex(const QString &itemText);

   signals:
   void itemClicked();

   private slots:
   void onItemClicked(const QModelIndex &index);

   protected:
   void mouseMoveEvent(QMouseEvent *event) override;
   void leaveEvent(QEvent *event) override;
   bool eventFilter(QObject *o, QEvent *event) override;
   
   private:
   void init();   
   void updateMenu();
   void setHoveredIndex(const QModelIndex &index);

   QPoint adjustXY(const QSize &s, const QPoint &p, const QRect &screenGeo, bool isSubMenu);
   QPoint adjustSubMenuPosition(Menu *subMenu, const QPoint &intendedPos);

   // Flags
   bool isIconic = false;
   bool isDarkMode = false;

   // Max Visible Items
   int _maxVisibleItems;
   
   // Clicked Item Shortcut, Text, Index
   QString _clickedItemShortcut;
   QString _clickedItemText;
   int _clickedItemIndex = -1;
   
   // Submenus
   QMap<int, Menu*> subMenus;
   QTimer *subMenuTimer = nullptr;
   QModelIndex hoveredIndex;

   // Popup
   RoundedBox *popup = nullptr;

   // Delegate
   Delegate *_delegate = nullptr;

   // Model
   QStandardItemModel _model;

   // Layout
   QVBoxLayout *layout = nullptr;
   
   // Scroll Bar
   ScrollBar *vScroll = nullptr;
   
   // Opacity Effect & Animation for Fade In & Out Effect
   SmoothOpacity *smooth_opacity = nullptr;
   QPropertyAnimation *animation = nullptr;

   // Parent Menu
   Menu *parentMenu = nullptr;
   
   // Items Size
   QSize _itemSize;
};
