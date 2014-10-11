#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QDialog>
#include <buffy/config/config.h>

namespace Ui {
class Preferences;
}

class Preferences : public QDialog
{
    Q_OBJECT

public:
    explicit Preferences(QWidget *parent = 0);
    ~Preferences();

    void from_config(buffy::config::Config& config);
    void to_config(buffy::config::Config& config);

protected slots:
    void folder_list_context_menu(const QPoint&);

private:
    Ui::Preferences *ui;
};

#endif // PREFERENCES_H
