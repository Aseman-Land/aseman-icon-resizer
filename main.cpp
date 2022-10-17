#include <QCoreApplication>
#include <QImageReader>
#include <QImageWriter>
#include <QPainter>
#include <QDir>
#include <QDebug>

void process(const QImage &img, const QString &path)
{
    const auto img_size = img.size();
    const auto img_ratio = (qreal)img_size.width() / img_size.height();

    const auto files = QDir(path).entryList({"*.png"});
    for (const auto &f: files)
    {
        const auto filePath = path + '/' + f;

        QImageReader r(filePath);
        const auto size = r.size();
        const auto ratio = (qreal)size.width() / size.height();

        auto img_scaled_size = size;
        if (img_ratio > ratio)
            img_scaled_size.setWidth(size.height() * img_ratio);
        else
            img_scaled_size.setHeight(size.width() / img_ratio);

        const auto img_scaled = img.scaled(img_scaled_size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

        QImage img_new(size, img.format());
        img_new.fill(QColor(0,0,0,0));

        QPainter painter(&img_new);
        painter.fillRect(img_new.rect(), QColor(0,0,0,0));
        painter.drawImage( QRectF(0,0,size.width(), size.height()), img_scaled,
                           QRectF((img_scaled_size.width() - size.width())/2,
                                  (img_scaled_size.height() - size.height())/2,
                                  size.width(), size.height()));

        QFile::remove(filePath);


        QImageWriter w(filePath);
        w.write(img_new);

        qDebug() << f;
    }

    const auto dirs = QDir(path).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const auto &d: dirs)
        process(img, path + '/' + d);
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    const auto args = app.arguments();
    if (args.count() < 3)
    {
        qDebug() << app.applicationName().toStdString().c_str() << "[source_icon]" << "[path]";
        return 0;
    }

    const auto source = args.at(1);
    const auto path = args.at(2);

    QImageReader r(source);
    const auto img = r.read();
    process(img, path);

    return 0;
}
