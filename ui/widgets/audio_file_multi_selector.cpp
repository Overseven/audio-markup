#include "audio_file_multi_selector.h"
#include "ui_audio_file_multi_selector.h"

AudioFileMultiSelector::AudioFileMultiSelector(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AudioFileMultiSelector)
{
    ui->setupUi(this);
    connect(ui->treeWidget, &QTreeWidget::itemChanged, this, &AudioFileMultiSelector::item_changed);
}

AudioFileMultiSelector::~AudioFileMultiSelector()
{
    delete ui;
}

void AudioFileMultiSelector::set_sample_provider(std::shared_ptr<ISamplesProvider> _samples_provider)
{
    samples_provider = _samples_provider;
    samples_provider->connect_to_signal_samples_list_changed(this, SLOT(update_list()), true);
}

void AudioFileMultiSelector::update_list()
{
    ui->treeWidget->clear();
    auto filenames = samples_provider->get_files_list();

    auto root_item = new QTreeWidgetItem(ui->treeWidget);
    root_item->setText(0, "files");
    root_item->setCheckState(1, Qt::CheckState::Unchecked);

    for (const auto &filename : filenames) {
        auto new_item = new QTreeWidgetItem(root_item);
        new_item->setText(0, filename);
        new_item->setCheckState(1, Qt::CheckState::Unchecked);
    }
}

void AudioFileMultiSelector::item_changed(QTreeWidgetItem *item, int column)
{
    if (column == 0) {
        return;
    }
    auto state = item->checkState(column);
    ui->treeWidget->blockSignals(true);

    set_item_checkbox_state_recursive_down(item, column, state);
    if (state == Qt::CheckState::Unchecked) {
        set_item_checkbox_state_recursive_up(item, column, state);
    }

    ui->treeWidget->blockSignals(false);
}

void AudioFileMultiSelector::set_item_checkbox_state_recursive_up(QTreeWidgetItem *item, int column, Qt::CheckState state)
{
    item->setCheckState(column, state);
    auto it = item->parent();
    while (it != ui->treeWidget->invisibleRootItem() && it != nullptr){
        it->setCheckState(column, state);
        it = it->parent();
    }
}

void AudioFileMultiSelector::set_item_checkbox_state_recursive_down(QTreeWidgetItem *item, int column, Qt::CheckState state)
{
    item->setCheckState(column, state);
    auto childCount = item->childCount();
    for (int i = 0; i < childCount; i++){
        auto it = item->child(i);
        set_item_checkbox_state_recursive_down(it, column, state);
    }
}

QVector<QString> AudioFileMultiSelector::get_selected_audio_filenames() const
{
    auto selected = QVector<QString>();

    auto root = ui->treeWidget->invisibleRootItem();
    if (root->childCount() == 0) {
        return {};
    }
    collect_selected(root, selected);

    return selected;
}

void AudioFileMultiSelector::collect_selected(QTreeWidgetItem *item, QVector<QString> &selected) const
{
    auto check_and_add_to_selected = [this, &selected](QTreeWidgetItem *it) {
        if (it == ui->treeWidget->invisibleRootItem()) {
            return;
        }

        auto item_name = it->text(0);
        if (!item_name.endsWith(".wav")) {
            return;
        }
        bool is_selected = it->checkState(1);
        if (is_selected) {
            auto filename = it->text(0);
            selected.push_back({filename});
        }
    };

    check_and_add_to_selected(item);
    auto childCount = item->childCount();
    for (int i = 0; i < childCount; i++){
        auto it = item->child(i);
        collect_selected(it, selected);
    }
}
