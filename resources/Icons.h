#pragma once

enum Icons {
    ArrowDownLight,
    ArrowRightLight,    
    CutLight,
    CopyLight,
    HideLight,
    ShowLight,
    PasteLight,
    RedoLight,
    SelectAllLight,
    ClearLight,
    CheckLight,
    DotLight,
    SearchLight,
    UndoLight,

    ArrowDownDark,
    ArrowRightDark,    
    CutDark,
    CopyDark,
    HideDark ,
    ShowDark ,
    PasteDark,
    RedoDark,
    SelectAllDark,
    ClearDark,
    CheckDark,
    DotDark,
    SearchDark,
    UndoDark
};

class QString;
class QPixmap;
class QSize;

QString icon(Icons icon);
QPixmap renderSVGIcon(const QString &iconPath, const QSize &size);
