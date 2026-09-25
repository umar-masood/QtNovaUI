#include "Icons.h"
#include <QString>
#include <QPainter>
#include <QPixmap>
#include <QSvgRenderer>
#include <QSize>
#include <QDebug>

QString icon(Icons icon) {
    switch (icon) {   
        case Icons::ArrowDownLight:         return ":/icons/arrow-down.svg";
        case Icons::ArrowRightLight:        return ":/icons/arrow-right.svg";
        case Icons::CheckLight:             return ":/icons/check.svg";
        case Icons::DotLight:               return ":/icons/dot.svg";
        case Icons::SearchLight:            return ":/icons/search.svg";
        case Icons::CutLight:               return ":/icons/cut.svg";
        case Icons::CopyLight:              return ":/icons/copy.svg";
        case Icons::HideLight:              return ":/icons/hide.svg";
        case Icons::ShowLight:              return ":/icons/show.svg";
        case Icons::PasteLight:             return ":/icons/paste.svg";
        case Icons::RedoLight:              return ":/icons/redo.svg";
        case Icons::SelectAllLight:         return ":/icons/select-all.svg";
        case Icons::UndoLight:              return ":/icons/undo.svg";
        case Icons::ClearLight:             return ":/icons/x.svg";

        case Icons::ArrowDownDark:         return ":/icons/arrow-down-dark.svg";
        case Icons::ArrowRightDark:        return ":/icons/arrow-right-dark.svg";
        case Icons::CheckDark:             return ":/icons/check-dark.svg";
        case Icons::DotDark:               return ":/icons/dot-dark.svg";
        case Icons::SearchDark:            return ":/icons/search-dark.svg";
        case Icons::CutDark:               return ":/icons/cut-dark.svg";
        case Icons::CopyDark:              return ":/icons/copy-dark.svg";
        case Icons::HideDark:              return ":/icons/hide-dark.svg";
        case Icons::ShowDark:              return ":/icons/show-dark.svg";
        case Icons::PasteDark:             return ":/icons/paste-dark.svg";
        case Icons::RedoDark:              return ":/icons/redo-dark.svg";
        case Icons::SelectAllDark:         return ":/icons/select-all-dark.svg";
        case Icons::UndoDark:              return ":/icons/undo-dark.svg";
        case Icons::ClearDark:             return ":/icons/x-dark.svg";
    }

    qWarning("The requested icon is not found");
    return QString();
}

QPixmap renderSVGIcon(const QString &iconPath, const QSize &size) {
    QSvgRenderer svg(iconPath);
    svg.setAspectRatioMode(Qt::KeepAspectRatio);
    
    if (!svg.isValid()) {
        qWarning() << "SVG not valid or not found:" << iconPath;
        return QPixmap();
    }

    // --- Render SVG into 16x16 pixmap ---
    QPixmap pm(size);
    pm.fill(Qt::transparent);

    QPainter p(&pm);
    p.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    svg.render(&p, QRectF(0, 0, pm.width(), pm.height()));
    p.end();

    return pm;
}