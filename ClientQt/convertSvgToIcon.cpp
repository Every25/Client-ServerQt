#include <convertSvgToIcon.h>

QIcon convertSvgToIcon(QString svgString)
{
    QByteArray svgData = svgString.toUtf8();
    QSvgRenderer renderer(svgData);

    // Размер иконки
    int iconSize = 64;

    QPixmap pixmap(iconSize, iconSize);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    renderer.render(&painter);

    return QIcon(pixmap);
}