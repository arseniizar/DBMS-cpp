//
// Created by Arsenii Zarudniuk on 09/12/2025.
//
#ifndef SQLTABLEMODEL_HPP
#define SQLTABLEMODEL_HPP

#include <QAbstractTableModel>
#include <vector>
#include "engine/table/structs/column/Column.hpp"

class SqlTableModel : public QAbstractTableModel {
    Q_OBJECT
public:
    explicit SqlTableModel(const std::vector<Column>& data, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
    std::vector<Column> m_data;
};

#endif // SQLTABLEMODEL_HPP
