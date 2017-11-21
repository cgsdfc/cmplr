#include "scanner.h"

using namespace utillib;

// SymbolScanner
SymbolScanner::SymbolScanner(const size_t * symbols, struct utillib_symbol const * table) noexcept {
  utillib_symbol_scanner_init(&scanner_, symbols, table);
}

std::size_t SymbolScanner::Lookahead() {
  return utillib_symbol_scanner_lookahead(&scanner_);
}

void SymbolScanner::Shiftaway() {
  utillib_symbol_scanner_shiftaway(&scanner_);
}

void const* SymbolScanner::Semantic(){
  return utillib_symbol_scanner_semantic(&scanner_);
}

bool SymbolScanner::Reacheof() {
  return utillib_symbol_scanner_reacheof(&scanner_);
}

struct utillib_scanner_op const *SymbolScanner::GetOp() {
  return &utillib_symbol_scanner_op;
}

// CharScanner
CharScanner::CharScanner(std::FILE *file) {
  utillib_char_scanner_init(&scanner_, file);
}

std::size_t CharScanner::Lookahead() {
  return utillib_char_scanner_lookahead(&scanner_);
}

void CharScanner::Shiftaway() {
  utillib_char_scanner_shiftaway(&scanner_);
}

void const* CharScanner::Semantic(){
  return nullptr;
}

bool CharScanner::Reacheof() {
  return utillib_char_scanner_reacheof(&scanner_);
}

struct utillib_scanner_op const *CharScanner::GetOp() {
  return nullptr;
}

CharScanner::~CharScanner() noexcept {
  utillib_char_scanner_destroy(&scanner_);
}

std::pair<std::size_t, std::size_t> CharScanner::GetLocation() const {
  return std::make_pair(scanner_.row, scanner_.col);
}
