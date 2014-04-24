#ifndef TYPES_H
#define TYPES_H

#pragma once

#define DataRole        Qt::UserRole + 1
#define ItemTypeRole    Qt::UserRole + 2

typedef enum
{
    Implants,
    Abutments
}ModelType;

typedef enum
{
    RootItem,
    ProviderItem,
    SeriesItem
}ItemType;


#endif // TYPES_H
