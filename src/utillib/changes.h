#ifndef UTILLIB_CHANGES_H
#define UTILLIB_CHANGES_H

struct utillib_changes {
  bool * changes;
};

void utillib_changes_init(struct utillib_changes *self, size_t N);
void utillib_changes_set(struct utillib_changes * self, size_t pos);

