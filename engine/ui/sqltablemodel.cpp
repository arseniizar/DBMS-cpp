//
// Created by Arsenii Zarudniuk on 09/12/2025.
//

#include "sqltablemodel.hpp"

SqlTableModel::SqlTableModel(const std::vector<Column>& data, QObject *parent)
    : QAbstractTableModel(parent), m_data(data) {}

int SqlTableModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid() || m_data.empty()) {
        return 0;
    }
    // Кількість рядків визначається по першій колонці
    return m_data[0].get_rows().size();
}

int SqlTableModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return m_data.size();
}

QVariant SqlTableModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || role != Qt::DisplayRole) {
        return QVariant();
    }

    int row = index.row();
    int col = index.column();

    if (col < m_data.size() && row < m_data[col].get_rows().size()) {
        return QString::fromStdString(m_data[col].get_rows()[row].get_data());
    }

    return QVariant();
}

QVariant SqlTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    if (orientation == Qt::Horizontal) {
        if (section < m_data.size()) {
            return QString::fromStdString(m_data[section].get_name());
        }
    } else if (orientation == Qt::Vertical) {
        // Нумеруємо рядки, починаючи з 1
        return QString::number(section + 1);
    }

    return QVariant();
}
